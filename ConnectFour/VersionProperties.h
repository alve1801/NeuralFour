#pragma once



#ifndef VERSIONPROPERTIES_H
#define VERSIONPROPERTIES_H

#include "Definitions.h"

#if defined(_WIN64)
#define VERSION_BITS 64
#elif defined(_WIN32)
#define VERSION_BITS 32
#endif



#define APP_NAME "KantFour"
#define APP_NAME_W L"KantFour"



#define DEPLOY_VERSION 0

#define INITIALIZER_VERSION_ACTIVE 0
#define INITIALIZER_TIME 240


#define MASTER_VERSION_KEY 1
#define CONFIG_VERSION_KEY (1 + MASTER_VERSION_KEY)


#define LIB_OPENCV 1



#define MAX_FPS 60


#define DRAW_SAFE 0

#define SMARTMUTEX_DEBUG 1

#define GARBAGE_CHECK 0

#define BRESENHAM_ENCRYPT "Jack Elton Bresenham: Bresenham retired from 27 years of service at IBM as a Senior Technical Staff Member in 1987. He taught for 16 years at Winthrop University and has nine patents.[1] He has three children: Janet, Linda, and David. Bresenham's line algorithm, developed in 1962, is his most well-known innovation. It determines which points on a 2-dimensional raster should be plotted in order to form a straight line between two given points, and is commonly used to draw lines on a computer screen. It is one of the earliest algorithms discovered in the field of computer graphics. The Midpoint circle algorithm shares some similarities to his line algorithm and is known as Bresenham's circle algorithm."






#ifdef _DEBUG
#undef DEPLOY_VERSION
#define DEPLOY_VERSION 0
#endif



#define INITIALIZER_ACTIVATED (DEPLOY_VERSION || INITIALIZER_VERSION_ACTIVE)
#define VERIFY_INSTALLATION_ACTIVATED DEPLOY_VERSION



#undef min
#undef max

constexpr double MinimalFrameTime = TIME_S_TO_MS(1.0 / MAX_FPS);

#endif