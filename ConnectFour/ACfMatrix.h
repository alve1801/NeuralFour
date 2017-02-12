#pragma once
#include <vector>
#include "Definitions.h"
#include "FVector.h"

#define MATRIX_WIDTH 7
#define MATRIX_HEIGHT 6

struct ACfMatrix
{

	std::vector<UChar> Matrix;
	std::vector<UChar> Heights;


	ACfMatrix()
	{
		Matrix.resize(MATRIX_WIDTH* MATRIX_HEIGHT, 0);
		Heights.resize(MATRIX_WIDTH, 0);
	}

	template<typename T>
	static UChar GetIndex(FVector<T> Position) 
	{
		return Position.y * MATRIX_WIDTH + Position.x;
	}

	/*template<typename T>
	static FVector<UChar> GetPosition(T Index)
	{
		return FVector<UChar>(Index/5, Index%5);
	}*/

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
		RETURN_IF(Heights[Position] == MATRIX_HEIGHT) false;
		return true;
	}

	bool Insert(UChar Position, UChar Id)
	{
		RETURN_IF(Heights[Position] == MATRIX_HEIGHT) false;
		Matrix[Position  + Heights[Position] * MATRIX_WIDTH] = Id;
		Heights[Position]++;
		return true;
	}

	void Reset()
	{
		for (UChar Index = 0; Index < MATRIX_HEIGHT* MATRIX_WIDTH; ++Index)
		{
			Matrix[Index] = 0;
		}
		for (UChar Index = 0; Index < MATRIX_WIDTH; ++Index)
		{
			Heights[Index] = 0;
		}
	}
};
