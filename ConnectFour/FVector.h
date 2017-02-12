#pragma once


#ifndef FVECTOR_H
#define FVECTOR_H

#include <SDL.h>
#include "VersionProperties.h"

// ReSharper disable once CppUnusedIncludeDirective
#if LIB_OPENCV
#include "opencv2/core/mat.hpp"
#endif
#include "Definitions.h"

template<typename Type>
struct FVector
{
	Type x;
	Type y;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/)
	{
		ar & x;
		ar & y;
	}

	FVector() : x(0), y(0)
	{
	}

	explicit FVector(Type Valuexy) : x(Type(Valuexy)), y(Type(Valuexy))
	{
	}

	explicit FVector(Type Valuex, Type Valuey) : x(Type(Valuex)), y(Type(Valuey))
	{
	}

	template<typename T>
	explicit FVector(T Valuex, T Valuey) : x(Type(Valuex)), y(Type(Valuey))
	{
	}

	template<typename T1, typename T2>
	explicit FVector(T1 Valuex, T2 Valuey) : x(Type(Valuex)), y(Type(Valuey))
	{
	}

	FVector(FVector<Type> const& InVector) : x(InVector.x), y(InVector.y)
	{
	}

	template<typename T>
	explicit FVector(FVector<T> const& InVector) : x(Type(InVector.x)), y(Type(InVector.y))
	{
	}

	explicit FVector(SDL_Point InPoint) : x(Type(InPoint.x)), y(Type(InPoint.y))
	{
	}


	FVector operator+(const FVector P) const
	{
		return FVector<Type>(x + P.x, y + P.y);
	}

	FVector operator-(const FVector P) const
	{
		return FVector(x - P.x, y - P.y);
	}

	FVector operator-() const
	{
		return FVector(-x, -y);
	}


	template<typename T>
	FVector operator*(const T Factor) const
	{
		return FVector(Type(x * Factor), Type(y * Factor));
	}

	template<>
	FVector operator*<FVector>(const FVector Factor) const
	{
		return FVector(x * Factor.x, y * Factor.y);
	}

	template<typename T>
	FVector operator/(const T Dividor) const
	{
		return FVector(x / Dividor, y / Dividor);
	}

	template<>
	FVector operator/<FVector>(const FVector Factor) const
	{
		return FVector(x / Factor.x, y / Factor.y);
	}


	void operator+=(const FVector P)
	{
		x += P.x;
		y += P.y;
	}

	void operator-=(const FVector P)
	{
		x -= P.x;
		y -= P.y;
	}

	bool operator==(const FVector P) const
	{
		return x == P.x && y == P.y;
	}

	bool operator!=(const FVector P) const
	{
		return x != P.x || y != P.y;
	}

	template<typename NewType>
	FVector<NewType> Cast() const
	{
		return FVector<NewType>(NewType(x), NewType(y));
	}

	template<typename TR>
	static FVector<TR> Lerp(FVector A, FVector B, float Alpha)
	{
		return FVector(TR(A.x + Alpha * (B.x - A.x)), TR(A.y + Alpha * (B.y - A.y)));
	}

	FVector<Type> GetSwap() const
	{
		return FVector<Type>(y, x);
	}

	template<typename TR>
	TR GetLength() const
	{
		return TR(sqrt(pow(x, 2) + pow(y, 2)));
	}

	template<typename TR, typename T>
	TR GetDistance(FVector<T>& P) const
	{
		return (*this - P).template GetLength<TR>();
	}

	FVector<Type> GetNormalized() const
	{
		return (*this) * Type(1 / GetLength<Type>());
	}

	FVector<Type> GetFast() const
	{
		FVector<Type> Temp = GetNormalized();
		Type Factor = 1;
		if (abs(Temp.x) > abs(Temp.y))
		{
			Factor = 1.f / abs(Temp.x);
		}
		else
		{
			Factor = 1.f / abs(Temp.y);
		}
		return Temp * Factor;
	}

	FVector<Type> GetOrthogonal() const
	{
		return FVector<Type>(y, -x);
	}

	FVector<int> GetRounded() const
	{
		return FVector<int>(int(roundf(x)), int(roundf(y)));
	}

	FVector<Type> GetRotated(double Degrees) const
	{
		Degrees *= Constants::Pi / 180;
		return FVector<Type>(Type(x * cos(Degrees) - y * sin(Degrees)), Type(x * sin(Degrees) + y * cos(Degrees)));
	}

	template<typename TR>
	TR GetRotation() const
	{
		return TR(atan2(y, x) * 180 / Constants::Pi);
	}


	SDL_Point ToSDL() const
	{
		SDL_Point NewPoint;
		NewPoint.x = x;
		NewPoint.y = y;
		return NewPoint;
	}
#if LIB_OPENCV
	cv::Point ToCV() const
	{
		return cv::Point(x, y);
	}
#endif
};


typedef FVector<int> FPoint;
typedef FVector<int> FSize;


typedef FVector<long double> FLongVector;





#endif