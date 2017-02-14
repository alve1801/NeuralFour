// ConnectFour 
#pragma once
#include "ASmartMutex.h"

#include "boost/serialization/access.hpp"
#include "boost/thread.hpp"
#include "boost/enable_shared_from_this.hpp"
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;


namespace NeuralNetwork
{
	class ANode;
	class AEdge;
	class ALayer;
	class AInstance;

	typedef signed char VResult;
	typedef float VValue;

	inline VValue GetRandomWeight()
	{
		return (VValue(rand()) / VValue(RAND_MAX)- 0.5f)*2;
	}

	inline VResult GetResultFromBool(bool Input)
	{
		return (Input ? 1 : -1);
	}

	/*inline string ValueSerialization(VValue Value)
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
	}*/

};

class NeuralNetwork::AEdge
{
public:
	AEdge();
	AEdge(ANode* InTaregetNode, VValue InWeight);
	~AEdge();

	void TransmitData(VResult Value) const;

	ANode* TargetNode;
	VValue Weight;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int Version)
	{
		ar & Weight;
		ar & TargetNode;
	}

};
BOOST_CLASS_VERSION(NeuralNetwork::AEdge, 1);

class NeuralNetwork::ANode
{
public:
	ANode();
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


	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int Version)
	{
		ar & OutputEdges;
		ar & Bias;
	}

};
BOOST_CLASS_VERSION(NeuralNetwork::ANode, 1);


class NeuralNetwork::ALayer
{
public:
	ALayer();
	ALayer(size_t NodeCount);
	~ALayer();

	static ALayer* ConstructOffspring(ALayer* Parent);

	vector<ANode*> Nodes;



	void Process();
	void TransmitData();
	void LinkLayer(ALayer* OtherLayer);

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int Version)
	{
		ar & Nodes;
	}

};
BOOST_CLASS_VERSION(NeuralNetwork::ALayer, 1);

class NeuralNetwork::AInstance
{
public:
	AInstance();
	~AInstance();


	static AInstance* ConstructOffspring(AInstance* Parent);

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
	UInt Id;

	void Mutate();
	void GenerateOffspring(AInstance* Parent0, AInstance* Parent1);
	void GenerateOffspring(AInstance* Parent);

	void Rebirth();

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int Version)
	{
		ar & Id;
		ar & Layers;
		ar & Wins;
		ar & Fitness;
		ar & TotalFitness;
	}

};
BOOST_CLASS_VERSION(NeuralNetwork::AInstance, 1);