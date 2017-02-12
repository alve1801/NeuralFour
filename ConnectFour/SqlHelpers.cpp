#include "stdafx.h"
#include "SqlHelpers.h"

bool SqlHelpers::OpenDataBase(wstring Path, ADataBase** DataBase, int* ErrorCode)
{
	int Result = sqlite3_open16(Path.data(), DataBase);
	if (Result)
	{
		if (ErrorCode)
		{
			*ErrorCode = Result;
		}
		return false;
	}
	return true;
}

void SqlHelpers::CloseDataBase(ADataBase* DataBase)
{
	sqlite3_close(DataBase);
}

void SqlHelpers::ExecuteCommad(ADataBase* DataBase, string Command, FCallback Callback, void* CallbackData)
{
	char* ErrorMessage = nullptr;
	int Result = sqlite3_exec(DataBase, Command.c_str(), Callback, CallbackData, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", ErrorMessage);
		sqlite3_free(ErrorMessage);
	}
}

void SqlHelpers::ExecuteCommad(ADataBase* DataBase, string Command)
{
	char* ErrorMessage = nullptr;

	int Result = sqlite3_exec(DataBase, Command.c_str(), nullptr, 0, &ErrorMessage);
	if (Result != SQLITE_OK)
	{
		fprintf(stderr, "SQL error: %s\n", ErrorMessage);
		sqlite3_free(ErrorMessage);
	}
}

void SqlHelpers::BeginTransaction(ADataBase* DataBase)
{
	ExecuteCommad(DataBase, "BEGIN");
}

void SqlHelpers::CommitTransaction(ADataBase* DataBase)
{
	ExecuteCommad(DataBase, "COMMIT");
}
