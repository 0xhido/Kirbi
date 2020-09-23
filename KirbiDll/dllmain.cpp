// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include "InlineHook.h"

InlineHook* g_InlineHook;

typedef
NTSYSCALLAPI
NTSTATUS
(*NtCreateThreadExPtr)(
    OUT PHANDLE hThread,
    IN ACCESS_MASK DesiredAccess,
    IN PVOID ObjectAttributes,
    IN HANDLE ProcessHandle,
    IN PVOID lpStartAddress,
    IN PVOID lpParameter,
    IN ULONG Flags,
    IN SIZE_T StackZeroBits,
    IN SIZE_T SizeOfStackCommit,
    IN SIZE_T SizeOfStackReserve,
    OUT PVOID lpBytesBuffer
    );

NTSTATUS
NTAPI
NtCreateThreadExHook(
    OUT PHANDLE hThread,
    IN ACCESS_MASK DesiredAccess,
    IN PVOID ObjectAttributes,
    IN HANDLE ProcessHandle,
    IN PVOID lpStartAddress,
    IN PVOID lpParameter,
    IN ULONG Flags,
    IN SIZE_T StackZeroBits,
    IN SIZE_T SizeOfStackCommit,
    IN SIZE_T SizeOfStackReserve,
    OUT PVOID lpBytesBuffer
)
{
    MessageBoxA(nullptr, "NtCreateThreadEx has been called.", "Hi!", 0);

    NTSTATUS result = reinterpret_cast<NtCreateThreadExPtr>(
        g_InlineHook->GetDuplicateFunction())(
            hThread,
            DesiredAccess,
            ObjectAttributes,
            ProcessHandle,
            lpStartAddress,
            lpParameter,
            Flags,
            StackZeroBits,
            SizeOfStackCommit,
            SizeOfStackReserve,
            lpBytesBuffer
            );

    return result;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        try
        {
            MessageBoxA(nullptr, "DLL Loaded!", "Hi!", 0);

            HMODULE ntdll = GetModuleHandleA("ntdll");
            if (ntdll == NULL) {
                throw std::exception("[NTDLL] Module invalid handle");
            }

            PVOID originalFunction = GetProcAddress(ntdll, "NtCreateThreadEx");
            if (originalFunction == NULL) {
                throw std::exception("[GetProcAddress] Failed");
            }

            g_InlineHook = new InlineHook(originalFunction, NtCreateThreadExHook);
        }
        catch (const std::exception&)
        {
            return false;
        }

        break;
        
    case DLL_PROCESS_DETACH:
        if (g_InlineHook) {
            delete g_InlineHook;
        }

        break;

    default:
        break;
    }

    return TRUE;
}