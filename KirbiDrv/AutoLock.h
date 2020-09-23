#pragma once

#include <ntddk.h>

template<typename T>
class AutoLock {
public:
	AutoLock(T& Lock) : _lock(Lock) {
		_lock.Lock();
	}

	~AutoLock() {
		_lock.Unlock();
	}
private:
	T& _lock;
};
