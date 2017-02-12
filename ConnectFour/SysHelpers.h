#pragma once

#ifndef SYSHELPERS_H
#define SYSHELPERS_H

#ifndef SysHELPERS_H
#define SysHELPERS_H

#include "Utils.h"


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#define SYS_HELPERS WindowsHelpers
#define SYS_IS_WINDOWS
#include "WindowsHelpers.h"


#elif defined(__unix__ ) || defined(__linux__ )

#define SYS_IS_LINUX
#define SYS_HELPERS LinuxHelpers
#include "LinuxHelpers.h"


#elif defined(__APPLE__)

#define SYS_IS_MAC
#define SYS_HELPERS MacHelpers

#endif

namespace UI 
{
	class AWindow;
}


using std::wstring;



namespace SysHelpers
{
	enum SysType
	{
		Sys_Windows,
		Sys_Linux,
		Sys_Mac
	};
	extern SysType Sys;

	void Init();

	wstring GetAppUserStorageLocation();
	wstring GetTempStorageLocation();

	vector<wstring> GetCommandLineArguments();

	bool CreateNewDirectory(wstring Path);

	FFilePathData BasicFileSave(wstring InitialFileName = L"", wstring InitialDir = L"", wstring FileFilter = L"", wstring DefaultExtension = L"");
	FFilePathData ImageFileSave(wstring InitialFileName = L"", wstring InitialDir = L"", wstring DefaultExtension = L"");

	FFilePathData BasicFileOpen(wstring InitialFileName = L"", wstring InitialDir = L"", wstring FileFilter = L"", wstring DefaultExtension = L"");

	bool CopyBitmapToClipboard(UI::AWindow* ParentWindow, wstring Path);

	FPoint GetCursorPosition();


}



#endif

#endif