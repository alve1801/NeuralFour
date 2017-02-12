#pragma once

#ifndef DRAWHELPERS_H
#define DRAWHELPERS_H

#include "stdafx.h"
#include "Utils.h"
#include "AUserInterface.h"


#define DRAW_FILLED -1

enum class ELineType
{
	Line_1,
	Line_4,
	Line_Min
};

enum class ECircleType
{
	Raster,
	Vertex
};

enum class ETextDrawType
{
	Solid,
	Blended
};

void LogErrorSDL(ostream &os, const string &msg);

TTF_Font* LoadFont(string Path, const int Size);

void SetTextureTint(UI::FTexture* Texture, FColor Modulation);
void SetTextureAlpha(UI::FTexture* Texture, UChar Modulation);
void SetTextureTintAlpha(UI::FTexture* Texture, FColor Modulation);

void DrawTexture(SDL_Renderer* Renderer, UI::FTexture* Texture, FPoint Position, float Angle = 0.f, FPoint Size = FPoint());

void DrawTexture(SDL_Renderer* Renderer, SDL_Texture* Texture, FPoint Position, float Angle = 0.f, FPoint Size = FPoint());

void SetRenderColor(SDL_Renderer* Renderer, FColor NewColor);

FColor GetRenderColor(SDL_Renderer* Renderer);

void DrawLine(SDL_Renderer* Renderer, const FPoint P1, const FPoint P2, ELineType LineType = ELineType::Line_Min, const unsigned int Thickness = 1);

void DrawCircle(SDL_Renderer* Renderer, const FPoint Center, const unsigned int Radius, const ECircleType CircleType = ECircleType::Raster, const float VertextDensity = -1, const double Start = 0, const double End = 1);

void DrawFont(SDL_Renderer* Renderer, FPoint Position, wstring Text, TTF_Font* Font, FColor Color, const bool bMultiLine = false, ETextDrawType DrawType = ETextDrawType::Solid, FRectangle RenderRectangle = FRectangle());


void DrawRectangle(SDL_Renderer* Renderer, FRectangle Rectangle, const int Thickness = DRAW_FILLED, const float Offset = 0.0f);






#endif