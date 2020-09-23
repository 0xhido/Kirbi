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

	::ExReleaseRundownProtection(&g_RundownProtection);
}

void NormalInjectCode(
	IN PVOID NormalContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
) {

}