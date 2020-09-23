#include "InlineHook.h"

InlineHook::InlineHook(PVOID originalFunction, PVOID hookFunction)
	: _originalFunction(originalFunction), _hookFunction(hookFunction) {
	
	DuplicateOriginal();
	InsertTrampoline();
}

InlineHook::~InlineHook() {
	try
	{
		RestoreOriginal();
	}
	catch (const std::exception&)
	{
		// ignore
	}
}

void InlineHook::DuplicateOriginal() {
	_duplicateFunction = VirtualAlloc(
		nullptr,
		FUNCTION_SIZE,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);
	if (_duplicateFunction == nullptr) {
		throw std::exception("[Duplication Failed] Could not allocate memory");
	}

	memcpy_s(_duplicateFunction, FUNCTION_SIZE, _originalFunction, FUNCTION_SIZE);
}

void InlineHook::InsertTrampoline() {
	std::vector<UCHAR> jmpShellcode;

	for (const auto byte : TRAMPOLINE_SHELLCODE) {
		jmpShellcode.push_back(static_cast<UCHAR>(byte));
	}

	// Fill in the hook function address
	memcpy_s(&jmpShellcode[2], sizeof(PVOID), &_hookFunction, sizeof(PVOID));

	DWORD oldProtect;

	BOOL ok = VirtualProtect(
		_originalFunction,
		jmpShellcode.size(),
		PAGE_EXECUTE_READWRITE,
		&oldProtect
	);
	if (!ok) {
		throw std::exception("[Shellcode Failed] Could not change page protection");
	}

	// Insert instruction to the original function
	memcpy_s(_originalFunction, jmpShellcode.size(), jmpShellcode.data(), jmpShellcode.size());

	// restore page protection
	VirtualProtect(
		_originalFunction,
		jmpShellcode.size(),
		oldProtect,
		&oldProtect // not relevant
	);
}

void InlineHook::RestoreOriginal() {
	DWORD oldProtection;

	BOOL ok = VirtualProtect(
		_originalFunction,
		FUNCTION_SIZE,
		PAGE_EXECUTE_READWRITE,
		&oldProtection 
	);
	if (!ok) {
		throw std::exception("[Restore Failed] Could not change page protection");
	}

	memcpy_s(_originalFunction, FUNCTION_SIZE, _duplicateFunction, FUNCTION_SIZE);

	VirtualProtect(
		_originalFunction,
		FUNCTION_SIZE,
		oldProtection,
		&oldProtection // not relevant
	);

	VirtualFree(
		_duplicateFunction,
		0,
		MEM_RELEASE
	);
}