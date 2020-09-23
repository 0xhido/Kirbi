#pragma once

#include <Windows.h>
#include <exception>
#include <vector>
#include <cstddef>

const size_t FUNCTION_SIZE = 25;

const char TRAMPOLINE_SHELLCODE[] =
"\x48\xb8\x00\x00\x00\x00\x00\x00\x00\x00"
"\xff\xe0";


class InlineHook
{
public:
	InlineHook(PVOID originalFunciton, PVOID hookFunction);
	~InlineHook();

	InlineHook(const InlineHook&) = delete;
	InlineHook& operator=(const InlineHook&) = delete;

	PVOID GetOriginalFunction() { return _originalFunction; }
	PVOID GetDuplicateFunction() { return _duplicateFunction; }

private:
	PVOID _originalFunction;
	PVOID _hookFunction;
	PVOID _duplicateFunction;

	void DuplicateOriginal();
	void InsertTrampoline();
	void RestoreOriginal();
};

