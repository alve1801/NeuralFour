#pragma once

#ifndef AALKANEUSERINTERFACE_H
#define AALKANEUSERINTERFACE_H

#include "Utils.h"
#include "boost/function.hpp"
#include "AUserInterface.h"
#include "ACfInstance.h"

class AAlkane;
struct FRenderData;



class ACfBoard;
class ACfGraph;



class AMainUserInterface : public UI::AUserInterface
{
public:
	AMainUserInterface(UI::AWindow* InParentWindow);
	~AMainUserInterface();


	void Init() override;

	void Tick() override;
	void Render(SDL_Renderer* Renderer) override;

	void UpdateAreas() override;

	vector<ACfBoard*> Boards;
	ACfGraph* Graph;

	ACfBoard* AddBoard(FRectangle BoardArea, ACfInstance* Instance);
};


class AMainWindow : public UI::AWindow
{
public:
	AMainWindow();
	virtual ~AMainWindow();

	AMainUserInterface MainUserInterface;


	void Render() override;
	void Tick() override;

	virtual void Init(string Name, FRectangle InArea);
	void Destroy() override;
};



class ACfBoard : public UI::AComponent
{
public:
	ACfBoard(UI::AUserInterface* InParentUserInterface, FRectangle InArea);
	virtual ~ACfBoard();



	void Render(SDL_Renderer* Renderer) override;
	void Tick() override;

	FVector<float> CellSize;

	ACfInstance* CfInstance;
	//list<>
};



class ACfGraph : public UI::AComponent
{
public:
	ACfGraph(UI::AUserInterface* InParentUserInterface, FRectangle InArea);
	virtual ~ACfGraph();



	void Render(SDL_Renderer* Renderer) override;
	void Tick() override;


	vector<pair<vector<double>, FColor>> TotalEntries;
	//vector<ULong> ReusedEntries;

	double HighestValue = 0;
	double LowestValue = 0;
	double Scale = 0;

	void AddValue(size_t Index, double Value);
	void AddGraph();
};

#endif
