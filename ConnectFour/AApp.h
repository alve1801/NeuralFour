#pragma once

#ifndef AAPP_H
#define AAPP_H


#include "AUserInterface.h"
#include <stack>
#include "VersionProperties.h"
#include "AThreadInfo.h"
#include "FKeyData.h"
#include "ACfUserInterface.h"
#include "ACfInstance.h"


class ABinding;
class Alkanes;

using namespace std;



struct FMouseData
{
	bool bDrag = true;
	FPoint LastPosition, Position, CurrentDrag, HoveredCell;



	FFlag<UShort> Events;

	FMouseData::FMouseData()
	{
		CurrentDrag = Position = LastPosition = HoveredCell = FPoint();
	}


	void UpdatePosition(FPoint NewPosition)
	{
		LastPosition = Position;
		Position = NewPosition;
		if (bDrag)
		{
			CurrentDrag += Position - LastPosition;
		}
	}
	void Reset()
	{
		CurrentDrag = FPoint();
		if (Events.Has(UI::Mouse_LClick))
		{
			Events.Remove(UI::Mouse_LClick);
			Events.Add(UI::Mouse_LHold);
		}

		if (Events.Has(UI::Mouse_RClick))
		{
			Events.Remove(UI::Mouse_RClick);
			Events.Add(UI::Mouse_RHold);
		}


		Events.Remove(UI::Mouse_Move | UI::Mouse_WDown | UI::Mouse_WUp | UI::Mouse_Move | UI::Mouse_LUp | UI::Mouse_RUp);
	}
};



struct FRenderData
{

	UInt ToolSymbolSize = 0;
	UInt FunctionalGroupSymbolSize = 0;

	//Colors
	struct FTheme
	{
		string Name;

		FColor BackGround;

		FColor UserInterfaceButtons;
		FColor UserInterfaceButtonHighlighting;
		FColor UserInterfaceDropDown;
		
		FColor MerelsLines;

	} Theme;


	struct FFonts
	{
		TTF_Font* Large;
		TTF_Font* Medium;
		TTF_Font* Small;
	} Fonts;

	float BindingLineLengthRatio = 0.f;


	

	FRenderData::FRenderData()
	{

	}

};

class AApp
{
public:
	AApp();
	~AApp();

private:
	bool bRunning;

public:
	AMainWindow MainWindow;

	int Execute();

	static bool InitSDL();

	bool Init();

	bool InitMainWindow();

	void EventHandler(SDL_Event* Event);

	void Tick();

	void Render();

private:
	void Destroy();

	
public:

	void Exit();

	bool bPaused = true;
	bool bDrawBoards = true;


	FMouseData Mouse;
	FKeyData Keys;

private:

	void MouseEventHandler(SDL_Event* Event);

public:
	const size_t SoftwareVersion = MASTER_VERSION_KEY;

	FRenderData RenderData;


	vector<ACfInstance*> Instances;
	vector<ACfPlayer*> Players;
	UInt Generations = 1;


	UShort RestartIterations;
	UShort MerelsInstanceCount;

	void EndLearingPhase();
	//void MergeArchives(bool* bDonePtr, double* ProgressPtr, double* TotalProgressPtr);

};


#endif
