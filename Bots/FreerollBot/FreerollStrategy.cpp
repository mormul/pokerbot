#include "StdAfx.h"
#include "FreerollStrategy.h"


FreerollStrategy::FreerollStrategy(void)
{
}

FreerollStrategy::~FreerollStrategy(void)
{
}

double FreerollStrategy::ProcessQuery(const char* pQuery) const
{
	if(!pQuery) return 0;

	if(strcmp(pQuery, "dll$play") == 0)
	{
		return -1;
	}

	if(strcmp(pQuery, "dll$swag") == 0)
	{
		return 0;
	}

	if(strcmp(pQuery, "dll$alli") == 0)
	{
		return 0;
	}

	return 0;
}