#include "stdafx.h"
#include "WindowsHelpers.h"

#ifdef SYS_IS_WINDOWS

#include "Utils.h"
#include "cpprest/filestream.h"
#include "cpprest/http_client.h"
#include "cpprest/json.h"
#include "cpprest/producerconsumerstream.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "AApp.h"
#include "Shellapi.h"
#include "Globals.h"
#include "SDL_syswm.h"
#include "VersionProperties.h"



using namespace std;

using namespace utility;
using namespace concurrency::streams;

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace web::json;

namespace WindowsHelpers
{
	wstring GetAppUserStorageLocation()
	{
		static wstring Path;
		if (Path != L"")
		{
			return Path;
		}
		TCHAR AppDataPath[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, AppDataPath)))
		{
			Path = AppDataPath;
			Path.append(L"\\" APP_NAME_W);
		}
		return Path;
	}

	wstring GetTempStorageLocation()
	{
		static wstring Path;
		if (Path != L"")
		{
			return Path;
		}
		wchar_t Buffer[MAX_PATH] = L"\0";
		GetTempPathW(MAX_PATH, Buffer);
		Path = Buffer;
		return Path;
	}

	vector<wstring> GetCommandLineArguments()
	{
		wstring CommandLine = GetCommandLineW();
		wchar_t** ResultBuffer;
		int ResultCount = 0;
		ResultBuffer = CommandLineToArgvW(CommandLine.data(), &ResultCount);
		vector<wstring> Result;
		for (int Index = 0; Index < ResultCount; ++Index)
		{
			Result.push_back(ResultBuffer[Index]);
		}
		return Result;
	}

	FFilePathData BasicFileSave(wstring InitialFileName, wstring InitialDir, wstring FileFilter, wstring DefaultExtension)
	{
		wchar_t Path[MAX_PATH] = L"\0";
		wchar_t FileTitle[MAX_PATH] = L"\0";

		OPENFILENAMEW ofn;
		ZeroMemory(&Path, sizeof(Path));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;


		if (FileFilter.size())
		{
			wchar_t Filter[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(FileFilter, Filter);

			for (size_t Index = 0; Index < MAX_PATH; Index++)
			{
				if (Filter[Index] == '|')
				{
					Filter[Index] = '\0';
				}
			}

			ofn.lpstrFilter = Filter;
		}
		else
		{
			ofn.lpstrFilter = L"Any File\0*.*\0";
		}

		ofn.lpstrFile = Path;
		ofn.lpstrFileTitle = FileTitle;
		Utils::WstringToWSTR(InitialFileName, Path);
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (InitialDir.size())
		{
			wchar_t InDir[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(InitialDir, InDir);
			ofn.lpstrInitialDir = InDir;
		}

		if (DefaultExtension.size())
		{
			wchar_t DefExt[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(DefaultExtension, DefExt);
			ofn.lpstrDefExt = DefExt;
		}
		//ofn.lpstrInitialDir = L"C:\\a.png";


		if (GetSaveFileNameW(&ofn))
		{
			return FFilePathData(Path, FileTitle, wstring(Path).substr(ofn.nFileExtension));
		}

		return FFilePathData();
	}

	FFilePathData ImageFileSave(wstring InitialFileName, wstring InitialDir, wstring DefaultExtension)
	{
		return BasicFileSave(InitialFileName, InitialDir, L"PNG Files|*.png|JPG Files|*.jpg", DefaultExtension);
	}

	FFilePathData BasicFileOpen(wstring InitialFileName, wstring InitialDir, wstring FileFilter, wstring DefaultExtension)
	{
		wchar_t Path[MAX_PATH] = L"\0";
		wchar_t FileTitle[MAX_PATH] = L"\0";

		OPENFILENAMEW ofn;
		ZeroMemory(&Path, sizeof(Path));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;


		if (FileFilter.size())
		{
			wchar_t Filter[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(FileFilter, Filter);

			for (size_t Index = 0; Index < MAX_PATH; Index++)
			{
				if (Filter[Index] == '|')
				{
					Filter[Index] = '\0';
				}
			}

			ofn.lpstrFilter = Filter;
		}
		else
		{
			ofn.lpstrFilter = L"Any File\0*.*\0";
		}

		ofn.lpstrFile = Path;
		ofn.lpstrFileTitle = FileTitle;
		Utils::WstringToWSTR(InitialFileName, Path);
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (InitialDir.size())
		{
			wchar_t InDir[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(InitialDir, InDir);
			ofn.lpstrInitialDir = InDir;
		}

		if (DefaultExtension.size())
		{
			wchar_t DefExt[MAX_PATH] = L"\0";
			Utils::WstringToWSTR(DefaultExtension, DefExt);
			ofn.lpstrDefExt = DefExt;
		}
		//ofn.lpstrInitialDir = L"C:\\a.png";


		if (GetOpenFileNameW(&ofn))
		{
			return FFilePathData(Path, FileTitle, wstring(Path).substr(ofn.nFileExtension));
		}

		return FFilePathData();
	}

	bool CopyBitmapToClipboard(UI::AWindow* ParentWindow, wstring Path)
	{
		struct stat fs = { 0 };
		/*if (!stat(Path.data(), &fs) == 0)
		{
			return false;

		}*/

		HDC hdc = CreateCompatibleDC(nullptr);

		wchar_t Buffer[MAX_PATH] = L"\0";
		Utils::WstringToWSTR(Path, Buffer);

		LPCWSTR BufferLPWSTR = Buffer;


		

		HBITMAP hBitmap = static_cast<HBITMAP>(LoadImage(nullptr, BufferLPWSTR, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));

		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_bool i = SDL_GetWindowWMInfo(ParentWindow->Window, &wmInfo);
		HWND Handle = wmInfo.info.win.window;


		if (OpenClipboard(Handle))
		{
			EmptyClipboard();
			SetClipboardData(CF_BITMAP, hBitmap);
			CloseClipboard();
			DeleteDC(hdc);
			return true;
		}
		return false;
	}

	FPoint GetCursorPosition()
	{
		POINT Position;
		GetCursorPos(&Position);
		return FPoint(Position.x, Position.y);
	}
}

#endif

