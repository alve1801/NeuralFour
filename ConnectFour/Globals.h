#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

//#include "ALocalization.h"
#include <unordered_map>
#include "boost/function.hpp"
#include "AThreadInfo.h"
#include <random>

class AApp;

namespace Globals
{
	extern AApp* App;

	extern boost::thread_specific_ptr<AThreadInfo> ThreadInfos;

	extern std::vector<std::wstring> InitialArguments;

	extern double DeltaTime;

	extern bool bLearningPhase;

	extern std::uniform_int_distribution<int> CfMatrixRandomPosition;

}

#endif // !GLOBALS_H