#pragma once
#include <vector>
#include "Definitions.h"
#include "FVector.h"


struct ACfMatrix
{

	std::vector<UChar> Matrix;
	std::vector<UChar> Heights;


	ACfMatrix()
	{
		Matrix.resize(25, 0);
		Heights.resize(5, 0);
	}

	template<typename T>
	static UChar GetIndex(FVector<T> Position) 
	{
		return Position.y * 5 + Position.x;
	}

	template<typename T>
	static FVector<UChar> GetPosition(T Index)
	{
		return FVector<UChar>(Index/5, Index%5);
	}

	UChar& operator[](UChar Index)
	{
		return Matrix[Index];
	}

	template<typename T>
	UChar& operator[](FVector<T> Position)
	{
		return Matrix[GetIndex(Position)];
	}

	bool HasSpace(UChar Position)
	{
		RETURN_IF(Heights[Position] == 5) false;
		return true;
	}

	bool Insert(UChar Position, UChar Id)
	{
		RETURN_IF(Heights[Position] == 5) false;
		Matrix[Position  + Heights[Position] * 5] = Id;
		Heights[Position]++;
		return true;
	}

	void Reset()
	{
		for (UChar Index = 0; Index < 25; ++Index)
		{
			Matrix[Index] = 0;
		}
		for (UChar Index = 0; Index < 5; ++Index)
		{
			Heights[Index] = 0;
		}
	}
};
