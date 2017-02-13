#include "stdafx.h"
#include "ACfInstance.h"
#include "Globals.h"
#include "AApp.h"
#include "SqlHelpers.h"

vector<ACfInstance*> ACfInstance::AllInstances;

FVector<SChar> Orientations[8] = { 
	FVector<SChar>(0, 1), 
	FVector<SChar>(1, 1), 
	FVector<SChar>(1, 0), 
	FVector<SChar>(1, -1), 
	FVector<SChar>(0, -1),
	FVector<SChar>(-1, -1),
	FVector<SChar>(-1, 0), 
	FVector<SChar>(-1, 1)
};


ACfInstance::ACfInstance()
{
	AllInstances.push_back(this);
}

ACfInstance::~ACfInstance()
{
}

void ACfInstance::Init()
{
	Players[0] = new ACfPlayer(this, 0);
	Players[1] = new ACfPlayer(this, 1);

	AllNeuralNetworkInstances.push_back(Players[0]->NeuralNetworkInstance);
	AllNeuralNetworkInstances.push_back(Players[1]->NeuralNetworkInstance);

	Thread = new boost::thread(boost::bind(&ACfInstance::Execute, this));
}

void ACfInstance::Execute()
{
	Globals::ThreadInfos.reset(new AThreadInfo());

	
	while (true)
	{
		if (!Globals::bLearningPhase || Globals::App->bPaused)
		{
			THREAD_SLEEP_MS(100);
			continue;
		}
		
		StartRound();

		Rounds++;
		TotalRounds++;

		if (Rounds >= 2)
		{

			//bNextGeneration = true;


			//while (true)
			//{
			//	//GenLock.Unlock();
			//	THREAD_SLEEP_MS(100);
			//	//GenLock.Lock();

			//	if (!bNextGeneration)
			//	{
			//		ASmartWriteLock GenLock(Mutex);

			//		Rounds = 0;

			//		break;
			//	}
			//}
			NeuralNetwork::AInstance* Opponent = Players[0]->NeuralNetworkInstance;
			NeuralNetwork::AInstance* Gladiator = Players[1]->NeuralNetworkInstance;
			OpponentIndex++;
			if (Gladiator->Fitness < Opponent->Fitness)
			{
				Opponent->Wins++;
				Opponent->Fitness = 0;

				Gladiator->GenerateOffspring(AllNeuralNetworkInstances[AllNeuralNetworkInstances.size() - 2]);
				Gladiator->Fitness = 0;
				Offsprings++;
				PRINT "Bad Offspring" TAB Generations TAB Offsprings TAB OpponentIndex TAB Opponent->Fitness END;

				OpponentIndex = 0;


			}
			else if (OpponentIndex == AllNeuralNetworkInstances.size() -1)
			{
				Opponent->Fitness = 0;

				Gladiator->Wins++;

				Players[1]->NeuralNetworkInstance = new NeuralNetwork::AInstance();
				Players[1]->NeuralNetworkInstance->AddLayer(MATRIX_WIDTH * MATRIX_HEIGHT * 3);
				Players[1]->NeuralNetworkInstance->AddLayer(size_t(MATRIX_WIDTH * MATRIX_HEIGHT * 3 * 2));
				Players[1]->NeuralNetworkInstance->AddLayer(MATRIX_WIDTH);

				Players[1]->NeuralNetworkInstance->GenerateOffspring(Gladiator);
				AllNeuralNetworkInstances.push_back(Players[1]->NeuralNetworkInstance);

				Generations++;
				Offsprings++;

				PRINT "Next Gen" TAB Generations END;

				OpponentIndex = 0;

			}
			else
			{
				Opponent->Fitness = 0;
				Players[0]->NeuralNetworkInstance = AllNeuralNetworkInstances[OpponentIndex];
				Players[0]->NeuralNetworkInstance->Fitness = 0;
			}
			

			Rounds = 0;


		}

		
	}
}

void ACfInstance::StartRound()
{
	ASmartWriteLock Lock(Mutex);

	FigureMatrix.Reset();
	FigureMatrixCode = 0;
	bool bEnd = false;

	if (PlayerOffset == 0)
	{
		PlayerOffset = 1;
	}
	else
	{
		PlayerOffset = 0;
	}

	while (!bEnd)
	{
		for (int Index = 0; Index < 2; ++Index)
		{

			int PlayerIndex = (Index + PlayerOffset) % 2;


			bool bResult = Players[PlayerIndex]->NextMove();


			if (!bResult)
			{
				Players[PlayerIndex]->Failure();
				bEnd = true;
			}

			else if (CheckForSuccess(Players[PlayerIndex]))
			{
				Players[PlayerIndex]->Success();
				Players[(PlayerIndex+1)%2]->Failure();


				bEnd = true;
				break;
			}
			else 
			{
				for (int RowIndex = 0; RowIndex < MATRIX_WIDTH; ++RowIndex)
				{
					if (FigureMatrix.HasSpace(RowIndex))
					{
						break;
					}
					if (RowIndex == 4)
					{
						bEnd = true;
						break;
					}
				}
			}
			

			if (bEnd)
			{
				break;
			}
			
		}
		if (bEnd)
		{
			break;
		}
	}
	Players[0]->Reset();
	Players[1]->Reset();

}

void ACfInstance::UpdateFigureMatrix()
{
	ASmartWriteLock Lock(Mutex);

	FigureMatrixCode = 0;
	for (int Index = 0; Index < 24; ++Index)
	{
		FigureMatrixCode += ULong(round(std::pow(3, Index) * (FigureMatrix[Index] != 0 ? (FigureMatrix[Index]) : 0)));
	}
}

bool ACfInstance::CheckForSuccess(ACfPlayer* Player)
{
	for (int x = 0; x < MATRIX_WIDTH; ++x)
	{
		for (int y = 0; y < MATRIX_HEIGHT; ++y)
		{
			UChar Id = FigureMatrix[FVector<UChar>(x, y)];
			if (Id)
			{
				for (UChar OrientationIndex = 0; OrientationIndex < 8; ++OrientationIndex)
				{
					FVector<SChar> CheckPosition = FVector<SChar>(x, y);

					bool bWin = true;
					for (int OffsetIndex = 1; OffsetIndex < 4; ++OffsetIndex)
					{
						CheckPosition += Orientations[OrientationIndex];
						if (!FRectangle(0,0,4,4).Contains(CheckPosition.Cast<int>()) || FigureMatrix[CheckPosition] != Id)
						{
							bWin = false;
							break;
						}
					}
					if (bWin)
					{
						return true;
					}
				}
			}
		}
	}



	return false;
}



