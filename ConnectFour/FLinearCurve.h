
#ifndef FLINEARCURVE_H
#define FLINEARCURVE_H
#pragma once
#include <vector>

namespace
{
	int Lerp(int A, int B, float Alpha)
	{
		return int(A + Alpha * float(B - A));
	}
}


struct FLinearCurve
{
	struct FEntry
	{
		int x;
		int y;

		FEntry::FEntry() : x(0), y(0)
		{
		}

		FEntry::FEntry(int InX, int InY)
		{
			x = InX;
			y = InY;
		}
	};


private:
	std::vector<FEntry> Entries;
public:
	FLinearCurve::FLinearCurve()
	{
	}

	bool AddEntry(int InX, int InY)
	{
		int Size = int(Entries.size());
		if (Size == 0)
		{
			Entries.push_back(FEntry(InX, InY));
		}
		for (int Index = 0; Index < Size; Index++)
		{
			if (Entries[Index].x == InX)
			{
				return false;
			}
			else if (Entries[Index].x > InX)
			{
				Entries.insert(Entries.begin() + Index, FEntry(InX, InY));
				return true;
			}
			else if (Index == Size - 1)
			{
				Entries.push_back(FEntry(InX, InY));
				return true;
			}
		}
		return false;
	}

	int GetValue(int InX)
	{
		int Size = int(Entries.size());

		if (Size == 0)
		{
			return 0;
		}

		if (Size == 1 || InX <= Entries[0].x)
		{
			return Entries[0].y;
		}

		if (InX >= Entries[Size - 1].x)
		{
			return Entries[Size - 1].y;
		}

		for (int Index = 0; Index < Size - 1; Index++)
		{
			if (Entries[Index].x == InX)
			{
				return Entries[Index].y;
			}
			if (InX < Entries[Index + 1].x)
			{
				return Lerp(float(Entries[Index].y), float(Entries[Index + 1].y), (InX - Entries[Index].x) / float(Entries[Index + 1].x - Entries[Index].x));
			}
		}
		return 0;
	}
};


#endif