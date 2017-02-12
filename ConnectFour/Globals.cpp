#include "stdafx.h"
#include "Globals.h"

namespace Globals
{
	AApp* App;
	boost::thread_specific_ptr<AThreadInfo> ThreadInfos;
	std::vector<std::wstring> InitialArguments;

	double DeltaTime;

	bool bLearningPhase = true;

	std::uniform_int_distribution<int> CfMatrixRandomPosition(0, 4);

}