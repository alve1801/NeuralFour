#include "stdafx.h"
#include "DrawHelpers.h"
#include "VersionProperties.h"

using namespace std;


void LogErrorSDL(ostream &os, const string &msg)
{
	cout << " error: " << SDL_GetError() << endl;
	//OutputDebugStringA("My output string.");
}


TTF_Font* LoadFont(string Path, const int Size)
{

	return TTF_OpenFont(Path.c_str(), Size);
}



void SetTextureTint(UI::FTexture* Texture, FColor Modulation)
{
	SDL_SetTextureColorMod(Texture->Texture, Modulation.R, Modulation.G, Modulation.B);
}

void SetTextureAlpha(UI::FTexture* Texture, UChar Alpha)
{
	SDL_SetTextureAlphaMod(Texture->Texture, Alpha);
}

void SetTextureTintAlpha(UI::FTexture* Texture, FColor Modulation)
{
	SDL_SetTextureColorMod(Texture->Texture, Modulation.R, Modulation.G, Modulation.B);
	SDL_SetTextureAlphaMod(Texture->Texture, Modulation.A);
}



void DrawTexture(SDL_Renderer* Renderer, UI::FTexture* Texture, FPoint Position, float Angle, FPoint Size)
{
#if DRAW_SAFE
	RETURN_IF_NULLPTR(Texture);
	RETURN_IF_NULLPTR(Texture->Texture);
#endif
	DrawTexture(Renderer, Texture->Texture, Position, Angle, Size);
}

void DrawTexture(SDL_Renderer* Renderer, SDL_Texture* Texture, FPoint Position, float Angle, FPoint Size)
{
#if DRAW_SAFE
	RETURN_IF_NULLPTR(Texture);
#endif
	if (Size == FPoint())
	{
		SDL_QueryTexture(Texture, nullptr, nullptr, &Size.x, &Size.y);
	}

	FRectangle RenderRect = FRectangle(Position, Position + Size);

	SDL_Rect SDLRenderRect = RenderRect.ToSDL();
	SDL_RenderCopyEx(Renderer, Texture, nullptr, &SDLRenderRect, Angle, nullptr, SDL_FLIP_NONE);
}


void SetRenderColor(SDL_Renderer* Renderer, FColor Color)
{
	SDL_SetRenderDrawColor(Renderer, Color.R, Color.G, Color.B, Color.A);
}

FColor GetRenderColor(SDL_Renderer * Renderer)
{
	Uint8 R, G, B, A;
	SDL_GetRenderDrawColor(Renderer, &R, &G, &B, &A);
	return FColor(int(R), int(G), int(B), int(A));
}





void BresenhamsLine4(SDL_Renderer* Renderer, const FPoint P1, const FPoint P2)
{
	int dx = abs(P2.x - P1.x);
	int	dy = abs(P2.y - P1.y);


	int ix = -1;
	int iy = -1;

	if (P1.x < P2.x)
	{
		ix = 1;
	}
	if (P1.y < P2.y)
	{
		iy = 1;

	}
	int e = 0;

	FPoint Buffer = P1;

	for (int i = 0; i < dx + dy; i++)
	{
		//draw_pixel(P1.x, P1.y, color)
		SDL_RenderDrawPoint(Renderer, Buffer.x, Buffer.y);
		int e1 = e + dy;
		int e2 = e - dx;
		if (abs(e1) < abs(e2))
		{
			Buffer.x += ix;
			e = e1;
		}
		else
		{
			Buffer.y += iy;
			e = e2;
		}

	}
}

void BresenhamsCircle(SDL_Renderer* Renderer, const FPoint Center, const int Radius)
{
	int f = 1 - Radius;
	int ddF_x = 0;
	int ddF_y = -2 * Radius;

	FPoint CurrentPoint(0, Radius);

	SDL_RenderDrawPoint(Renderer, Center.x, Center.y + Radius);
	SDL_RenderDrawPoint(Renderer, Center.x, Center.y - Radius);
	SDL_RenderDrawPoint(Renderer, Center.x + Radius, Center.y);
	SDL_RenderDrawPoint(Renderer, Center.x - Radius, Center.y);

	while (CurrentPoint.x < CurrentPoint.y)
	{
		if (f >= 0)
		{
			CurrentPoint.y--;
			ddF_y += 2;
			f += ddF_y;
		}
		CurrentPoint.x++;
		ddF_x += 2;
		f += ddF_x + 1;

		SDL_RenderDrawPoint(Renderer, Center.x + CurrentPoint.x, Center.y + CurrentPoint.y);
		SDL_RenderDrawPoint(Renderer, Center.x - CurrentPoint.x, Center.y + CurrentPoint.y);
		SDL_RenderDrawPoint(Renderer, Center.x + CurrentPoint.x, Center.y - CurrentPoint.y);
		SDL_RenderDrawPoint(Renderer, Center.x - CurrentPoint.x, Center.y - CurrentPoint.y);
		SDL_RenderDrawPoint(Renderer, Center.x + CurrentPoint.y, Center.y + CurrentPoint.x);
		SDL_RenderDrawPoint(Renderer, Center.x - CurrentPoint.y, Center.y + CurrentPoint.x);
		SDL_RenderDrawPoint(Renderer, Center.x + CurrentPoint.y, Center.y - CurrentPoint.x);
		SDL_RenderDrawPoint(Renderer, Center.x - CurrentPoint.y, Center.y - CurrentPoint.x);
	}
}

void VertexCircle(SDL_Renderer* Renderer, const FPoint Center, const unsigned int Radius, double VertextDensity, double Start, double End)
{
	if (VertextDensity < 0)
	{
		VertextDensity = max(pow(Radius / 10, 0.5) * 10, 8.0);
	}


	VertextDensity = 1 / VertextDensity;

	Start *= 2 * Constants::Pi;
	End *= 2 * Constants::Pi;
	VertextDensity *= 2 * Constants::Pi;

	if (Start > End)
	{
		swap(Start, End);
	}


	double CurrentRatio = Start;
	bool First = true;
	FPoint LastPoint;
	while (true)
	{
		FPoint CurrentPoint(int(round(cos(CurrentRatio) * Radius + Center.x)), int(round(sin(CurrentRatio) * Radius + Center.y)));
		if (!First)
		{
			SDL_RenderDrawLine(Renderer, CurrentPoint.x, CurrentPoint.y, LastPoint.x, LastPoint.y);
		}
		else
		{
			First = false;
		}
		if (CurrentRatio >= End)
		{
			break;
		}
		LastPoint = CurrentPoint;
		CurrentRatio = min(End*1.01, CurrentRatio + VertextDensity);

	}
}




void DrawLine(SDL_Renderer* Renderer, const FPoint P1, const FPoint P2, ELineType LineType, const unsigned int Thickness)
{


	if (Thickness > 1 && LineType != ELineType::Line_1)
	{
		FVector<float> Normal = FVector<float>(float(P2.x - P1.x), float(P2.y - P1.y)).GetNormalized().GetOrthogonal();
		FPoint Offset;
		for (int Index = (int)-round(Thickness * 0.5f); Index < (int)round(Thickness * 0.5f); Index++)
		{
			Offset.x = (int)round(Normal.x * Index);
			Offset.y = (int)round(Normal.y * Index);
			BresenhamsLine4(Renderer, Offset + P1, Offset + P2);
		}
	}
	else
	{
		switch (LineType)
		{
		case ELineType::Line_1:
		case ELineType::Line_Min:
			SDL_RenderDrawLine(Renderer, P1.x, P1.y, P2.x, P2.y);
			break;
		case ELineType::Line_4:
			BresenhamsLine4(Renderer, P1, P2);
			break;
		default:
			SDL_RenderDrawLine(Renderer, P1.x, P1.y, P2.x, P2.y);
			break;
		}
	}

}

void DrawCircle(SDL_Renderer* Renderer, const FPoint Center, const unsigned int Radius, const ECircleType CircleType, const float VertextDensity, const double Start, const double End)
{
	switch (CircleType)
	{
	case ECircleType::Raster:
		BresenhamsCircle(Renderer, Center, Radius);

		break;

	case ECircleType::Vertex:
		VertexCircle(Renderer, Center, Radius, VertextDensity, Start, End);
		break;

	default:
		break;
	}
}

void DrawFont(SDL_Renderer* Renderer, FPoint Position, wstring Text, TTF_Font* Font, FColor Color, const bool bMultiLine, ETextDrawType DrawType, FRectangle RenderRectangle)
{
	if (!Font)
	{
		return;
	}
	size_t Start = 0, End = 0;
	UShort Line = 0;

	size_t Length = Text.length();

	UShort FontHeight = TTF_FontHeight(Font);

	bool bAutoSize = false;

	if (RenderRectangle == FRectangle())
	{
		bAutoSize = true;
	}

	while (End < Length)
	{
		FPoint NewPosition = Position;
		wstring Part;
		if (bMultiLine)
		{
			Line++;

			NewPosition += FPoint(0, FontHeight * (Line - 1));

			while (End < Length && Text[End] != '\n')
			{
				End++;
			}

			Part = Text.substr(Start, End - Start);
			End++;
			Start = End;
		}
		else
		{
			Part = Text;
		}

		SDL_Surface* SurfaceMessage = nullptr;

		switch (DrawType)
		{
		case ETextDrawType::Solid:
			SurfaceMessage = TTF_RenderUNICODE_Solid(Font, (UShort*)Part.data(), Color.ToSDL());
			break;
		case ETextDrawType::Blended:
			SurfaceMessage = TTF_RenderUNICODE_Blended(Font, (UShort*)Part.data(), Color.ToSDL());

			break;
		default:
			break;
		}
		if (!SurfaceMessage)
		{
			return;
		}

		SDL_Texture* Message = SDL_CreateTextureFromSurface(Renderer, SurfaceMessage);
		SDL_FreeSurface(SurfaceMessage);
		if (bAutoSize)
		{
			FPoint RenderRectangleSize;
			TTF_SizeUNICODE(Font, (UShort*)Part.data(), &RenderRectangleSize.x, &RenderRectangleSize.y);
			RenderRectangle = FRectangle(NewPosition, RenderRectangleSize.x, RenderRectangleSize.y);
		}
		if (Color.A != 255)
		{
			SDL_SetTextureAlphaMod(Message, Color.A);
		}

		SDL_Rect SDLRenderRect = RenderRectangle.ToSDL();
		SDL_RenderCopy(Renderer, Message, nullptr, &SDLRenderRect);
		SDL_DestroyTexture(Message);

		if (!bMultiLine)
		{
			break;
		}
	}
}

void DrawRectangle(SDL_Renderer* Renderer, FRectangle Rectangle, const int Thickness, const float Offset)
{
	if (Thickness == 1)
	{
		SDL_Rect SDLRenderRect = Rectangle.ToSDL();
		SDL_RenderDrawRect(Renderer, &SDLRenderRect);
	}
	else if (Thickness > 1)
	{
		FRectangle Buffer;
		for (int Index = (int)-ceil(Thickness * (1 - (Offset * 0.5f + 0.5f))); Index <= (int)floor(Thickness * (Offset * 0.5f + 0.5f))-1; Index++)
		{
			Buffer.TL = Rectangle.TL - FPoint(Index);
			Buffer.BR = Rectangle.BR + FPoint(Index);

			SDL_Rect SDLRenderRect = Buffer.ToSDL();
			SDL_RenderDrawRect(Renderer, &SDLRenderRect);

		}
	}
	else
	{
		SDL_Rect SDLRenderRect = Rectangle.ToSDL();
		SDL_RenderFillRect(Renderer, &SDLRenderRect);
	}
}




