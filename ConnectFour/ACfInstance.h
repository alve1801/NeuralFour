﻿// Merels 
#pragma once
#include "ACfPlayer.h"
#include "ACfMatrix.h"
#include "ASmartMutex.h"
#include "ANeuralNetwork.h"
#include <atomic>


class ACfInstance
{
public:

	ACfInstance();
	~ACfInstance();

	void Init();

	void Execute();

	void StartRound();

	static vector<ACfInstance*> AllInstances;

	vector<NeuralNetwork::AInstance*> AllNeuralNetworkInstances;
	size_t OpponentIndex;
	//NeuralNetwork::AInstance* Gl

	size_t Generations = 1;
	size_t Offsprings = 0;


	boost::thread* Thread;

	ASmartMutex Mutex;

	atomic<bool> bNextGeneration = false;


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
