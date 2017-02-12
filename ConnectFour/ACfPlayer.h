// Merels 
#pragma once
#include "Definitions.h"
#include <random>
#include "ANeuralNetwork.h"

#define OTHER_PLAYER_INDEX(PlayerIndex) (PlayerIndex+1)%2

class ACfInstance;

class ACfPlayer
{
public:

	explicit ACfPlayer(ACfInstance* InInstance, UChar Index);
	~ACfPlayer();

private:
	UChar Id;
	ACfInstance* Instance;

public:
	ACfInstance* GetInstance() const
	{
		return Instance;
	}

	UChar GetId() const
	{
		return Id;
	}
	UChar GetIndex() const
	{
		return Id + 1;
	}

	NeuralNetwork::AInstance* NeuralNetworkInstance;


	void InsertChip(const UChar Position);

	bool NextMove();

	void Success();

	void Failure();

	void Reset();

};
