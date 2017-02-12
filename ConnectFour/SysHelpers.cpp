#include "stdafx.h"
#include "SysHelpers.h"
#include "boost\function.hpp"
#include "boost\filesystem.hpp"

#ifdef SYS_IS_WINDOWS

#endif

#ifdef SYS_IS_LINUX

#endif

#ifdef SYS_IS_MAC

#endif

namespace SysHelpers
{
	SysType Sys = Sys_Windows;


	void Init()
	{
#ifdef SYS_IS_WINDOWS
		Sys = Sys_Windows;
#endif

#ifdef SYS_IS_LINUX
		SysHelpers::SysType SysHelpers::Sys = SysHelpers::Sys_Linux;
#endif

#ifdef SYS_IS_MAC
		SysHelpers::SysType SysHelpers::Sys = SysHelpers::Sys_Mac;
#endif
	}

	wstring GetAppUserStorageLocation()
	{
		return SYS_HELPERS::GetAppUserStorageLocation();
	}

	wstring GetTempStorageLocation()
	{
		return SYS_HELPERS::GetTempStorageLocation();
	}

	vector<wstring> GetCommandLineArguments()
	{
		return SYS_HELPERS::GetCommandLineArguments();
	}

	bool CreateNewDirectory(wstring Path)
	{
		if (!boost::filesystem::exists(Path))
		{
			boost::filesystem::create_directory(Path);
		}
		return true;
	}

	FFilePathData BasicFileSave(wstring InitialFileName, wstring InitialDir, wstring FileFilter, wstring DefaultExtension)
	{
		return SYS_HELPERS::BasicFileSave(InitialFileName, InitialDir, FileFilter, DefaultExtension);
	}

	FFilePathData ImageFileSave(wstring InitialFileName, wstring InitialDir, wstring DefaultExtension)
	{
		return SYS_HELPERS::ImageFileSave(InitialFileName, InitialDir, DefaultExtension);
	}

	FFilePathData BasicFileOpen(wstring InitialFileName, wstring InitialDir, wstring FileFilter, wstring DefaultExtension)
	{

		return SYS_HELPERS::BasicFileOpen(InitialFileName, InitialDir, FileFilter, DefaultExtension);
	}

	bool CopyBitmapToClipboard(UI::AWindow* ParentWindow, wstring Path)
	{
		return SYS_HELPERS::CopyBitmapToClipboard(ParentWindow, Path);
	}

	FPoint GetCursorPosition()
	{
		return SYS_HELPERS::GetCursorPosition();
	}

}


