#pragma once

#ifndef UTILS_H
#define UTILS_H


#include "boost/thread.hpp"
#include <boost/rational.hpp>
#include <fstream>
#include <algorithm>
#include "VersionProperties.h"

#if LIB_OPENCV
#include "opencv2/core.hpp"
#endif

#include "Definitions.h"

#include "FVector.h"
#include "FRectangle.h"
#include "FColor.h"
#include "FFlag.h"
#include "FConfigFile.h"
#include "FFilePathData.h"
//
//struct FPoint;
//typedef FPoint FSize;


namespace boost
{
	template<typename T>
	constexpr T floor(rational<T> const& r)
	{
		return static_cast<T>(r.numerator() / r.denominator());
	}
};


namespace Utils
{
	string XORCipher(string str, string key);

	wstring Superscript(wstring Input);

#if LIB_OPENCV
	namespace CV
	{
		cv::Mat SurfaceToMat(SDL_Surface* Surface);
		void ValueToAlpha(cv::Mat& Image, bool bInverted = false);

		void WriteEncodedImageToFile(std::ofstream& Stream, vector<UChar>& Data);

		/**
		 * \brief 
		 * \param Quality 0-100, 0 is lowest
		 */
		void SaveAsJpg(wstring Path, cv::Mat& Image, UChar Quality = 90);


		/**
		* \brief
		* \param Compression 0-9, 0 is lowest
		*/
		void SaveAsPng(wstring Path, cv::Mat& Image, UChar Compression = 6);


		void SaveAsBmp(wstring Path, cv::Mat& Image);

	}
#endif
	void CropStartAndEnd(int& Start, int& End, int Length);


	wstring StringToWString(const string s);

	string WStringToString(const wstring s, char Tofu = '§');

	void WstringToWSTR(wstring NewString, wchar_t Wstr[], size_t Offset = 0);

	vector<string> PathSplit(string Path);
	string PathJoin(vector<string> SplitPath, int Start = 0, int End = -1);
	string PathSegment(string Path, int Start = 0, int End = -1);

	vector<wstring> PathSplit(wstring Path);
	wstring PathJoin(vector<wstring> SplitPath, int Start = 0, int End = -1);
	wstring PathSegment(wstring Path, int Start = 0, int End = -1);

	wstring GetRoot(size_t Length);
	wstring GetPrevalence(size_t Count);
	wstring GetComplexPrevalence(size_t Count);
	void Capitalize(wstring& String1);

	string IntToRoman(int Value);


	template<typename T1, typename T2>
	inline T2 Lerp(T1 A, T1 B, T2 Alpha)
	{
		return T2(A) + Alpha * T2(B - A);
	}



	template<typename T0, typename T1>
	bool Between(T0 A, T1 Min, T1 Max)
	{
		return A >= Min && A <= Max;
	}

	template<typename T>
	T PositiveModulo(T i, T n)
	{
		return (n + (i % n)) % n;
	}

	template<typename T>
	T Square(T n)
	{
		return n*n;
	}
	
	template<typename T0, typename T1>
	T0 Clamp(T0 Val, T1 Min, T1 Max)
	{
		return std::max(T0(Min), std::min(Val, T0(Max)));
	}

	template<typename T0>
	T0 Difference(T0 Val0, T0 Val1)
	{
		return abs(abs(Val0) - abs(Val1));
	}

	template<typename T0, typename T1>
	T0 Percentage(T0 Val, T1 Min, T1 Max)
	{
		return T0((Val - Min) / (Max - Min));
	}

	template<typename T0>
	T0 RandomScalar()
	{
		return static_cast <T0>(rand()) / static_cast<T0>(RAND_MAX);
	}

	template<typename T>
	void VectorAppend(vector<T>& VectorA, vector<T>& VectorB, const bool bReverse = false)
	{
		if (bReverse)
		{
			VectorA.insert(VectorA.end(), VectorB.rbegin(), VectorB.rend());
			return;
		}
		VectorA.insert(VectorA.end(), VectorB.begin(), VectorB.end());
	}

	template<typename T>
	void VectorContains(vector<T>& Vector, T& Element)
	{
		return Vector.find(Element) != Vector.end();
	}

	inline string ToHexadecimal(int Integer)
	{
		char Buffer[16];
		return _itoa(Integer, Buffer, 16);
	}

	inline string ToHexadecimal(long Integer)
	{
		char Buffer[32];
		return _ltoa(Integer, Buffer, 16);
	}

	inline string ReadAllBinary(const string& path)
	{
		ostringstream buf;
		ifstream input(path.c_str(), ios::binary);
		buf << input.rdbuf();
		return buf.str();
	}

	inline string ReadAll(const string& path)
	{
		ostringstream buf;
		ifstream input(path.c_str());
		buf << input.rdbuf();
		return buf.str();
	}

	wstring LineSplit(wstring InString, size_t MaxSCharacters, const char* PreferredSChars, size_t* LineNumbers = nullptr);

};



#endif
