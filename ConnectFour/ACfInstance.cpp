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


	MaxLivingOpponents = 500;
	OppponentsUpdateTick = 50;
}

ACfInstance::~ACfInstance()
{
}

void ACfInstance::Init()
{
	Players[0] = new ACfPlayer(this, 0);
	Players[1] = new ACfHumanPlayer(this, 1);

	Log.open(SysHelpers::GetAppUserStorageLocation() + L"\\Log.csv", ofstream::app);
	Log << "Generations" << ";" << "Offsprings" << ";" << "CurrentFailedOffsprings" << ";" << endl;

	size_t Index = 1;
	ifstream File;

	while (ALWAYS_FALSE true)
	{
		try
		{
			wstring Filename = SysHelpers::GetAppUserStorageLocation() + L"\\Gladiator_" + to_wstring(Index) + L".Glad";
			File.open(Filename, ifstream::binary);
			BREAK_IF(!File.is_open() || File.bad());
			boost::archive::binary_iarchive ia(File);

			NeuralNetwork::ASharedInstance NewIntance;
			ia >> NewIntance;
			Opponents.push_back(NewIntance);
			File.close();
			Index++;
			Generations++;
		
			PRINT NewIntance->Wins TAB Generations END;
			if (Opponents.size() > MaxLivingOpponents && Generations % OppponentsUpdateTick == 0)
			{
				DecimateOpponents();
			}
		}
		catch (...)
		{
			break;
		}
	}


	if (Opponents.size() > 2)
	{
		//delete Players[0]->NeuralNetworkInstance;
		//delete Players[1]->NeuralNetworkInstance;
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

			//NeuralNetwork::ASharedInstance Opponent = Players[0]->NeuralNetworkInstance;
			//NeuralNetwork::ASharedInstance Gladiator = Players[1]->NeuralNetworkInstance;
			//OpponentIndex++;
			//if (Gladiator->Fitness < Opponent->Fitness)
			//{
			//	Opponent->Wins++;
			//	Opponent->Fitness = 0;
			//	SaveGladiator(Opponent);

			//	if (LastGladiator == nullptr)
			//	{
			//		LastGladiator = Opponent;
			//	}
			//	Gladiator->GenerateOffspring(LastGladiator);
			//	Gladiator->Fitness = 0;
			//	Offsprings++;
			//	CurrentFailedOffsprings++;
			//	//PRINT "Bad Offspring" TAB Generations TAB Offsprings TAB OpponentIndex TAB Opponent->Fitness END;

			//	OpponentIndex = 0;


			//}
			//else if (OpponentIndex == Opponents.size() -1)
			//{
			//	Opponents.push_back(Gladiator);

			//	LastGladiator = Gladiator;
			//	Gladiator->Wins++;
			//	SaveGladiator(Gladiator);
			//	Players[1]->NeuralNetworkInstance = NeuralNetwork::AInstance::ConstructOffspring(Gladiator);
			//	Players[1]->NeuralNetworkInstance->Fights++;
			//	Players[1]->NeuralNetworkInstance->Id = UInt(Generations);
			//	Generations++;
			//	Offsprings++;

			//	PRINT "Next Gen" TAB Generations TAB Offsprings TAB CurrentFailedOffsprings END;

			//	Log << to_string(Generations) << ";" << to_string(Offsprings) << ";" << to_string(CurrentFailedOffsprings) << ";" << endl;

			//	Opponent->Fitness = 0;

			//	OpponentIndex = 0;
			//	CurrentFailedOffsprings = 0;

			//	if (Generations % OppponentsUpdateTick == 0)
			//	{
			//		PRINT "SORT_START" TAB Opponents.front()->Wins TAB Opponents.back()->Wins TAB Opponents.size() END;

			//		DecimateOpponents();
			//		PRINT "SORT_STOP" TAB Opponents.front()->Wins TAB Opponents.back()->Wins TAB Opponents.size() END;

			//	}
			//}

			//Opponent->Fitness = 0;
			//Players[0]->NeuralNetworkInstance = Opponents[OpponentIndex];
			//Players[0]->NeuralNetworkInstance->Fitness = 0;
			//Players[0]->NeuralNetworkInstance->Fights++;
			

			Rounds = 0;


		}

		
	}
}

void ACfInstance::DecimateOpponents()
{
	std::sort(Opponents.begin(), Opponents.end(), [](NeuralNetwork::ASharedInstance a, NeuralNetwork::ASharedInstance b)
	{
		return a->Wins > b->Wins;
	});
	if (Opponents.size() > MaxLivingOpponents)
	{
		for (int Index = int(Opponents.size() - 1); Index >= MaxLivingOpponents - OppponentsUpdateTick; --Index)
		{
			if (Opponents[Index] != LastGladiator && Opponents[Index] != Players[1]->NeuralNetworkInstance && (Opponents[Index]->Fights > OppponentsUpdateTick || Opponents[Index]->Fights == 0))
			{
				Opponents.erase(Opponents.begin() + Index);
			}
		}
	}
	OpponentIndex = 0;
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

			Lock.Unlock();
			bool bResult = Players[PlayerIndex]->NextMove();
			Lock.Lock();


			if (!bResult)
			{
				Players[PlayerIndex]->Failure();
				bEnd = true;
			}

			else if (CheckForSuccess(Players[PlayerIndex]))
			{
				PRINT "Over" END;
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
			Lock.Unlock();
			Globals::App->bWaitingForPlayer = true;
			while (Globals::App->bWaitingForPlayer)
			{
				THREAD_SLEEP_MS(100);
			}
			Lock.Lock();
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
	UChar player = Player->GetId();

	for (int a = 0; a < MATRIX_HEIGHT; ++a)
	{
		for (int b = 0; b < MATRIX_WIDTH; ++b)
		{
			CONTINUE_IF(FigureMatrix[FVector<UChar>(b, a)] != player);
			int vertical = 1;//(|)
			int horizontal = 1;//(-)
			int diagonal1 = 1;//(\)
			int diagonal2 = 1;//(/)
			int i;//vertical
			int ii;//horizontal
					//check for vertical(|)
			for (i = a + 1; i <= MATRIX_HEIGHT && FigureMatrix[FVector<UChar>(b, i)] == player; i++, vertical++);//Check down
			for (i = a - 1; i >= 0 && FigureMatrix[FVector<UChar>(b, i)] == player; i--, vertical++);//Check up
			if (vertical >= MATRIX_WIN)return true;
			//check for horizontal(-)
			for (ii = b - 1; ii >= 0 && FigureMatrix[FVector<UChar>(ii, a)] == player; ii--, horizontal++);//Check left
			for (ii = b + 1; ii <= MATRIX_WIDTH && FigureMatrix[FVector<UChar>(ii, a)] == player; ii++, horizontal++);//Check right
			if (horizontal >= MATRIX_WIN) return true;
			//check for diagonal 1 (\)
			for (i = a - 1, ii = b - 1; i >= 0 && ii >= 0 && FigureMatrix[FVector<UChar>(ii, i)] == player; diagonal1++, i--, ii--);//up and left
			for (i = a + 1, ii = b + 1; i <= MATRIX_HEIGHT && ii <= MATRIX_WIDTH && FigureMatrix[FVector<UChar>(ii, i)] == player; diagonal1++, i++, ii++);//down and right
			if (diagonal1 >= MATRIX_WIN) return true;
			//check for diagonal 2(/)
			for (i = a - 1, ii = b + 1; i >= 0 && ii <= MATRIX_WIDTH && FigureMatrix[FVector<UChar>(ii, i)] == player; diagonal2++, i--, ii++);//up and right
			for (i = a + 1, ii = b - 1; i <= MATRIX_HEIGHT && ii >= 0 && FigureMatrix[FVector<UChar>(ii, i)] == player; diagonal2++, i++, ii--);//down and left
			if (diagonal2 >= MATRIX_WIN) return true;
		}
	}



	return false;
}

void ACfInstance::SaveGladiator(NeuralNetwork::ASharedInstance Gladiator)
{
	ASmartWriteLock Lock(Gladiator->Mutex);

	ofstream fs(SysHelpers::GetAppUserStorageLocation() + L"\\Gladiator_" + to_wstring(Gladiator->Id) + L".Glad", ofstream::binary);
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



