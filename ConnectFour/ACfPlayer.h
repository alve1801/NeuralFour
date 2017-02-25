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
	virtual ~ACfPlayer();
	bool bHuman = false;
protected:
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

	NeuralNetwork::ASharedInstance NeuralNetworkInstance;


	void InsertChip(const UChar Position);

	virtual int NextMove();

	virtual void Success();

	virtual void Failure();

	virtual void Reset();

};



class ACfHumanPlayer : public ACfPlayer
{
public:
	ACfHumanPlayer(ACfInstance* InInstance, UChar Index);
	~ACfHumanPlayer();

	int NextMove() override;

	void Success() override;

	void Failure() override;

	void Reset() override;
};