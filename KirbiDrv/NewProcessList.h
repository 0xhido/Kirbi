#pragma once

#include "Kirbi.h"

#include "List.h"
#include "AutoLock.h"

struct ProcessEntry {
	LIST_ENTRY Entry;
	ULONG ProcessId;
};

class NewProcessList
{
public:
	void Init();
	void Destroy();

	void AddProcess(ULONG ProcessId);
	void RemoveProcess(ULONG ProcessId);
	bool IsNewProcess(ULONG ProcessId);
private:
	List _processes;
};

