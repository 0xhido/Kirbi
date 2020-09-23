#pragma once

#include <ntddk.h>
#include "FastMutex.h"

#define MAX_ITEMS 1024

class List
{
public:
	List();
	~List();

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

