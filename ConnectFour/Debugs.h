#pragma once

#ifndef DEBUGS_H
#define DEBUGS_H

#include "stdafx.h"
#include "VersionProperties.h"
#if LIB_OPENCV
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#endif

#include "Globals.h"

using namespace std;

#define TESTEXCEPTIONS

//#define TESTDELAYS

#define DEFAULTDEBUGS

//#define MESH_DEBUG_ACTIVE


#ifdef TESTEXCEPTIONS

#define CHECK(Condition, Exception) if (!(Condition)) throw Exception

#else

#define CHECK(Condition, Exception) /**/

#endif


#ifdef MESH_DEBUG_ACTIVE

#define MESH_DEBUG(Code) Code

#else

#define MESH_DEBUG(Code) /**/

#endif

#ifdef TESTDELAYS

#define DELAY(Time) SDL_Delay(Time)

#else

#define DELAY(Time) /**/

#endif




#ifdef DEFAULTDEBUGS

#define DEBUG(Line) Line

#else

#define DEBUG(Line) /**/

#endif

MESH_DEBUG(
class ADebugAlkaneMesh
{
public:
	ADebugAlkaneMesh();
	~ADebugAlkaneMesh();

	void Render();
	void Load();

	UI::AWindow* Window;

	AlkaneMesh::AMesh Mesh;

	AlkaneMesh::FNodes Loop;

	TTF_Font* DefaultFont;


};
)

class XGarbageDetection : public runtime_error
{
public:
	XGarbageDetection::XGarbageDetection() : runtime_error("XGarbageDetection") {}
	XGarbageDetection::XGarbageDetection(string Text) : runtime_error(Text) {}
};

class XNullPointer : public runtime_error
{
public:
	XNullPointer::XNullPointer() : runtime_error("XNullPointer") {}
	XNullPointer::XNullPointer(string Text) : runtime_error(Text) {}
};

class XCyclo : public runtime_error
{
public:
	XCyclo::XCyclo() : runtime_error("XCyclo") {}
	XCyclo::XCyclo(string Text) : runtime_error(Text) {}
};

class XDefault : public runtime_error
{
public:
	XDefault::XDefault() : runtime_error("XDefault") {}
	XDefault::XDefault(string Text) : runtime_error(Text) {}
};

#endif