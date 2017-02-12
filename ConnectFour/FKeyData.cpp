#include "stdafx.h"
#include "FKeyData.h"

void FKeyData::ProcessEvent(SDL_Keycode Key, bool bKeyDown)
{
	switch (Key)
	{
	case SDLK_LCTRL:
		KeyUpdate(EModKeys::Key_CtrlL, bKeyDown);
		break;
	case SDLK_RCTRL:
		KeyUpdate(EModKeys::Key_CtrlR, bKeyDown);
		break;
	case SDLK_LALT:
		KeyUpdate(EModKeys::Key_AltL, bKeyDown);
		break;
	case SDLK_RALT:
		KeyUpdate(EModKeys::Key_AltR, bKeyDown);
		break; 
	case SDLK_LSHIFT:
		KeyUpdate(EModKeys::Key_ShiftL, bKeyDown);
		break;
	case SDLK_RSHIFT:
		KeyUpdate(EModKeys::Key_ShiftR, bKeyDown);
		break;

	default:
		break;
	}
}

void FKeyData::KeyUpdate(UShort Mask, bool bKeyDown)
{
	if (bKeyDown)
	{
		Mods.Add(Mask);
	}
	else
	{
		Mods.Remove(Mask);
	}
}
