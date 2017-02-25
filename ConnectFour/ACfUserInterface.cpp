#include "stdafx.h"
#include "ACfUserInterface.h"
#include "AApp.h"
#include "VersionProperties.h"

#if LIB_OPENCV
#include "opencv2/opencv.hpp"
#endif

#include "SysHelpers.h"
#include "DrawHelpers.h"
#include "Globals.h"



#define IMAGE_OPTIONS_ICONAREA FRectangle(0, 0, 12, 12)
#define IMAGE_OPTIONS_TEXTOFFSET FPoint(20, -6)
#define IMAGE_OPTIONS_ICONS UI::FCheckboxIcons(FColor(231, 32), FColor(231, 64), FColor(231, 255), 1)


#define RENDERDATA Globals::App->RenderData
#define FONTS RENDERDATA.Fonts
#define THEME RENDERDATA.CurrentTheme

#define LOCALIZATION Globals::CurrentLocalization.LanguageObjects


#define DEFAULT_BUTTON_ICONS UI::FButtonIcons(true, FColor(128, 32), FColor(128, 64))
#define DEFAULT_CHECKBOX_ICONS UI::FCheckboxIcons(FColor(231, 32), FColor(231, 64), FColor(231, 255), 1)
#define DEFAULT_SYMBOL_BUTTON_ICONS(Path) UI::FButtonIcons(ParentWindow, Path, false, true, true)

#define ADD_DEFAULT_CALLBACK(Callback) AddEvent(UI::Button_LDown, false)->AddEventCallback(UI::Button_LUp, UI::FCallback(Callback, true))

#define TEXTURE_DIR "Textures\\"

const FVector<UChar> MerelsBoardGrid[24] =
{
	FVector<UChar>(3, 0),
	FVector<UChar>(6, 0),
	FVector<UChar>(6, 3),
	FVector<UChar>(6, 6),
	FVector<UChar>(3, 6),
	FVector<UChar>(0, 6),
	FVector<UChar>(0, 3),
	FVector<UChar>(0, 0),
	FVector<UChar>(3, 1),
	FVector<UChar>(5, 1),
	FVector<UChar>(5, 3),
	FVector<UChar>(5, 5),
	FVector<UChar>(3, 5),
	FVector<UChar>(1, 5),
	FVector<UChar>(1, 3),
	FVector<UChar>(1, 1),
	FVector<UChar>(3, 2),
	FVector<UChar>(4, 2),
	FVector<UChar>(4, 3),
	FVector<UChar>(4, 4),
	FVector<UChar>(3, 4),
	FVector<UChar>(2, 4),
	FVector<UChar>(2, 3),
	FVector<UChar>(2, 2)
};




AMainWindow::AMainWindow()
	: AWindow()	, MainUserInterface(this)
{
	UserInterface = &MainUserInterface;
}

AMainWindow::~AMainWindow()
{
}

void AMainWindow::Render()
{
	SetRenderColor(Renderer, Globals::App->RenderData.Theme.BackGround);
	SDL_RenderClear(Renderer);
	AWindow::Render();
}

void AMainWindow::Tick()
{
	AWindow::Tick();
}

void AMainWindow::Init(string Name, FRectangle InArea)
{


	UserInterface = &MainUserInterface;
	UInt InitRendererFlags = SDL_RENDERER_ACCELERATED;
	UInt InitWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	AWindow::Init(Name, InArea, &Globals::App->Mouse.Events, &Globals::App->Mouse.Position, &InitWindowFlags, &InitRendererFlags);
	bTickAlways = true;
}

void AMainWindow::Destroy()
{
	AWindow::Destroy();
}



AMainUserInterface::AMainUserInterface(UI::AWindow* InParentWindow) : AUserInterface(InParentWindow)
{
	
}

AMainUserInterface::~AMainUserInterface()
{
}

void AMainUserInterface::Init()
{

	UpdateAreas();


	Graph = new ACfGraph(this, FRectangle(32, 120 + 32 + 256, 1024, 256));
	AddComponent(Graph);
}

void AMainUserInterface::Tick()
{
		

	AUserInterface::Tick();
}

void AMainUserInterface::Render(SDL_Renderer* Renderer)
{
	AUserInterface::Render(Renderer);

}

void AMainUserInterface::UpdateAreas()
{
	AUserInterface::UpdateAreas();
}

ACfBoard* AMainUserInterface::AddBoard(FRectangle BoardArea, ACfInstance* Instance)
{
	ACfBoard* Board = new ACfBoard(this, BoardArea);
	AddComponent(Board);
	Boards.push_back(Board);
	Board->CfInstance = Instance;
	return Board;
}


ACfBoard::ACfBoard(UI::AUserInterface* InParentUserInterface, FRectangle InArea) : AComponent(InParentUserInterface, InArea)
{
	CellSize = (Area.GetDimensions().Cast<double>() / FVector<double>(MATRIX_WIDTH, MATRIX_HEIGHT)).Cast<float>();
}

ACfBoard::~ACfBoard()
{
}

void ACfBoard::Render(SDL_Renderer* Renderer)
{

	
	SetRenderColor(Renderer, Globals::App->RenderData.Theme.MerelsLines);

	DrawRectangle(Renderer, Area, 1);


	for (UChar Index = 1; Index < MATRIX_WIDTH; ++Index)
	{
		DrawLine(Renderer, Area.TL + FPoint(CellSize.x * Index, 0), FPoint(Area.TL.x + CellSize.x * Index, Area.BR.y - 1));
		//DrawLine(Renderer, Area.TL + FPoint(0, CellSize.x * Index), FPoint(Area.BR.x - 1, Area.TL.y + CellSize.x * Index));
	}
	for (UChar Index = 1; Index < MATRIX_HEIGHT; ++Index)
	{
		//DrawLine(Renderer, Area.TL + FPoint(CellSize.x * Index, 0), FPoint(Area.TL.x + CellSize.x * Index, Area.BR.y - 1));
		DrawLine(Renderer, Area.TL + FPoint(0, CellSize.y * Index), FPoint(Area.BR.x - 1, Area.TL.y + CellSize.y * Index));
	}
	if (Globals::App->bDrawBoards && CfInstance)
	{

		ASmartReadLock Lock(CfInstance->Mutex);
		for (int x = 0; x < MATRIX_WIDTH; ++x)
		{
			for (int y = 0; y < MATRIX_HEIGHT; ++y)
			{
				UChar PlayerId = CfInstance->FigureMatrix[FVector<UChar>(x, y)];
				CONTINUE_IF(PlayerId == 0);
				SetRenderColor(Renderer, (PlayerId == 1 ? FColor(0) : FColor(255)));

				FPoint BL = FPoint(Area.TL.x + x*CellSize.x, Area.BR.y - y * CellSize.y - CellSize.y);
				DrawRectangle(Renderer, FRectangle(BL + (CellSize*0.2).Cast<int>(), BL + (CellSize*0.8).Cast<int>()));
			}
		}

		if (Globals::App->bWaitingForPlayer)
		{
			SetRenderColor(Renderer, FColor(255));

			FPoint BL = FPoint(Area.TL.x + Globals::App->PlayerInput*CellSize.x, Area.BR.y - MATRIX_HEIGHT * CellSize.y - CellSize.y);
			DrawRectangle(Renderer, FRectangle(BL + (CellSize*0.2).Cast<int>(), BL + (CellSize*0.8).Cast<int>()));
		}
	}

}

void ACfBoard::Tick()
{
}




ACfGraph::ACfGraph(UI::AUserInterface* InParentUserInterface, FRectangle InArea) : AComponent(InParentUserInterface, InArea)
{
	/*TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 0, 0)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(0, 255, 0)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(0, 0, 255)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 0, 255)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 255, 0)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(0, 255, 255)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 128, 0)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 0, 128)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(255, 128, 128)));
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(128, 255, 128)));*/
}

ACfGraph::~ACfGraph()
{
}

void ACfGraph::Render(SDL_Renderer* Renderer)
{
	SetRenderColor(Renderer, FColor(120));
	DrawRectangle(Renderer, Area, 1);

	SetRenderColor(Renderer, FColor(120, 128));
	DrawLine(Renderer, Area.TL + FPoint(0, Area.GetHeight()/2), FPoint(Area.BR.x, Area.TL.y + Area.GetHeight() / 2));

	if (TotalEntries.empty())
	{
		return;
	}
	int Height = Area.GetHeight();
	int PixelStepSize = 10;
	int EntryCount = Area.GetWidth() / PixelStepSize;

	for (int Index = 0; Index < TotalEntries.size(); ++Index)
	{
	/*	if (TotalEntries[Index].first.size() < EntryCount)
		{
			continue;
		}*/

	
		SetRenderColor(Renderer, TotalEntries[Index].second);

		int LastPixel = 0;
		int Pixel = Area.TL.x;



		for (size_t ValueIndex = size_t(max(int(TotalEntries[Index].first.size())- EntryCount, 0)); ValueIndex < TotalEntries[Index].first.size(); ValueIndex += 1)
		{
			int Current = int(Area.BR.y - Height*Utils::Percentage(TotalEntries[Index].first[size_t(ValueIndex)], -Scale, Scale));

			if (Pixel > Area.TL.x)
			{
				SDL_RenderDrawLine(Renderer, Pixel - PixelStepSize, LastPixel, Pixel, Current);
			}



			LastPixel = Current;

			Pixel += PixelStepSize;
		}
	}

	/*HighestValue = std::max(TotalEntries.back(), HighestValue);


	double StepSize = std::max(double(1), double(TotalEntries.size()) / Area.GetWidth());
	double Ratio = double(Area.GetHeight()) / (HighestValue - TotalEntries.front());
	int Pixel = Area.TL.x;
	SetRenderColor(Renderer, FColor(255, 0, 0));


	for (double Index = 0; size_t(Index) < TotalEntries.size(); Index += StepSize)
	{
		SDL_RenderDrawPoint(Renderer, Pixel, int(Area.BR.y - (TotalEntries[size_t(Index)] - TotalEntries.front()) * Ratio));
		Pixel++;
	}*/


}

void ACfGraph::Tick()
{
}

void ACfGraph::AddValue(size_t Index, double Value)
{
	LowestValue = min(LowestValue, Value);
	HighestValue = max(HighestValue, Value);
	Scale = max(abs(LowestValue), abs(HighestValue));

	TotalEntries[Index].first.push_back(Value);

}

void ACfGraph::AddGraph()
{
	TotalEntries.push_back(pair<vector<double>, FColor>(0, FColor(UChar(Utils::RandomScalar<double>() * 255), UChar(Utils::RandomScalar<double>() * 255), UChar(Utils::RandomScalar<double>() * 255))));

}
