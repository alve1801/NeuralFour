// ConnectFour 
#pragma once
#include "ASmartMutex.h"

#include "boost/serialization/access.hpp"
#include "boost/thread.hpp"
#include "boost/enable_shared_from_this.hpp"
#include <boost/serialization/version.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/enable_shared_from_this.hpp"

using namespace std;


namespace NeuralNetwork
{
	class ANode;
	class AEdge;
	class ALayer;
	class AInstance;


	typedef boost::shared_ptr<AEdge> ASharedEdge;
	typedef boost::shared_ptr<ANode> ASharedNode;
	typedef boost::shared_ptr<ALayer> ASharedLayer;
	typedef boost::shared_ptr<AInstance> ASharedInstance;

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
};

class NeuralNetwork::AEdge
{
public:
	AEdge();
	AEdge(ANode* InTaregetNode, VValue InWeight);
	~AEdge();

	void TransmitData(VValue Value) const;

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

	VValue Value;
	VValue InValue;
	VValue Bias;
	VValue CorrectValue;

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


	static ASharedInstance Construct();
	static ASharedInstance ConstructOffspring(ASharedInstance Parent);

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
	UInt Fights = 0;
	UInt Id;

	void Mutate();
	void CalcWeightDelta(int CorrectNode);
	void GenerateOffspring(ASharedInstance Parent0, ASharedInstance Parent1);
	void GenerateOffspring(ASharedInstance Parent);

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