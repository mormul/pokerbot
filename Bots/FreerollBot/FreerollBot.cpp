// FreerollBot.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FreerollBot.h"
#include "OpenHoldemProvider.h"

OpenHoldemProvider theProvider;

FREEROLLBOT_API double process_message(const char* pmessage, const void* param)
{
	return theProvider.ProcessMessage(pmessage, param);
}
