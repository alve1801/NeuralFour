#pragma once

#ifndef FFLAG_H
#define FFLAG_H

#include "boost/serialization/access.hpp"



template<typename Type>
struct FFlag
{
private:
	Type Flag;

public:

	FFlag()
	{
		Flag = 0;
	}

	template<typename T>
	FFlag(T InFlag)
	{
		Flag = Type(InFlag);
	}

	FFlag(const FFlag& InFlag)
	{
		Flag = InFlag.Flag;
	}

	void Set(Type NewFlags)
	{
		Flag = NewFlags;
	}

	void Add(Type NewFlags)
	{
		Flag = NewFlags | Flag;
	}

	void Remove(Type RemoveFlags)
	{
		Flag -= RemoveFlags & Flag;
	}

	bool HasAll(Type CheckFlags) const
	{
		return ((Flag & CheckFlags) == CheckFlags);
	}

	bool Has(Type CheckFlags) const
	{
		return (Flag & CheckFlags) != 0;
	}



	bool HasAll(const FFlag CheckFlags) const
	{
		return ((Flag & CheckFlags.Flag) == CheckFlags.Flag);
	}

	bool Has(const FFlag OtherFlag) const
	{
		return (Flag & OtherFlag.Flag) != 0;
	}

	void Set(const FFlag NewFlags)
	{
		Flag = NewFlags.Flag;
	}

	void Add(const FFlag NewFlags)
	{
		Flag = NewFlags.Flag | Flag;
	}

	void Remove(const FFlag RemoveFlags)
	{
		Flag -= RemoveFlags.Flag & Flag;
	}


	Type GetFlag() const
	{
		return Flag;
	}

	void Reset()
	{
		Flag = 0;
	}


	void operator= (Type Val)
	{
		Flag = Val;
	}

	void operator= (FFlag InFlag)
	{
		Flag = InFlag.Flag;
	}

private:
	friend boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & Flag;
	}
};

#endif