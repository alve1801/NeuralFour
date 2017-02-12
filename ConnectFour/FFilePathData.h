
#ifndef FFILEPATHDATA_H
#define FFILEPATHDATA_H
#pragma once
#include <string>

struct FFilePathData
{
	std::wstring Path;
	std::wstring FileName;
	std::wstring FileExtension;
	bool bValid;

	FFilePathData()
	{
		bValid = false;
	}

	FFilePathData(std::wstring InPath, std::wstring InFileName, std::wstring InFileExtension)
	{
		Path = InPath;
		FileName = InFileName;
		FileExtension = InFileExtension;
		bValid = true;
	}

	explicit FFilePathData(std::wstring InPath)
	{
		Path = InPath;
		bool bExtensionComplete = false;
		for (int Index = int(Path.size()) - 1; Index >= 0; --Index)
		{
			if (Path[Index] == '.')
			{
				bExtensionComplete = true;
			}
			else if (Path[Index] == '\\')
			{
				break;
			}
			if (!bExtensionComplete)
			{
				FileExtension.insert(FileExtension.begin(), Path[Index]);
			}
			FileName.insert(FileName.begin(), Path[Index]);

		}

		bValid = true;
	}
};


#endif