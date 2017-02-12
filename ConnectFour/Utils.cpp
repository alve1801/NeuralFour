#include "stdafx.h"
#include "Utils.h"
#include "AApp.h"
#include "DrawHelpers.h"



#if LIB_OPENCV
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#endif

using namespace std;

namespace Utils
{
	void Capitalize(wstring & StringToCapitalize) {
		for (int Index = 0; Index < StringToCapitalize.size(); Index++) {
			wchar_t CurrentSChar = StringToCapitalize[Index];
			if (isalpha(CurrentSChar))
			{
				StringToCapitalize[Index] = toupper(StringToCapitalize[Index]);
				break;
			}
		}
	}

	string IntToRoman(int Value)
	{
		int Buffer = abs(Value);
		struct FRomanData { unsigned int Value; char const* Numeral; };
		const static FRomanData Romandata[] =
		{
			{ 1000, "M" },{ 900, "CM" },
			{ 500, "D" },{ 400, "CD" },
			{ 100, "C" },{ 90, "XC" },
			{ 50, "L" },{ 40, "XL" },
			{ 10, "X" },{ 9, "IX" },
			{ 5, "V" },{ 4, "IV" },
			{ 1, "I" },
			{ 0, nullptr }
		};

		std::string Result;
		if (Value < 0)
		{
			Result += '-';
		}
		for (const FRomanData* Current = Romandata; Current->Value > 0; ++Current)
		{
			while (Buffer >= short(Current->Value))
			{
				Result += Current->Numeral;
				Buffer -= Current->Value;
			}
		}
		
		return Result;
	}

	string XORCipher(string str, string key)
	{
		string Result;

		for (int Index = 1; Index < str.size() + 1; Index++)
		{
			Result += str[Index - 1] ^ key[(Index - 1) % key.size()] ^ 1;
		}

		return Result;
	}

	wstring Superscript(wstring Input)
	{
		return L"^[" + Input + L"]";
	}

	void CropStartAndEnd(int& Start, int& End, int Length)
	{
		if (End < 0)
		{
			End = Length + End + 1;
		}
		else if (End > Length)
		{
			End = Length;
		}
		if (Start < 0)
		{
			Start = Length + Start;
		}
		if (End < Start)
		{
			std::swap(End, Start);
		}
	}

	wstring StringToWString(const string s)
	{
		return wstring(s.begin(), s.end());
	}

	string WStringToString(const wstring s, const char Tofu)
	{
		string Result;
		for each (wchar_t C in s)
		{
			if (C >= 256)
			{
				Result += Tofu;
				continue;
			}
			Result += char(C);
		}
		return Result;
	}



	/*@brief
	Returns the greek prefix for the length of an alcane, example: meth, eth, prop, but...
	*/
	void WstringToWSTR(wstring NewString, wchar_t Wstr[], size_t Offset)
	{
		for (size_t Index = Offset; Index < Offset + NewString.size(); Index++)
		{
			Wstr[Index] = NewString[Index];
		}
	}

	vector<string> PathSplit(string Path)
	{
		vector<string> Result(1, "");
		string* CurrentString = &Result[0];
		string PatrtialPath = "";

		for (int Index = 0; Index < Path.size(); Index++)
		{
			if (Path[Index] == '\\')
			{
				Result.push_back("");
				CurrentString = &Result[Result.size() - 1];
			}
			else
			{
				CurrentString->push_back(Path[Index]);
			}
		}
		return Result;
	}

	string PathJoin(vector<string> SplitPath, int Start, int End)
	{
		CropStartAndEnd(Start, End, int(SplitPath.size()));
		string Result = "";
		for (int Index = Start; Index < End; ++Index)
		{
			if (Index != Start)
			{
				Result += '\\';
			}
			Result.append(SplitPath[Index]);
		}
		return Result;
	}

	string PathSegment(string Path, int Start, int End)
	{
		return PathJoin(PathSplit(Path), Start, End);
	}


	vector<wstring> PathSplit(wstring Path)
	{
		vector<wstring> Result(1, L"");
		wstring* CurrentString = &Result[0];
		wstring PatrtialPath = L"";

		for (int Index = 0; Index < Path.size(); Index++)
		{
			if (Path[Index] == '\\')
			{
				Result.push_back(L"");
				CurrentString = &Result[Result.size() - 1];
			}
			else
			{
				CurrentString->push_back(Path[Index]);
			}
		}
		return Result;
	}

	wstring PathJoin(vector<wstring> SplitPath, int Start, int End)
	{
		CropStartAndEnd(Start, End, int(SplitPath.size()));
		wstring Result = L"";
		for (int Index = Start; Index < End; ++Index)
		{
			if (Index != Start)
			{
				Result += '\\';
			}
			Result.append(SplitPath[Index]);
		}
		return Result;
	}

	wstring PathSegment(wstring Path, int Start, int End)
	{
		return PathJoin(PathSplit(Path), Start, End);
	}

	wstring LineSplit(wstring InString, size_t MaxSCharacters, const char* PreferredSChars, size_t* LineNumbers)
	{

		size_t Length = InString.length();

		if (Length <= MaxSCharacters)
		{
			return InString;
		}

		wstring NewString = InString;
		size_t Line = 0;
		size_t SplitIndex = 0;
		size_t Start = 0;
		
		size_t PreferredSCharCount = strlen(PreferredSChars);
		while (true)
		{

			for (size_t Index = Start; Index < MaxSCharacters + Start; Index++)
			{
				if (Index >= Length)
				{
					if (LineNumbers)
					{
						*LineNumbers = Line+1;
					}
					return NewString;
				}

				for (size_t SCharIndex = 0; SCharIndex < PreferredSCharCount; SCharIndex++)
				{
					if (NewString[Index] == PreferredSChars[SCharIndex])
					{
						SplitIndex = Index;
					}
				}
			}
			if (SplitIndex == Start)
			{
				SplitIndex = Start + MaxSCharacters;
			}
			Start += MaxSCharacters + 1;
			NewString.insert(NewString.begin() + SplitIndex + 1, '\n');
			Line++;
		}
	}
}
#if LIB_OPENCV
cv::Mat Utils::CV::SurfaceToMat(SDL_Surface* Surface)
{
	cv::Mat Result = cv::Mat(Surface->h, Surface->w, CV_8UC4);
	int bpp = Surface->format->BytesPerPixel;

	for (int x = 0; x < Surface->w; x++)
	{
		for (int y = 0; y < Surface->h; y++)
		{
			Uint32 pixelColor = *(Uint32*) ((Uint8 *) Surface->pixels + y * Surface->pitch + x * bpp);

			FColor Color;
			SDL_GetRGBA(pixelColor, Surface->format, &Color.R, &Color.G, &Color.B, &Color.A);

			Result.at<cv::Vec4b>(y, x) = Color.ToScalar();
		}
	}

	return Result;
}

void Utils::CV::ValueToAlpha(cv::Mat& Image, bool bInverted)
{


	for (int x = 0; x < Image.cols; x++)
	{
		for (int y = 0; y < Image.rows; y++)
		{
			cv::Vec4b Pixel = Image.at<cv::Vec4b>(y, x);
			UChar Value = max(Pixel[0], max(Pixel[1], Pixel[2]));

			Image.at<cv::Vec4b>(y, x)[3] = bInverted ? Value : (255 - Value);
		}
	}


}

void Utils::CV::WriteEncodedImageToFile(std::ofstream& Stream, vector<UChar>& Data)
{
	Stream.write(reinterpret_cast<const char*>(Data.data()), Data.size());
}

void Utils::CV::SaveAsJpg(wstring Path, cv::Mat & Image, UChar Quality)
{
	std::vector<uchar> Buffer;

	std::vector<int> Parameters;
	Parameters.push_back(CV_IMWRITE_JPEG_QUALITY);
	Parameters.push_back(Quality);

	cv::imencode(".jpg", Image, Buffer, Parameters);

	std::ofstream File(Path, std::ofstream::binary);
	WriteEncodedImageToFile(File, Buffer);
	File.close();

}

void Utils::CV::SaveAsPng(wstring Path, cv::Mat& Image, UChar Compression)
{
	std::vector<uchar> Buffer(0);

	std::vector<int> Parameters;
	Parameters.push_back(CV_IMWRITE_PNG_COMPRESSION);
	Parameters.push_back(Compression);

	cv::imencode(".png", Image, Buffer, Parameters);

	std::ofstream File(Path, std::ofstream::binary);
	WriteEncodedImageToFile(File, Buffer);
	File.close();
}

void Utils::CV::SaveAsBmp(wstring Path, cv::Mat& Image)
{
	std::vector<uchar> Buffer(0);

	std::vector<int> Parameters;

	cv::imencode(".bmp", Image, Buffer, Parameters);

	std::ofstream File(Path, std::ofstream::binary);
	WriteEncodedImageToFile(File, Buffer);
	File.close();
}
#endif