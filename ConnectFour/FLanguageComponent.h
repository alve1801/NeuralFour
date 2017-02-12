#pragma once

#include <string>

using namespace std;

#ifndef FLANGUAGECOMPONENT_H
#define FLANGUAGECOMPONENT_H

enum ESplitType
{
	Cypher,
	Space,
	NoCypherBefore
};

struct FTextComponent
{
	FTextComponent(wstring InText)
	{
		Text = InText;
		SplitType = ESplitType::Cypher;
	}

	static wstring Join(vector<FTextComponent>& Components)
	{
		wstring Result;
		wstring Seperator;
		for each(auto Name in Components)
		{
			if (Result != L"")
			{
				Result += Seperator;
			}
			Result += Name.Text;
			Seperator = Name.GetSeperator();
		}
		return Result;
	}

	wstring Text;
	ESplitType SplitType;
	wstring GetSeperator()
	{
		switch (SplitType)
		{
		case ESplitType::Cypher:
			return L"-";
		case ESplitType::Space:
			return L" ";
		case ESplitType::NoCypherBefore:
			return L"";
		default:
			return L"Unkown seperator error.";
		}
	}
};

#endif