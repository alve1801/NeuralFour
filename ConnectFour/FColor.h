
#ifndef FCOLOR_H
#define FCOLOR_H
#pragma once
#include <SDL.h>
#include "VersionProperties.h"
#if LIB_OPENCV
#include <opencv2/core/mat.hpp>
#endif

typedef unsigned char UChar;

struct FColor
{
	UChar R;
	UChar G;
	UChar B;
	UChar A;

	constexpr FColor() : R(), G(), B(), A()
	{
	}

	explicit constexpr FColor(UChar InRGB, UChar InA = 255) : R(InRGB), G(InRGB), B(InRGB), A(InA)
	{
	}

	explicit constexpr FColor(UChar InR, UChar InG, UChar InB, UChar InA = 255) : R(InR), G(InG), B(InB), A(InA)
	{
	}

	SDL_Color ToSDL() const
	{
		SDL_Color NewColor;
		NewColor.r = R;
		NewColor.g = G;
		NewColor.b = B;
		NewColor.a = A;
		return NewColor;
	}

#if LIB_OPENCV
	cv::Scalar ToScalar() const
	{
		return cv::Scalar(B, G, R, A);
	}
#endif

	FColor GetRGB(UChar NewAlpha = 0) const
	{
		return FColor(R, G, B, NewAlpha);
	}

	FColor operator*(FColor& C) const
	{
		return FColor(R * C.R, G * C.G, B * C.B, A * C.A);
	}
};

#endif