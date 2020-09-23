#include "NotifyRoutines.h"
#include "ApcRoutines.h"
#include "Undocumented.h"

VOID
OnProcessNotify(
    _In_ HANDLE ParentId,
    _In_ HANDLE ProcessId,
    _In_ BOOLEAN Create
) {
    UNREFERENCED_PARAMETER(ParentId);

    if (Create) {
        PEPROCESS process;
        
        NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &process);
        if (NT_SUCCESS(status)) {
            if (!::PsGetProcessWow64Process(process) && !::PsIsProtectedProcess(process)) {
                g_NewProcessList.AddProcess(HandleToULong(ProcessId));
            }
        }
    }
    else {
        g_NewProcessList.RemoveProcess(HandleToULong(ProcessId));
    }

    DbgPrint("[Process] %d %s\n", HandleToULong(ProcessId), Create ? "created" : "exit");
}

VOID
OnThreadNotify(
    _In_ HANDLE ProcessId,
    _In_ HANDLE ThreadId,
    _In_ BOOLEAN Create
) {
    ULONG pid = HandleToULong(ProcessId);

    if (Create && g_NewProcessList.IsNewProcess(pid)) {
        g_NewProcessList.RemoveProcess(pid); // No longer new process

        PETHREAD thread;
        NTSTATUS status = ::PsLookupThreadByThreadId(ThreadId, &thread);
        if (!NT_SUCCESS(status)) {
            DbgPrint(DRIVER_PREFIX "Failed to find thread\n");
            return;
        }

        PKAPC apc = (PKAPC)ExAllocatePoolWithTag(PagedPool, sizeof(KAPC), DRIVER_TAG);
        if (apc == nullptr) {
            DbgPrint(DRIVER_PREFIX "Failed to allocate memory for KAPC\n");
            return;
        }

        KeInitializeApc(
            apc,
            thread,
            OriginalApcEnvironment,
            KernelFreeKApc,
            RundownFreeKApc,
            NormalInjectCode,
            KernelMode,
            nullptr
        );

        if (ExAcquireRundownProtection(&g_RundownProtection)) {
            BOOLEAN inserted = KeInsertQueueApc(
                apc,
                nullptr,
                nullptr,
                0
            );
            if (!inserted) {
                ExFreePoolWithTag(apc, DRIVER_TAG);
                ExReleaseRundownProtection(&g_RundownProtection);

                KdPrint((DRIVER_PREFIX "[Injector] KernelMode APC insertion failed (%X)\n", status));

                return;
            }
        }
    }
}

VOID
OnLoadImageNotify(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,                // pid into which image is being mapped
    _In_ PIMAGE_INFO ImageInfo
) {
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ImageInfo);

    if (FullImageName) {
        KdPrint((
            DRIVER_PREFIX "%wZ loaded %s\n",
            FullImageName,
            PsGetProcessImageFileName(PsGetCurrentProcess())
            ));
    }
}