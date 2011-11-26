#pragma once
#include "IOpenHoldemStrategy.h"

class FreerollStrategy :
	public IOpenHoldemStrategy
{
public:
	FreerollStrategy(void);
	virtual ~FreerollStrategy(void);
	virtual double ProcessQuery(const char* pQuery) const;
};

