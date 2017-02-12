#pragma once


#ifndef WINDOWSHELPERS_H
#define WINDOWSHELPERS_H

#include "SysHelpers.h"
#include "Utils.h"


#ifdef SYS_IS_WINDOWS


#define ENTRY_MAIN winMain

#include <winsock2.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <iphlpapi.h>

#include <shlobj.h>

#include <windows.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <Commdlg.h>

#include <shlwapi.h>
#include "shlobj.h"

#pragma comment(lib, "User32")
#pragma comment(lib, "Comdlg32")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "Shell32")
#pragma comment(lib, "Ole32")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "IPHLPAPI")
#pragma comment(lib,"shlwapi.lib")


namespace UI 
{
	class AWindow;
}


using namespace std;

namespace WindowsHelpers
{

	wstring GetAppUserStorageLocation();
	wstring GetTempStorageLocation();

	vector<wstring> GetCommandLineArguments();


	FFilePathData BasicFileSave(wstring InitialFileName = L"", wstring InitialDir = L"", wstring FileFilter = L"", wstring DefaultExtension = L"");
	FFilePathData ImageFileSave(wstring InitialFileName = L"", wstring InitialDir = L"", wstring DefaultExtension = L"");

	FFilePathData BasicFileOpen(wstring InitialFileName = L"", wstring InitialDir = L"", wstring FileFilter = L"", wstring DefaultExtension = L"");

	bool CopyBitmapToClipboard(UI::AWindow* ParentWindow, wstring Path);

	FPoint GetCursorPosition();

}


#endif
#endif