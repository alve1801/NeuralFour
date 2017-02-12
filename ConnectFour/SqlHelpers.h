#pragma once
#include <sqlite3.h>
#include "Definitions.h"

#define SQLHELPERS_CALLBACK_ARGUMENTS void* Data, int ArgumentsCount, char** Arguments, char** Columns
namespace SqlHelpers
{
	typedef sqlite3 ADataBase;
	typedef int (*FCallback)(void*, int, char**, char**);

	bool OpenDataBase(wstring Path, ADataBase** DataBase, int* ErrorCode = nullptr);
	void CloseDataBase(ADataBase* DataBase);


	void ExecuteCommad(ADataBase* DataBase, string Command, FCallback Callback, void* CallbackData = nullptr);
	void ExecuteCommad(ADataBase* DataBase, string Command);

	void BeginTransaction(ADataBase* DataBase);
	void CommitTransaction(ADataBase* DataBase);
	
}

