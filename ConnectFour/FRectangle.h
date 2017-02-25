
#ifndef FRECTANGLE_H
#define FRECTANGLE_H

#pragma once
#include "FVector.h"
#include <SDL.h>
// ReSharper disable once CppUnusedIncludeDirective
#include "VersionProperties.h"

#if LIB_OPENCV
#include <opencv2/core/mat.hpp>
#endif

struct FRectangle
{
	FPoint TL;
	FPoint BR;

	FRectangle() : TL(), BR()
	{
	}

	explicit FRectangle(FPoint InTL) : TL(InTL), BR(InTL)
	{
	}

	FRectangle(int x, int y) : TL(x, y), BR(x, y)
	{
	}

	FRectangle(FPoint InTL, FPoint InBR) : TL(InTL), BR(InBR)
	{
	}

	FRectangle(FPoint InTL, FPoint Size, bool FromSize) : TL(InTL), BR(InTL + Size)
	{
	}

	FRectangle(FPoint InTL, int Width, int Height) : TL(InTL), BR(InTL.x + Width, InTL.y + Height)
	{
	}

	FRectangle(FPoint InTL, int WidthHeight) : TL(InTL), BR(InTL.x + WidthHeight, InTL.y + WidthHeight)
	{
	}

	FRectangle(int x, int y, int Width, int Height) : TL(x, y), BR(x + Width, y + Height)
	{
	}

	explicit FRectangle(SDL_Rect InRect) : TL(InRect.x, InRect.y), BR(InRect.x + InRect.w, InRect.y + InRect.h)
	{
	}


	void operator+=(FPoint P)
	{
		TL += P;
		BR += P;
	}

	bool operator==(FRectangle P) const
	{
		return TL == P.TL && BR == P.BR;
	}

	bool operator!=(FRectangle P) const
	{
		return TL != P.TL || BR != P.BR;
	}

	FPoint GetCenter() const
	{
		return FPoint((TL.x + BR.x) / 2, (TL.y + BR.y) / 2);
	}

	FPoint GetDimensions() const
	{
		return BR - TL;
	}

	FRectangle GetDilated(int Dilation = 1) const
	{
		return FRectangle(TL - FPoint(Dilation), BR + FPoint(Dilation));
	}

	FRectangle GetDilated(FPoint Dilation) const
	{
		return FRectangle(TL - Dilation, BR + Dilation);
	}

	FRectangle GetClipped(FRectangle ClippingRectangle) const
	{
		return FRectangle(FPoint(max(TL.x, ClippingRectangle.TL.x), max(TL.y, ClippingRectangle.TL.y)), FPoint(min(BR.x, ClippingRectangle.BR.x), min(BR.y, ClippingRectangle.BR.y)));
	}

	FRectangle GetClipped(FRectangle* ClippingRectangle) const
	{
		return FRectangle(FPoint(max(TL.x, ClippingRectangle->TL.x), max(TL.y, ClippingRectangle->TL.y)), FPoint(min(BR.x, ClippingRectangle->BR.x), min(BR.y, ClippingRectangle->BR.y)));
	}

	int GetHeight() const
	{
		return BR.y - TL.y;
	}

	int GetWidth() const
	{
		return BR.x - TL.x;
	}

	void SetDimensions(FPoint P)
	{
		BR = TL + P;
	}

	void SetDimensions(int NewWidth, int NewHeight)
	{
		SetDimensions(FPoint(NewWidth, NewHeight));
	}

	void SetWidth(int NewWidth)
	{
		BR.x = TL.x + NewWidth;
	}

	void SetHeight(int NewHeight)
	{
		BR.y = TL.y + NewHeight;
	}

	void SetAnchor(FPoint P)
	{
		BR += P - TL;
		TL = P;
	}

	void SetHorizontalAnchor(int x)
	{
		BR.x += x - TL.x;
		TL.x = x;
	}

	void SetVerticalAnchor(int y)
	{
		BR.y += y - TL.y;
		TL.y = y;
	}

	bool Contains(FPoint InPoint) const
	{
		return InPoint.x >= TL.x && InPoint.y >= TL.y && InPoint.x <= BR.x && InPoint.y <= BR.y;
	}

	bool SizeIsNull() const
	{
		return TL == BR;
	}

	SDL_Rect ToSDL() const
	{
		SDL_Rect NewRect;
		NewRect.x = TL.x;
		NewRect.y = TL.y;
		NewRect.w = GetWidth();
		NewRect.h = GetHeight();
		return NewRect;
	}

#if LIB_OPENCV
	cv::Rect ToCV() const
	{
		return cv::Rect(TL.ToCV(), BR.ToCV());
	}
#endif

};


#endif