#include "List.h"

void List::Init() {
	_count = 0;
	::InitializeListHead(&_head);
	_mutex.Init();
}

void List::Destroy() {
	while (!IsListEmpty(&_head)) {
		auto entry = ::RemoveTailList(&_head);
		ExFreePool(entry);
	}
}

void List::Insert(PLIST_ENTRY entry) {
	if (_count > MAX_ITEMS) {
		auto head = ::RemoveHeadList(&_head);
		ExFreePool(head);
		
		_count--;
	}
	
	::InsertTailList(&_head, entry);
	_count++;
;}

void List::Remove(PLIST_ENTRY entry) {
	::RemoveEntryList(entry);
	ExFreePool(entry);

	_count--;
}

PLIST_ENTRY List::RemoveTail() {
	if (_count > 0) {
		_count--;
		return ::RemoveTailList(&_head);
	}

	return nullptr;
}

PLIST_ENTRY List::GetHead() {
	return &_head;
}

FastMutex& List::GetMutex() {
	return _mutex;
}

int List::GetCount() {
	return _count;
}
