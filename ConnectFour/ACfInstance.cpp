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

		if (Rounds >= 200)
		{
			ASmartWriteLock GenLock(Mutex);

			bNextGeneration = true;
			while (true)
			{
				GenLock.Unlock();
				THREAD_SLEEP_MS(100);
				GenLock.Lock();

				if (!bNextGeneration)
				{
					Rounds = 0;

					break;
				}
			}
		}

		//if (Rounds > 0 && Rounds % 100 == 0)
		//{
		//	double DeltaFitness = Utils::Difference(Players[0]->NeuralNetworkInstance.Fitness, Players[1]->NeuralNetworkInstance.Fitness);

		//	if (DeltaFitness > 3)
		//	{
		//		PRINT "Opression"  TAB DeltaFitness END;
		//		if (Players[0]->NeuralNetworkInstance.Fitness > Players[1]->NeuralNetworkInstance.Fitness)
		//		{
		//			Players[0]->NeuralNetworkInstance.OpressiveMerge(&Players[1]->NeuralNetworkInstance);
		//			Players[1]->NeuralNetworkInstance.Mutate();

		//		}
		//		else
		//		{
		//			Players[1]->NeuralNetworkInstance.OpressiveMerge(&Players[0]->NeuralNetworkInstance);
		//			Players[0]->NeuralNetworkInstance.Mutate();


		//		}
		//		//throw runtime_error("asd");

		//	}
		//	else
		//	{
		//		PRINT "Mutate" END;
		//		Players[0]->NeuralNetworkInstance.Mutate();
		//		Players[1]->NeuralNetworkInstance.Mutate();
		//	}
		//	
		//}

		Rounds++;
		TotalRounds++;
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
				for (int Index = 0; Index < 5; ++Index)
				{
					if (FigureMatrix.HasSpace(Index))
					{
						break;
					}
					if (Index == 4)
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
	for (int x = 0; x < 5; ++x)
	{
		for (int y = 0; y < 5; ++y)
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



