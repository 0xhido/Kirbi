#include "ApcRoutines.h"

void KernelFreeKApc(
	IN PKAPC Apc,
	IN OUT PKNORMAL_ROUTINE* NormalRoutine,
	IN OUT PVOID* NormalContext,
	IN OUT PVOID* SystemArgument1,
	IN OUT PVOID* SystemArgument2
) {
	UNREFERENCED_PARAMETER(NormalRoutine);
	UNREFERENCED_PARAMETER(NormalContext);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);

	ExFreePool(Apc);
}

void RundownFreeKApc(
	IN  PKAPC Apc
) {
	ExFreePool(Apc);

	ExReleaseRundownProtection(&g_RundownProtection);
}

void NormalInjectCode(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
) {
	UNREFERENCED_PARAMETER(NormalContext);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);

	PVOID address = nullptr;
	SIZE_T shellcodeSize = sizeof(InjectedShellcode);

	NTSTATUS status = ZwAllocateVirtualMemory(
		NtCurrentProcess(),
		&address,
		NULL,
		&shellcodeSize,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READ
	);
	if (!NT_SUCCESS(status)) {
		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] ZwAllocateVirtualMemory failed (%X)\n", status));

		return;
	}

	PMDL mdl = IoAllocateMdl(
		address,
		(ULONG)shellcodeSize,
		FALSE,
		FALSE,
		nullptr
	);
	if (mdl == nullptr) {
		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] IoAllocateMdl failed (%X)\n", status));

		return;
	}

	MmProbeAndLockPages(
		mdl,
		KernelMode,
		IoReadAccess
	);

	PVOID mappedAddress = MmMapLockedPagesSpecifyCache(
		mdl,
		KernelMode,
		MmNonCached,
		nullptr,
		FALSE,
		NormalPagePriority
	);
	if (mappedAddress == nullptr) {
		MmUnlockPages(mdl);
		IoFreeMdl(mdl);

		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] MmMapLockedPagesSpecifyCache failed\n"));

		return;
	}

	status = MmProtectMdlSystemAddress(
		mdl,
		PAGE_READWRITE
	);
	if (!NT_SUCCESS(status)) {
		MmUnmapLockedPages(mappedAddress, mdl);
		MmUnlockPages(mdl);
		IoFreeMdl(mdl);

		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] MmProtectMdlSystemAddress failed (%X)\n", status));

		return;
	}

	// Copy the shellcode into memory
	memcpy_s(mappedAddress, shellcodeSize, InjectedShellcode, shellcodeSize);

	MmUnmapLockedPages(mappedAddress, mdl);
	MmUnlockPages(mdl);
	IoFreeMdl(mdl);

	PKAPC apc = (PKAPC)ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC), DRIVER_TAG);
	if (apc == nullptr) {
		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] APC memory allocation failed (%X)\n", status));

		return;
	}

	KeInitializeApc(
		apc,
		KeGetCurrentThread(),
		OriginalApcEnvironment,
		KernelFreeKApc,
		RundownFreeKApc,
		reinterpret_cast<PKNORMAL_ROUTINE>(address),
		UserMode,
		nullptr
	);

	BOOLEAN inserted = KeInsertQueueApc(apc, nullptr, nullptr, 0);
	if (!inserted) {
		ExFreePoolWithTag(apc, DRIVER_TAG);
		ExReleaseRundownProtection(&g_RundownProtection);

		KdPrint((DRIVER_PREFIX "[Injector] UserMode APC insertion failed (%X)\n", status));

		return;
	}

	KdPrint((
		DRIVER_PREFIX "[Injector] Injected code and queued an APC successfully (pid=%d) %s\n", 
		HandleToULong(PsGetCurrentProcessId()), 
		PsGetProcessImageFileName(PsGetCurrentProcess())
		));

	ExReleaseRundownProtection(&g_RundownProtection);
}