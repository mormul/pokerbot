#pragma once
#include "FreerollStrategy.h"
#include "OpenHoldemProviderMock.h"
using ::testing::NiceMock;

class FreerollStrategyTests : public ::testing::Test
{
protected:
	FreerollStrategyTests(void);
	virtual ~FreerollStrategyTests(void);

	FreerollStrategy					_strategy;
	NiceMock<OpenHoldemProviderMock>	_provider;
};

