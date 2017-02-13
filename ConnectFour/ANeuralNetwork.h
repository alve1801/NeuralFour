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

	inline string ValueSerialization(VValue Value)
	{
		char Data[8], *pDouble = (char*)(double*)(&Value);
		for (int i = 0; i < 8; ++i)
		{
			Data[i] = pDouble[7 - i];
		}

		return string(Data);
	}

	inline string ValueSerialization(size_t Value)
	{
		string Result;
		char* pDouble = (char*)(ULong*)(&Value);
		for (int Index = 0; Index < 8; ++Index)
		{
			Result.push_back(pDouble[7 - Index]);
		}

		return Result;
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
	void GenerateOffspring(ANode* Parent);
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

	/**
	 * \brief 
	 * 8chars LayerCount
	 * Chars 8->LayerCount*8 LayerNodeCount
	 */
	string Dna;
	vector<ALayer*> Layers;

	ASmartMutex Mutex;

	void GenerateDna();

	void ResetInputs();
	void Process();
	void AddLayer(size_t NodeCount);

	VValue Fitness;
	VValue TotalFitness;

	VValue HighestOpression;

	UInt Wins;

	void Mutate();
	void GenerateOffspring(AInstance* Parent0, AInstance* Parent1);
	void GenerateOffspring(AInstance* Parent);

	void Rebirth();

};