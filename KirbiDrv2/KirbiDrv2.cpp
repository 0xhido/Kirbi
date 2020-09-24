#include "KirbiDrv2.h"

//
// Taken from ReactOS, used by InjpInitializeDllPaths.
//

typedef union
{
	WCHAR Name[sizeof(ULARGE_INTEGER) / sizeof(WCHAR)];
	ULARGE_INTEGER Alignment;
} ALIGNEDNAME;

//
// DOS Device Prefix \??\
//

ALIGNEDNAME ObpDosDevicesShortNamePrefix = { { L'\\', L'?', L'?', L'\\' } };
UNICODE_STRING ObpDosDevicesShortName = {
  sizeof(ObpDosDevicesShortNamePrefix), // Length
  sizeof(ObpDosDevicesShortNamePrefix), // MaximumLength
  (PWSTR)&ObpDosDevicesShortNamePrefix  // Buffer
};

NTSTATUS
NTAPI
KrbCreateSettings(
	_In_ PUNICODE_STRING RegistryPath,
	_Inout_ PKRB_SETTINGS Settings
) {
	NTSTATUS status;

	// "ImagePath" contains the full path of the driver file
	UNICODE_STRING ValueName = RTL_CONSTANT_STRING(L"ImagePath");

	OBJECT_ATTRIBUTES ObjectAttributes;
	InitializeObjectAttributes(
		&ObjectAttributes,
		RegistryPath,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
		NULL,
		nullptr
	);

	HANDLE hKey;
	status = ZwOpenKey(
		&hKey,
		KEY_READ,
		&ObjectAttributes
	);
	if (!NT_SUCCESS(status)) {
		return status;
	}

	UCHAR KeyValueInformationBuffer[sizeof(KEY_VALUE_FULL_INFORMATION) + sizeof(WCHAR) * 128];
	PKEY_VALUE_FULL_INFORMATION KeyValueInfo = (PKEY_VALUE_FULL_INFORMATION)KeyValueInformationBuffer;

	ULONG ResultLength;
	status = ZwQueryValueKey(
		hKey,
		&ValueName,
		KeyValueFullInformation,
		KeyValueInfo,
		sizeof(KeyValueInformationBuffer),
		&ResultLength
	);

	// Close handle in any case 
	ZwClose(hKey);

	if (!NT_SUCCESS(status)) {
		return status;
	}

	// Check length
	if (KeyValueInfo->Type != REG_EXPAND_SZ ||
		KeyValueInfo->DataLength < sizeof(ObpDosDevicesShortNamePrefix)) {

		return STATUS_BAD_DATA;
	}

	PWCHAR ImagePathValue = (PWCHAR)((PUCHAR)KeyValueInfo + KeyValueInfo->DataOffset);
	ULONG ImagePathValueLength = KeyValueInfo->DataLength;


	// Check if ImagePath starts with "\??\" and if so, skip it
	if (*(PULONGLONG)(ImagePathValue) == ObpDosDevicesShortNamePrefix.Alignment.QuadPart) {
		ImagePathValue += ObpDosDevicesShortName.Length / sizeof(WCHAR);
		ImagePathValueLength -= ObpDosDevicesShortName.Length;
	}

	// Extract directory path by cutting the part after the last "\"
	PWCHAR LastBackslash = wcsrchr(ImagePathValue, L'\\');
	if (!LastBackslash) {
		return STATUS_DATA_ERROR;
	}

	*LastBackslash = UNICODE_NULL;

	UNICODE_STRING Directory;
	RtlInitUnicodeString(&Directory, ImagePathValue);

	// Fill DLLs buffers
	UNICODE_STRING KrbDllNameX64 = RTL_CONSTANT_STRING(KRB_DLL_X64_NAME);






}

VOID
NTAPI
DriverUnload(
	_In_ PDRIVER_OBJECT DriverObject
) {
}

extern "C"
NTSTATUS
NTAPI
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath
) {
	NTSTATUS status;

	// Sets unload routine
	DriverObject->DriverUnload = DriverUnload;

	// Create injection settings
	KRB_SETTINGS Settings;

	// TODO add more architecture DllPaths support
	WCHAR BufferDllPathX64[128];
	Settings.DllPath[KrbArchitechtureX64].Length = 0;
	Settings.DllPath[KrbArchitechtureX64].MaximumLength = sizeof(BufferDllPathX64);
	Settings.DllPath[KrbArchitechtureX64].Buffer = BufferDllPathX64;



}