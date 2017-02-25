#include "stdafx.h"
#include "ACfPlayer.h"
#include "ACfInstance.h"
#include "Globals.h"
#include "AApp.h"


ACfPlayer::ACfPlayer(ACfInstance* InInstance, UChar Index)
{
	Instance = InInstance;
	Id = Index + 1;
	if (Index == 0)
	{
		NeuralNetworkInstance = NeuralNetwork::AInstance::Construct();
		NeuralNetworkInstance->AddLayer(MATRIX_WIDTH * MATRIX_HEIGHT * 3);
		NeuralNetworkInstance->AddLayer(size_t(MATRIX_WIDTH * MATRIX_HEIGHT * 3 * 2));
		NeuralNetworkInstance->AddLayer(MATRIX_WIDTH);
	}

}

ACfPlayer::~ACfPlayer()
{
}



void ACfPlayer::InsertChip(const UChar Position)
{
	//auto Move = Instance->ArchiveMove(Position);
	//Moves.push_back(AMove(Move, Position));
	ASmartWriteLock Lock(Instance->Mutex);

	Instance->FigureMatrix.Insert(Position, GetId());
}

int ACfPlayer::NextMove()
{
	NeuralNetwork::ALayer* InputLayer = NeuralNetworkInstance->Layers[0];
	ASmartWriteLock Lock(Instance->Mutex);
	for (int Index = 0; Index < 25; ++Index)
	{
		UChar PlayerId = Instance->FigureMatrix[Index];
		InputLayer->Nodes[Index * 2]->Value = NeuralNetwork::GetResultFromBool(PlayerId == 0);
		InputLayer->Nodes[Index * 2 + 1]->Value = NeuralNetwork::GetResultFromBool(PlayerId == Id);
		InputLayer->Nodes[Index * 3 + 2]->Value = NeuralNetwork::GetResultFromBool(PlayerId != Id && PlayerId != 0);
	}
	NeuralNetworkInstance->Process();

	NeuralNetwork::ALayer* OutputLayer = NeuralNetworkInstance->Layers[NeuralNetworkInstance->Layers.size() - 1];

	NeuralNetwork::VValue HigestVal = OutputLayer->Nodes[0]->InValue;
	int BestIndex = 0;

	for (int Index = 1; Index < OutputLayer->Nodes.size(); ++Index)
	{
		if (OutputLayer->Nodes[Index]->InValue > HigestVal)
		{
			HigestVal = OutputLayer->Nodes[Index]->InValue;
			BestIndex = Index;
		}
	}
	if (!Instance->FigureMatrix.HasSpace(BestIndex))
	{
		return -1;
	}

	InsertChip(BestIndex);

	return BestIndex;

}


void ACfPlayer::Success()
{
	NeuralNetworkInstance->Fitness += 1;
	NeuralNetworkInstance->TotalFitness += 1;

}

void ACfPlayer::Failure()
{
	NeuralNetworkInstance->Fitness -= 1;
	NeuralNetworkInstance->TotalFitness -= 1;
}

void ACfPlayer::Reset()
{
	//Moves.clear();
}






ACfHumanPlayer::ACfHumanPlayer(ACfInstance* InInstance, UChar Index) : ACfPlayer(InInstance, Index)
{
	bHuman = true;
}

ACfHumanPlayer::~ACfHumanPlayer()
{
}

int ACfHumanPlayer::NextMove()
{
	Globals::App->bWaitingForPlayer = true;
	while (true)
	{
		while (Globals::App->bWaitingForPlayer)
		{
			THREAD_SLEEP_MS(100);
		}
		ASmartWriteLock Lock(Instance->Mutex);

		if (Instance->FigureMatrix.HasSpace(Globals::App->PlayerInput))
		{
			InsertChip(Globals::App->PlayerInput);
			return Globals::App->PlayerInput;
		}
	}

	return -1;
}

void ACfHumanPlayer::Success()
{
	PRINT "WIN!" END;
}

void ACfHumanPlayer::Failure()
{
	PRINT "DEFEAT!" END;

}

void ACfHumanPlayer::Reset()
{
}
