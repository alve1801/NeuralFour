#include "stdafx.h"
#include "ASmartMutex.h"
#include "Utils.h"
#include "VersionProperties.h"
#include "Debugs.h"


ASmartMutex::ASmartMutex(): ReadLocks(0), bWriteLocked(false)
{
}


ASmartMutex::~ASmartMutex()
{
	DELETE_IF_EXISTING(PtrReadLock);
	DELETE_IF_EXISTING(PtrWriteLock);
}

bool ASmartMutex::ReadLock()
{
	if (bWriteLocked &&  Globals::ThreadInfos.get() == WriteLockThread)
	{
		return false;
	}
	ReadLocks++;

	RETURN_IF(ReadLocks > 1) true;
	PtrReadLock = new AReadLock(Mutex);
	return true;
}

bool ASmartMutex::WriteLock()
{
	if (bWriteLocked &&  Globals::ThreadInfos.get() == WriteLockThread)
	{
		return false;
	}

	AWriteLock* Buffer = new AWriteLock(Mutex);
	
	PtrWriteLock = Buffer;
	bWriteLocked = true;
	WriteLockThread = Globals::ThreadInfos.get();
	return true;
}

bool ASmartMutex::ReadUnlock()
{
	RETURN_IF(ReadLocks == 0) false;
	ReadLocks--;
	if (ReadLocks == 0)
	{
		AReadLock* Buffer = PtrReadLock;
		PtrReadLock = nullptr;
		delete Buffer;
	}
	return true;
}

bool ASmartMutex::WriteUnlock()
{
	RETURN_IF(!bWriteLocked) false;
	bWriteLocked = false;

	AWriteLock* Buffer = PtrWriteLock;
	PtrWriteLock = nullptr;
	delete Buffer;

	WriteLockThread = nullptr;
	return true;
}

