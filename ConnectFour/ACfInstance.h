// Merels 
#pragma once
#include "ACfPlayer.h"
#include "ACfMatrix.h"
#include "ASmartMutex.h"
#include "ANeuralNetwork.h"


class ACfInstance
{
public:

	ACfInstance();
	~ACfInstance();

	void Init();

	void Execute();

	void StartRound();

	static vector<ACfInstance*> AllInstances;


	boost::thread* Thread;

	ASmartMutex Mutex;

	bool bNextGeneration = false;


	ACfPlayer* Players[2];

	ULong Rounds = 0;
	ULong TotalRounds = 0;

	ACfMatrix FigureMatrix;
	ULong FigureMatrixCode;

	void UpdateFigureMatrix();

	bool CheckForSuccess(ACfPlayer* Player);

private:
	int PlayerOffset = 0;

};
