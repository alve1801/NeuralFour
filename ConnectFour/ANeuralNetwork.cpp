#include "stdafx.h"
#include "ANeuralNetwork.h"
#include "Utils.h"


NeuralNetwork::AEdge::AEdge(): TargetNode(nullptr), Weight(0)
{
}

NeuralNetwork::AEdge::AEdge(ANode* InTargetNode, VValue InWeight) : TargetNode(InTargetNode), Weight(InWeight)
{
}

NeuralNetwork::AEdge::~AEdge()
{
}

void NeuralNetwork::AEdge::TransmitData(VResult Value) const
{
	TargetNode->InValue += VValue(Value) * Weight;
}




NeuralNetwork::ANode::ANode(): Value(0), InValue(0), Bias(0)
{
}

NeuralNetwork::ANode::ANode(VValue InBias) : Value(0), InValue(0), Bias(InBias)
{
}

NeuralNetwork::ANode::~ANode()
{
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		delete OutputEdges[Index];
	}
}

void NeuralNetwork::ANode::TransmitData()
{
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		OutputEdges[Index]->TransmitData(Value);
	}
}

void NeuralNetwork::ANode::Process()
{
	InValue += Bias;
	if (InValue > 0)
	{
		Value = 1;
	}
	else
	{
		Value = -1;
	}
}

void NeuralNetwork::ANode::Mutate()
{
	if (GetRandomWeight() > 0.7)
	{
		for (int Index = 0; Index < OutputEdges.size(); ++Index)
		{
			OutputEdges[Index]->Weight = Utils::Clamp(OutputEdges[Index]->Weight + 0.3f * GetRandomWeight(), -1, 1);
		}
		Bias = Utils::Clamp(Bias + 0.3f * GetRandomWeight(), -1, 1);
	}
}

void NeuralNetwork::ANode::GenerateOffspring(ANode* Parent0, ANode* Parent1)
{
	if (GetRandomWeight() < 0)
	{
		Parent0 = Parent1;
	}
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		OutputEdges[Index]->Weight = Parent0->OutputEdges[Index]->Weight;
	}
	Bias = Parent0->Bias;
}

void NeuralNetwork::ANode::GenerateOffspring(ANode* Parent)
{
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		OutputEdges[Index]->Weight = Parent->OutputEdges[Index]->Weight;
	}
	Bias = Parent->Bias;
	Mutate();
}

void NeuralNetwork::ANode::Rebirth()
{
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		OutputEdges[Index]->Weight = GetRandomWeight();
	}
	Bias = GetRandomWeight();
}




NeuralNetwork::ALayer::ALayer()
{
}

NeuralNetwork::ALayer::ALayer(size_t NodeCount)
{
	for (int Index = 0; Index < NodeCount; ++Index)
	{
		Nodes.push_back(new ANode(GetRandomWeight()));
	}
}

NeuralNetwork::ALayer::~ALayer()
{
	for (int Index = 0; Index < Nodes.size(); ++Index)
	{
		delete Nodes[Index];
	}
}

NeuralNetwork::ALayer* NeuralNetwork::ALayer::ConstructOffspring(ALayer* Parent)
{
	ALayer* NewLayer = new ALayer();
	for (int Index = 0; Index < Parent->Nodes.size(); ++Index)
	{
		NewLayer->Nodes.push_back(new ANode(Parent->Nodes[Index]->Bias));
	}
	return NewLayer;
}

void NeuralNetwork::ALayer::Process()
{
	for (int Index = 0; Index < Nodes.size(); ++Index)
	{
		Nodes[Index]->Process();
	}
}

void NeuralNetwork::ALayer::TransmitData()
{
	for (int Index = 0; Index < Nodes.size(); ++Index)
	{
		Nodes[Index]->TransmitData();
	}
}

void NeuralNetwork::ALayer::LinkLayer(ALayer* OtherLayer)
{
	for (int Index = 0; Index < Nodes.size(); ++Index)
	{
		for (int OtherIndex = 0; OtherIndex < OtherLayer->Nodes.size(); ++OtherIndex)
		{
			Nodes[Index]->OutputEdges.push_back(new AEdge(OtherLayer->Nodes[OtherIndex], GetRandomWeight()));
		}
	}
}




NeuralNetwork::AInstance::AInstance(): Fitness(0), TotalFitness(0), HighestOpression(0), Wins(0), Id(0)
{
}

NeuralNetwork::AInstance::~AInstance()
{
	ASmartWriteLock Lock(Mutex);
	PRINT "Destruct" TAB Fights END;
	for (int Index = 0; Index < Layers.size(); ++Index)
	{
		delete Layers[Index];
	}
}

NeuralNetwork::ASharedInstance NeuralNetwork::AInstance::Construct()
{
	ASharedInstance NewInstance = boost::make_shared<AInstance>();
	return NewInstance;
}

NeuralNetwork::ASharedInstance NeuralNetwork::AInstance::ConstructOffspring(ASharedInstance Parent)
{
	ASharedInstance NewInstance = boost::make_shared<AInstance>();

	for (int LayerIndex = 0; LayerIndex < Parent->Layers.size(); ++LayerIndex)
	{
		NewInstance->Layers.push_back(ALayer::ConstructOffspring(Parent->Layers[LayerIndex]));

		if (NewInstance->Layers.size() > 1)
		{
			ALayer* LayerTop = NewInstance->Layers[NewInstance->Layers.size() - 1];
			ALayer* LayerBot = NewInstance->Layers[NewInstance->Layers.size() - 2];


			for (int LayerBotNodeIndex = 0; LayerBotNodeIndex < LayerBot->Nodes.size(); ++LayerBotNodeIndex)
			{
				for (int LayerTopNodeIndex = 0; LayerTopNodeIndex < LayerTop->Nodes.size(); ++LayerTopNodeIndex)
				{
					LayerBot->Nodes[LayerBotNodeIndex]->OutputEdges.push_back(new AEdge(LayerTop->Nodes[LayerTopNodeIndex], Parent->Layers[LayerIndex-1]->Nodes[LayerBotNodeIndex]->OutputEdges[LayerTopNodeIndex]->Weight));
				}
			}
		}
	}
	NewInstance->Mutate();
	return NewInstance;
}

void NeuralNetwork::AInstance::AddLayer(size_t NodeCount)
{
	ASmartWriteLock Lock(Mutex);

	Layers.push_back(new ALayer(NodeCount));
	if (Layers.size() > 1)
	{
		Layers[Layers.size() - 2]->LinkLayer(Layers[Layers.size() - 1]);
	}
}

void NeuralNetwork::AInstance::GenerateDna()
{
	ASmartWriteLock Lock(Mutex);

	Dna.clear();

	/*Dna.append(ValueSerialization(Layers.size()));

	for (int Index = 0; Index < Layers.size(); ++Index)
	{
		Dna.append(ValueSerialization(Layers[Index]->Nodes.size()));
	}

	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Dna.append(ValueSerialization(Layers[LayerIndex]->Nodes[NodeIndex]->Bias));

			for (int OutputEdgeIndex = 0; OutputEdgeIndex < Layers[LayerIndex]->Nodes[NodeIndex]->OutputEdges.size(); ++OutputEdgeIndex)
			{
				Dna.append(ValueSerialization(Layers[LayerIndex]->Nodes[NodeIndex]->OutputEdges[OutputEdgeIndex]->Weight));

			}
		}
	}*/
}

void NeuralNetwork::AInstance::ResetInputs()
{
	ASmartWriteLock Lock(Mutex);

	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->InValue = 0;
		}
	}
}

void NeuralNetwork::AInstance::Process()
{
	ASmartWriteLock Lock(Mutex);

	ResetInputs();

	Layers[0]->TransmitData();

	for (int Index = 1; Index < Layers.size() - 1; ++Index)
	{
		Layers[Index]->Process();

		Layers[Index]->TransmitData();
	}

	Layers[Layers.size() - 1]->Process();
}

void NeuralNetwork::AInstance::Mutate()
{
	ASmartWriteLock Lock(Mutex);
	Fitness = 0;
	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->Mutate();
		}
	}
}

void NeuralNetwork::AInstance::GenerateOffspring(NeuralNetwork::ASharedInstance Parent0, NeuralNetwork::ASharedInstance Parent1)
{
	ASmartWriteLock Lock(Mutex);
	ASmartWriteLock Parent0Lock(Parent0->Mutex);
	ASmartWriteLock Parent1Lock(Parent1->Mutex);


	Fitness = 0;
	TotalFitness = 0;

	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->GenerateOffspring(Parent0->Layers[LayerIndex]->Nodes[NodeIndex], Parent1->Layers[LayerIndex]->Nodes[NodeIndex]);
		}
	}


}

void NeuralNetwork::AInstance::GenerateOffspring(ASharedInstance Parent)
{
	ASmartWriteLock Lock(Mutex);
	ASmartWriteLock ParentLock(Parent->Mutex);


	Fitness = 0;
	TotalFitness = 0;

	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->GenerateOffspring(Parent->Layers[LayerIndex]->Nodes[NodeIndex]);
		}
	}
}

void NeuralNetwork::AInstance::Rebirth()
{
	ASmartWriteLock Lock(Mutex);
	Fitness = 0;
	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->Rebirth();
		}
	}
}
