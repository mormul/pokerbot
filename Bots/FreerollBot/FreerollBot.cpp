// FreerollBot.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FreerollBot.h"

FREEROLLBOT_API double process_message(const char* pmessage, const void* param)
{
	if(pmessage == NULL)
	{
		return 0;
	}

	if(param == NULL)
	{
		return 0;
	}

	if(strcmp(pmessage, "state") == 0)
	{
		//theProvider.ProcessState(reinterpret_cast<holdem_state*>(const_cast<void*>(param)));
		return 0;
	}

	if(strcmp(pmessage, "query") == 0)
	{ 
		//return process_query(static_cast<const char*>(param));
		return 0;
	}

	if(strcmp(pmessage, "pfgws") == 0)
	{
		//theProvider.SetCallBack(reinterpret_cast<p_getsym_t>(const_cast<void*>(param)));
		return 0;
	}

	return 0;
}
