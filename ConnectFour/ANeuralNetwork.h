// ConnectFour 
#pragma once
#include "ASmartMutex.h"

using namespace std;


namespace NeuralNetwork
{
	class ANode;
	class AEdge;
	class ALayer;
	class AInstance;

	typedef signed char VResult;
	typedef double VValue;

	inline VValue GetRandomWeight()
	{
		return (static_cast<VValue>(rand()) / static_cast <VValue>(RAND_MAX) - 0.5)*2;
	}

	inline VResult GetResultFromBool(bool Input)
	{
		return (Input ? 1 : -1);
	}

};

class NeuralNetwork::AEdge
{
public:
	AEdge(ANode* InTaregetNode, VValue InWeight);
	~AEdge();

	void TransmitData(VResult Value) const;

	ANode* TargetNode;
	VValue Weight;
};

class NeuralNetwork::ANode
{
public:
	ANode(VValue InBias);
	~ANode();


	vector<AEdge*> OutputEdges;

	VResult Value;
	VValue InValue;
	VValue Bias;

	void TransmitData();

	void Process();

	void Mutate();
	void GenerateOffspring(ANode* Parent0, ANode* Parent1);
	void Rebirth();

};


class NeuralNetwork::ALayer
{
public:
	ALayer(size_t NodeCount);
	~ALayer();


	vector<ANode*> Nodes;



	void Process();
	void TransmitData();
	void LinkLayer(ALayer* OtherLayer);

};


class NeuralNetwork::AInstance
{
public:
	AInstance();
	~AInstance();


	vector<ALayer*> Layers;

	ASmartMutex Mutex;

	void ResetInputs();
	void Process();
	void AddLayer(size_t NodeCount);

	VValue Fitness;
	VValue TotalFitness;

	VValue HighestOpression;

	UChar UnfitGenerations;

	void Mutate();
	void GenerateOffspring(AInstance* Parent0, AInstance* Parent1);

	void Rebirth();

};