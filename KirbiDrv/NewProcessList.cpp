#include "NewProcessList.h"

void NewProcessList::AddProcess(ULONG ProcessId) {
	AutoLock<FastMutex> locker(_processes.GetMutex());

	ProcessEntry* entry = (ProcessEntry*)ExAllocatePoolWithTag(PagedPool, sizeof(ProcessEntry), DRIVER_TAG);
	if (entry == nullptr) {
		DbgPrint(DRIVER_PREFIX "Failed to allocate memory for new process entry\n");
		return;
	}

	entry->ProcessId = ProcessId;
	_processes.Insert(&entry->Entry);
}

void NewProcessList::RemoveProcess(ULONG ProcessId) {
	AutoLock<FastMutex> locker(_processes.GetMutex());

	auto head = _processes.GetHead();
	auto current = head->Flink;

	while (current != head) {
		auto item = CONTAINING_RECORD(current, ProcessEntry, Entry);
		
		if (item->ProcessId == ProcessId) {
			_processes.Remove(current);
			return;
		}

		current = current->Flink;
	}
}

bool NewProcessList::IsNewProcess(ULONG ProcessId) {
	AutoLock<FastMutex> locker(_processes.GetMutex());

	auto head = _processes.GetHead();
	auto current = head->Flink;

	while (current != head) {
		auto item = CONTAINING_RECORD(current, ProcessEntry, Entry);

		if (item->ProcessId == ProcessId) {
			return true;
		}

		current = current->Flink;
	}

	return false;
}