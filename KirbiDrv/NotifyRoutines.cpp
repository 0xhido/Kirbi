#include "NotifyRoutines.h"
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
                g_NewProcessList->AddProcess(HandleToULong(ProcessId));
            }
        }
    }
    else {
        g_NewProcessList->RemoveProcess(HandleToULong(ProcessId));
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

    if (Create && g_NewProcessList->IsNewProcess(pid)) {
        g_NewProcessList->RemoveProcess(pid); // No longer new process

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




    }
}