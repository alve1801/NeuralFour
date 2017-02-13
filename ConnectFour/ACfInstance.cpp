#include "stdafx.h"
#include "ACfInstance.h"
#include "Globals.h"
#include "AApp.h"
#include "SqlHelpers.h"
#include "SysHelpers.h"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

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

	Log.open(SysHelpers::GetAppUserStorageLocation() + L"\\Log.csv", ofstream::app);
	Log << "Generations" << ";" << "Offsprings" << ";" << "CurrentFailedOffsprings" << ";" << endl;

	size_t Index = 1;
	ifstream File;

	while (true)
	{
		wstring Filename = SysHelpers::GetAppUserStorageLocation() + L"\\Gladiator_" + to_wstring(Index) + L".Glad";
		File.open(Filename, ifstream::binary);
		BREAK_IF(!File.is_open() || File.bad());
		boost::archive::binary_iarchive ia(File);

		NeuralNetwork::AInstance* NewIntance;
		ia >> NewIntance;
		Opponents.push_back(NewIntance);
		File.close();
		Index++;
		Generations++;
	}


	if (Opponents.size() > 2)
	{
		delete Players[0]->NeuralNetworkInstance;
		delete Players[1]->NeuralNetworkInstance;
		Players[0]->NeuralNetworkInstance = Opponents[Opponents.size() - 2];
		Players[1]->NeuralNetworkInstance = Opponents[Opponents.size() - 1];
	}
	else
	{
		Opponents.push_back(Players[0]->NeuralNetworkInstance);
		Opponents.push_back(Players[1]->NeuralNetworkInstance);
	}
	

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
				if (LastGladiator == nullptr)
				{
					LastGladiator = Opponent;
				}
				Gladiator->GenerateOffspring(LastGladiator);
				Gladiator->Fitness = 0;
				Offsprings++;
				CurrentFailedOffsprings++;
				//PRINT "Bad Offspring" TAB Generations TAB Offsprings TAB OpponentIndex TAB Opponent->Fitness END;

				OpponentIndex = 0;


			}
			else if (OpponentIndex == Opponents.size() -1)
			{
				LastGladiator = Gladiator;
				Gladiator->Wins++;
				SaveGladiator(Gladiator, Generations);
				Players[1]->NeuralNetworkInstance = NeuralNetwork::AInstance::ConstructOffspring(Gladiator);
				Opponents.push_back(Players[1]->NeuralNetworkInstance);

				Generations++;
				Offsprings++;

				PRINT "Next Gen" TAB Generations TAB Offsprings TAB CurrentFailedOffsprings END;

				Log << to_string(Generations) << ";" << to_string(Offsprings) << ";" << to_string(CurrentFailedOffsprings) << ";" << endl;

				Opponent->Fitness = 0;

				OpponentIndex = 0;
				CurrentFailedOffsprings = 0;

				if (Generations % 50 == 0)
				{
					std::sort(Opponents.begin(), Opponents.end(), [](NeuralNetwork::AInstance* a, NeuralNetwork::AInstance* b)
					{
						return a->Wins > b->Wins;
					});
					if (Opponents.size() > 500)
					{
						for (int Index = int(Opponents.size() - 1); Index >= int(Opponents.size() - 50); --Index)
						{
							if (Opponents[Index] != Gladiator)
							{
								delete Opponents[Index];
								Opponents.erase(Opponents.begin() + Index);
							}
						}
					}

					PRINT "SORT" TAB Opponents.front()->Wins TAB Opponents.back()->Wins TAB Opponents.size() END;

				}
			}

			Opponent->Fitness = 0;
			Players[0]->NeuralNetworkInstance = Opponents[OpponentIndex];
			Players[0]->NeuralNetworkInstance->Fitness = 0;
			

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

void ACfInstance::SaveGladiator(NeuralNetwork::AInstance* Gladiator, size_t Index)
{
	ASmartWriteLock Lock(Gladiator->Mutex);

	ofstream fs(SysHelpers::GetAppUserStorageLocation() + L"\\Gladiator_" + to_wstring(Index) + L".Glad", ofstream::binary);
	if (fs)
	{
		boost::archive::binary_oarchive oa(fs);
		oa << Gladiator;
		fs.close();
	}
	else
	{
		cout << "Handle invalid";
	}
}



