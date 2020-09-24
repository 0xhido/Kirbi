#pragma once

#include <ntddk.h>

//
//	Define
//

#define KRB_DLL_X64_NAME L"krbdllx64.dll"

//
//	Enumerations
//

enum KRB_ARCHITECTURE {
	KrbArchitechtureX86,
	KrbArchitechtureX64,
	KrbArchitechtureARM32,
	KrbArchitechtureARM64,
	KrbArchitechtureMax,

#if defined(_M_IX86)
	KrbArchitectureNative = KrbArchitechtureX86
#elif defined(_M_AMD64)
	KrbArchitectureNative = KrbArchitechtureX64
#elif defined(_M_ARM)
	KrbArchitectureNative = KrbArchitechtureARM32
#elif defined(_M_ARM64)
	KrbArchitectureNative = KrbArchitechtureARM64
#endif
};

enum KRB_METHOD {
	/// <summary>
	/// Injecting a "shellcode" which calls LdrLoadDll
	/// </summary>
	KrbMethodThunk
};

//
//	Structs
//

typedef struct _KRB_SETTINGS {
	/// <summary>
	/// Path to the injected DLLs
	/// Unsupported architectures have empty string
	/// </summary>
	UNICODE_STRING DllPath[KrbArchitechtureMax];

	/// <summary>
	/// Injected method
	/// </summary>
	KRB_METHOD Method;
} KRB_SETTINGS, *PKRB_SETTINGS;

//
//	Help functions
//

/// <summary>
/// 
/// </summary>
/// <param name="RegistryPath"></param>
/// <param name="Settings"></param>
/// <returns></returns>
NTSTATUS
NTAPI
KrbCreateSettings(
	_In_ PUNICODE_STRING RegistryPath,
	_Inout_ PKRB_SETTINGS Settings
);
