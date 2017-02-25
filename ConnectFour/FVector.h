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
#include "Utils.h"
#include <string>


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

	template<typename NewType>
	FVector<NewType> Cast() const
	{
		return FVector<NewType>(NewType(x), NewType(y));
	}


	FVector& operator+=(const FVector& Right)
	{
		x += Right.x;
		y += Right.y;
		return *this;
	}

	FVector& operator-=(const FVector& Right)
	{
		x -= Right.x;
		y -= Right.y;
		return *this;
	}

	bool operator==(const FVector& P) const
	{
		return x == P.x && y == P.y;
	}

	bool operator!=(const FVector& P) const
	{
		return x != P.x || y != P.y;
	}



	template<typename TR>
	static FVector<TR> Lerp(FVector A, FVector B, double Alpha)
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
		return TR(sqrt(TR(Utils::Square(x) + Utils::Square(y))));
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
		return Temp * 1.0 / abs((abs(Temp.x) > abs(Temp.y)) ? Temp.x : Temp.y);
	}

	FVector<Type> GetOrthogonal() const
	{
		return FVector<Type>(y, -x);
	}

	FVector<int> GetRounded() const
	{
		return FVector<int>(int(round(x)), int(round(y)));
	}

	FVector<Type> GetRotated(double Degrees) const
	{
		Degrees *= Constants::Pi / 180;
		double Sin = sin(Degrees);
		double Cos = cos(Degrees);
		Type Back = Type((Type(double(x) * Sin)) + (Type(double(y) * Cos)));
		Type Front = Type((Type(double(x) * Cos)) - (Type(double(y) * Sin)));
		return FVector<Type>(Front, Back);
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


	std::ostream& Dump(std::ostream &os, const FVector& p)
	{
		return os << '[' << x << '|' << y << ']';
	}

	string GetString()
	{
		return'[' + to_string(x) + '|' + to_string(y) + ']';
	}

};

template<typename T0>
FVector<T0> operator+(const FVector<T0>& Left, const FVector<T0>& Right)
{
	return FVector<T0>(Left.x + Right.x, Left.y + Right.y);
}

template<typename T0>
FVector<T0> operator-(const FVector<T0>& Left, const FVector<T0>& Right)
{
	return FVector<T0>(Left.x - Right.x, Left.y - Right.y);
}

template<typename T0>
FVector<T0> operator-(const FVector<T0>& Left)
{
	return FVector<T0>(-Left.x, -Left.y);
}




template<typename T0, typename T1>
FVector<T0> operator*(const FVector<T0>& Left, const T1 Right)
{
	return FVector<T0>(T0(Left.x * Right), T0(Left.y * Right));
}

template<typename T0, typename T1>
FVector<T0> operator*(const T1 Left, const FVector<T0>& Right)
{
	return FVector<T0>(T0(Left.x * Right), T0(Left.y * Right));
}

template<typename T0>
FVector<T0> operator*(const FVector<T0>& Left, const FVector<T0>& Right)
{
	return FVector<T0>(T0(Left.x * Right.x), T0(Left.y * Right.y));
}




template<typename T0, typename T1>
FVector<T0> operator/(const FVector<T0>& Left, const T1 Right)
{
	return FVector<T0>(T0(Left.x / Right), T0(Left.y / Right));
}

template<typename T0, typename T1>
FVector<T0> operator/(const T1 Left, const FVector<T0>& Right)
{
	return FVector<T0>(T0(Left.x / Right), T0(Left.y / Right));
}

template<typename T0>
FVector<T0> operator/(const FVector<T0>& Left, const FVector<T0>& Right)
{
	return FVector<T0>(T0(Left.x / Right.x), T0(Left.y / Right.y));
}


typedef FVector<int> FPoint;
typedef FVector<int> FSize;


typedef FVector<long double> FLongVector;





#endif