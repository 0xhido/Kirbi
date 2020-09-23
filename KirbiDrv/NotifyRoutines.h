#pragma once

#include <ntifs.h>
#include <ntddk.h>

#include "NewProcessList.h"

extern NewProcessList* g_NewProcessList;
extern EX_RUNDOWN_REF g_RundownProtection;

VOID
OnProcessNotify(
    _In_ HANDLE ParentId,
    _In_ HANDLE ProcessId,
    _In_ BOOLEAN Create
);

VOID
OnThreadNotify(
    _In_ HANDLE ProcessId,
    _In_ HANDLE ThreadId,
    _In_ BOOLEAN Create
);