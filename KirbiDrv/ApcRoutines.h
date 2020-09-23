#pragma once

#include <ntifs.h>

#include "Undocumented.h"

extern EX_RUNDOWN_REF g_RundownProtection;

void KernelFreeKApc(
	IN PKAPC Apc,
	IN OUT PKNORMAL_ROUTINE* NormalRoutine,
	IN OUT PVOID* NormalContext,
	IN OUT PVOID* SystemArgument1,
	IN OUT PVOID* SystemArgument2
);

void RundownFreeKApc(
	IN  PKAPC Apc
);

void NormalInjectCode(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
);