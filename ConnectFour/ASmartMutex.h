
#ifndef ASMARTMUTEX_H
#define ASMARTMUTEX_H
#pragma once
#include "boost/thread.hpp"
#include "Utils.h"
#include "AThreadInfo.h"


typedef boost::unique_lock<boost::shared_mutex> AWriteLock;
typedef boost::shared_lock<boost::shared_mutex> AReadLock;

class ASmartReadLock;
class ASmartWriteLock;

class ASmartMutex
{
public:
	friend class ASmartReadLock;
	friend class ASmartWriteLock;
	ASmartMutex();
	~ASmartMutex();



private:
	bool ReadLock();
	bool WriteLock();
	bool ReadUnlock();
	bool WriteUnlock();


	AReadLock* PtrReadLock = nullptr;
	AWriteLock* PtrWriteLock = nullptr;



	boost::shared_mutex Mutex;
	AThreadInfo* WriteLockThread;

	unsigned short ReadLocks;
	bool bWriteLocked;

public:

	bool IsLocked() const
	{
		return bWriteLocked || ReadLocks != 0;
	}

	bool IsReadLocked() const
	{
		return ReadLocks != 0;
	}

	bool IsWriteLocked() const
	{
		return bWriteLocked;
	}

	unsigned short GetReadLockCount() const
	{
		return ReadLocks;
	}
};

class ASmartReadLock
{
public:
	
	explicit ASmartReadLock(ASmartMutex& InMutex) : Mutex(&InMutex)
	{
		bLocked = Mutex->ReadLock();
	}

	explicit ASmartReadLock(ASmartMutex* InMutex) : Mutex(InMutex)
	{
		bLocked = Mutex->ReadLock();
	}

	~ASmartReadLock()
	{
		Unlock();
	}

	void Lock()
	{
		if (bLocked)
		{
			return;
		}
		Mutex->ReadLock();
		bLocked = true;
	}

	void Unlock()
	{
		if (!bLocked)
		{
			return;
		}
		Mutex->ReadUnlock();
		bLocked = false;
	}
private:
	bool bLocked = true;
	ASmartMutex* Mutex;
};

class ASmartWriteLock
{
public:

	explicit ASmartWriteLock(ASmartMutex& InMutex) : Mutex(&InMutex)
	{
		bLocked = Mutex->WriteLock();
	}

	explicit ASmartWriteLock(ASmartMutex* InMutex) : Mutex(InMutex)
	{
		bLocked = Mutex->WriteLock();
	}

	~ASmartWriteLock()
	{
		Unlock();
	}

	void Lock()
	{
		if (bLocked)
		{
			return;
		}
		Mutex->WriteLock();
		bLocked = true;
	}

	void Unlock()
	{
		if (!bLocked)
		{
			return;
		}
		Mutex->WriteUnlock();
		bLocked = false;
	}

private:
	bool bLocked = true;
	ASmartMutex* Mutex;
};

#endif