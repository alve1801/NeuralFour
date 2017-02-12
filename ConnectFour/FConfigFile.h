#pragma once

#ifndef FCONFIGFILE_H
#define FCONFIGFILE_H

#include "Definitions.h"
#include "FRectangle.h"
#include "FVector.h"
#include <map>
#include <fstream>



struct FConfigFile
{
	wstring FilePath;
	std::map<std::string, std::string> Entries;
	UInt Version;

	FConfigFile() : Version(0)
	{
	}

	explicit FConfigFile(wstring InPath) : FConfigFile()
	{
		FilePath = InPath;
	}

	~FConfigFile()
	{
		Entries.clear();
	}

	bool Exists() const
	{
		ifstream f(FilePath);
		return f.good();
	}

	bool CreateConfigFile() const
	{
		RETURN_IF(Exists()) false;
		std::ofstream InputStream(FilePath);
		return InputStream.is_open();
	}

	UInt GetVersion() const
	{
		return Version;
	}
	void SetVersion(UInt NewVersion)
	{
		Version = NewVersion;
	}


	bool CreateNewEntry(string Key, string StringValue)
	{
		RETURN_IF(STD_CONTAINS(Entries, Key)) false;
		Entries.insert(std::pair<string, string>(Key, StringValue));
		return true;
	}

	bool CreateNewEntry(string Key, int Value)
	{
		RETURN_IF(STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value);
		Entries.insert(std::pair<string, string>(Key, StringValue));
		return true;
	}

	bool CreateNewEntry(string Key, float Value)
	{
		RETURN_IF(STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value);
		Entries.insert(std::pair<string, string>(Key, StringValue));
		return true;
	}

	bool CreateNewEntry(string Key, FPoint Value)
	{
		RETURN_IF(STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value.x) + '|' + to_string(Value.y);
		Entries.insert(std::pair<string, string>(Key, StringValue));
		return true;
	}

	bool CreateNewEntry(string Key, FRectangle Value)
	{
		RETURN_IF(STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value.TL.x) + '|' + to_string(Value.TL.y) + '|' + to_string(Value.BR.x) + '|' + to_string(Value.BR.y);
		Entries.insert(std::pair<string, string>(Key, StringValue));
		return true;
	}



	bool SetEntry(string Key, string StringValue)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;
		Entries[Key] = StringValue;
		return true;
	}

	bool SetEntry(string Key, int Value)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value);
		Entries[Key] = StringValue;
		return true;
	}

	bool SetEntry(string Key, float Value)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value);
		Entries[Key] = StringValue;
		return true;
	}

	bool SetEntry(string Key, FPoint Value)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value.x) + '|' + to_string(Value.y);
		Entries[Key] = StringValue;
		return true;
	}

	bool SetEntry(string Key, FRectangle Value)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;
		string StringValue = to_string(Value.TL.x) + '|' + to_string(Value.TL.y) + '|' + to_string(Value.BR.x) + '|' + to_string(Value.BR.y);
		Entries[Key] = StringValue;
		return true;
	}



	string GetEntryAsString(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) "";
		return Entries[Key];
	}

	int GetEntryAsInt(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) 0;
		return stoi(Entries[Key]);
	}

	float GetEntryAsFloat(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) 0;
		return stof(Entries[Key]);
	}

	FPoint GetEntryAsPoint(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) FPoint();

		string ValueString = Entries[Key];
		FPoint Result;
		string Buffer;
		for (char Char : ValueString)
		{
			if (Char != ' ' && Char != '=')
			{
				Buffer += Char;
			}
			else
			{
				Result.x = stoi(Buffer);
				Buffer = "";
			}
		}
		Result.y = stoi(Buffer);

		return Result;
	}

	FRectangle GetEntryAsRectangle(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) FRectangle();

		string ValueString = Entries[Key];
		FRectangle Result;
		string Buffer;
		UChar Index = 0;
		for (char Char : ValueString)
		{
			if (Char != '|')
			{
				Buffer += Char;
			}
			else
			{
				switch (Index)
				{
				case 0:
					Result.TL.x = stoi(Buffer);
					break;
				case 1:
					Result.TL.y = stoi(Buffer);
					break;
				case 2:
					Result.BR.x = stoi(Buffer);
					break;
				default:
					break;
				}
				Buffer = "";
				Index++;
			}
		}
		Result.BR.y = stoi(Buffer);
		return Result;
	}

	bool GetEntryAsBool(string Key)
	{
		RETURN_IF(!STD_CONTAINS(Entries, Key)) false;

		string Value = Entries[Key];
		if (Value == "true" || Value == "1")
		{
			return true;
		}
		return false;
	}


	bool LoadEntries()
	{
		string Line;
		std::ifstream Stream(FilePath, std::ifstream::in);
		RETURN_IF(!Stream.is_open()) false;


		while (getline(Stream, Line))
		{
			string Key, Value;
			bool bKeyComplete = false;
			for (char Char : Line)
			{
				if (Char != ' ' && Char != '=')
				{
					if (bKeyComplete)
					{
						Value += Char;
					}
					else
					{
						Key += Char;
					}
				}
				else
				{
					bKeyComplete = true;
				}
			}
			if (Key != "")
			{
				Entries.insert(std::pair<string, string>(Key, Value));
			}
		}
		Version = stoi(Entries["CONFIG_FILE_VERSION"]);
		Entries.erase("CONFIG_FILE_VERSION");

		Stream.close();
		return true;
	}

	bool WriteEntries()
	{
		string Line;
		std::ofstream Stream(FilePath, std::ofstream::out | std::ofstream::trunc);
		RETURN_IF(!Stream.is_open()) false;

		Line = string("CONFIG_FILE_VERSION") + " = " + to_string(Version) + '\n';
		Stream << Line;


		for (auto Entry : Entries)
		{
			Line = Entry.first + " = " + Entry.second + '\n';
			Stream << Line;
		}
		Stream.close();
		return true;
	}
};

#endif