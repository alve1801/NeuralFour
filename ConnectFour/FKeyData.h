#pragma once

#ifndef FKEYDATA_H
#define FKEYDATA_H



#include "Utils.h"

enum EModKeys
{
	Key_CtrlL = FLAGINDEX(1),
	Key_CtrlR = FLAGINDEX(2),
	Key_AltL = FLAGINDEX(3),
	Key_AltR = FLAGINDEX(4),
	Key_ShiftL = FLAGINDEX(5),
	Key_ShiftR = FLAGINDEX(6),
};

struct FKeyData
{
	
	FFlag<UShort> Mods;

	void ProcessEvent(SDL_Keycode Key, bool bKeyDown);

private:
	void KeyUpdate(UShort Mask, bool bKeyDown);
};


#endif