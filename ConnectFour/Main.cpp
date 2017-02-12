#include "stdafx.h"
#include "AApp.h"
#include "SysHelpers.h"
#include "Utils.h"
#include <boost/filesystem/operations.hpp>
#include "VersionProperties.h"
#include "Globals.h"

using namespace std;


int main(int argc, char* args[])
{

	SysHelpers::Init();

	Globals::InitialArguments = SysHelpers::GetCommandLineArguments();
#if DEPLOY_VERSION
	{
		wstring WorkingDirectory = Utils::PathSegment(Globals::InitialArguments[0], 0, -2);
		boost::filesystem::current_path(WorkingDirectory);
	}
#endif


	AApp App;
	Globals::App = &App;
	return App.Execute();
}