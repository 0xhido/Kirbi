#pragma once

#include <ntddk.h>
#include "FastMutex.h"

#define MAX_ITEMS 1024

class List
{
public:
	void Init();
	void Destroy();

	void Insert(PLIST_ENTRY entry);
	void Remove(PLIST_ENTRY entry);
	PLIST_ENTRY RemoveTail();

	PLIST_ENTRY GetHead();
	FastMutex& GetMutex();
	int GetCount();

private:
	LIST_ENTRY _head;
	FastMutex _mutex;
	int _count;
};

