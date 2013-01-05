// FreerollBot.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FreerollBot.h"
#include "FileSystemLogger.h"
#include "OpenHoldemProvider.h"
#include "FreerollStrategy.h"

std::auto_ptr<ILogger> pLogger(NULL);
std::auto_ptr<IOpenHoldemStrategy> pStrategy(NULL);
std::auto_ptr<OpenHoldemProvider> pProvider(NULL);

FREEROLLBOT_API double process_message(const char* pmessage, const void* param)
{
	if (!pStrategy.get() && !pProvider.get())
	{
		pLogger.reset(new FileSystemLogger());
		pStrategy.reset(new FreerollStrategy());
		pProvider.reset(new OpenHoldemProvider());
		pProvider->SetLogger(pLogger.get());
		pStrategy->SetProvider(pProvider.get());
		pProvider->SetStrategy(pStrategy.get());
	}

	return pProvider->ProcessMessage(pmessage, param);
}
