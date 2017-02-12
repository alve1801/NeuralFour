#include "stdafx.h"
#include "ANeuralNetwork.h"
#include "Utils.h"


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




NeuralNetwork::ANode::ANode(VValue InBias) : Value(0), InValue(0), Bias(InBias)
{
}

NeuralNetwork::ANode::~ANode()
{
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
	else if (InValue < 0)
	{
		Value = -1;
	}
	else
	{
		Value = 0;
	}
}

void NeuralNetwork::ANode::Mutate()
{
	if (GetRandomWeight() < 0.2)
	{
		for (int Index = 0; Index < OutputEdges.size(); ++Index)
		{
			OutputEdges[Index]->Weight = Utils::Clamp(OutputEdges[Index]->Weight + 0.5 * (GetRandomWeight() * (GetRandomWeight() - 0.5)), 0, 1);
		}
		Bias = Utils::Clamp(Bias + 0.5 * (GetRandomWeight() * (GetRandomWeight() - 0.5)), 0, 1);
	}
}

void NeuralNetwork::ANode::OpressiveMerge(ANode* Opressor, NeuralNetwork::VValue OpressionFactor)
{
	if (GetRandomWeight() < OpressionFactor)
	{
		for (int Index = 0; Index < OutputEdges.size(); ++Index)
		{
			OutputEdges[Index]->Weight = Opressor->OutputEdges[Index]->Weight;
		}
		Bias = Opressor->Bias;
	}
}

void NeuralNetwork::ANode::Rebirth()
{
	for (int Index = 0; Index < OutputEdges.size(); ++Index)
	{
		OutputEdges[Index]->Weight = GetRandomWeight();
	}
	Bias = (GetRandomWeight() - 0.5) * 2;
}


NeuralNetwork::ALayer::ALayer(size_t NodeCount)
{
	for (int Index = 0; Index < NodeCount; ++Index)
	{
		Nodes.push_back(new ANode((GetRandomWeight() - 0.5) * 2));
	}
}

NeuralNetwork::ALayer::~ALayer()
{
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




NeuralNetwork::AInstance::AInstance(): Fitness(0), TotalFitness(0), HighestOpression(10), UnfitGenerations(0)
{
}

NeuralNetwork::AInstance::~AInstance()
{
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

void NeuralNetwork::AInstance::OpressiveMerge(AInstance* Opressor)
{
	ASmartWriteLock Lock(Mutex);
	ASmartWriteLock OtherLock(Opressor->Mutex);

	VValue OpressionFactor = Utils::Difference(Fitness, Opressor->Fitness);
	HighestOpression = max(HighestOpression, OpressionFactor);
	OpressionFactor = Utils::Clamp(OpressionFactor * 0.5, 0, 1);


	/*VValue CombinedFitness = abs(Fitness) + abs(Opressor->Fitness);
	VValue OpressionFactor;
	if (CombinedFitness != 0)
	{
		OpressionFactor = abs(Fitness) / CombinedFitness;
	}
	else
	{
		OpressionFactor = 0.5;
	}*/

	Fitness = 0;
	Opressor->Fitness = 0;

	for (int LayerIndex = 0; LayerIndex < Layers.size(); ++LayerIndex)
	{
		for (int NodeIndex = 0; NodeIndex < Layers[LayerIndex]->Nodes.size(); ++NodeIndex)
		{
			Layers[LayerIndex]->Nodes[NodeIndex]->OpressiveMerge(Opressor->Layers[LayerIndex]->Nodes[NodeIndex], OpressionFactor);
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
