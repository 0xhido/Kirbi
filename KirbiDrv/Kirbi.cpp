#include "Kirbi.h"
#include "NotifyRoutines.h"
#include "NewProcessList.h"

bool g_ProcessNotifyCreated = false;
bool g_ThreadNotifyCreated = false;

NewProcessList* g_NewProcessList = nullptr;
EX_RUNDOWN_REF g_RundownProtection;

VOID
DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject
) {
	UNREFERENCED_PARAMETER(DriverObject);

	if (g_ProcessNotifyCreated)
		PsSetCreateProcessNotifyRoutine(OnProcessNotify, TRUE);
	if (g_ThreadNotifyCreated)
		PsRemoveCreateThreadNotifyRoutine(OnThreadNotify);

	::ExWaitForRundownProtectionRelease(&g_RundownProtection);
}


extern "C"
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS status = STATUS_SUCCESS;

	DriverObject->DriverUnload = DriverUnload;

	g_NewProcessList = (NewProcessList*)ExAllocatePoolWithTag(PagedPool, sizeof(NewProcessList), DRIVER_TAG);
	if (g_NewProcessList == nullptr) {
		DbgPrint(DRIVER_PREFIX "Failed allocate memory for new process list\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	status = PsSetCreateProcessNotifyRoutine(OnProcessNotify, FALSE);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	g_ProcessNotifyCreated = true;


	status = PsSetCreateThreadNotifyRoutine(OnThreadNotify);
	if (!NT_SUCCESS(status))
	{
		PsSetCreateProcessNotifyRoutine(OnProcessNotify, TRUE);
		return status;
	}
	g_ThreadNotifyCreated = true;

	::ExInitializeRundownProtection(&g_RundownProtection);

	return STATUS_SUCCESS;
}