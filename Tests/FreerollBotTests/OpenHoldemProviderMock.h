#pragma once
#include "IOpenHoldemProvider.h"

class OpenHoldemProviderMock : public IOpenHoldemProvider
{
public:
	OpenHoldemProviderMock(void);
	virtual ~OpenHoldemProviderMock(void);

	MOCK_CONST_METHOD1(GetFlagButtonState, const bool(int index));
	MOCK_CONST_METHOD1(TestHand, const bool(const char * pHand));
	MOCK_CONST_METHOD2(TestHand, const bool(const char * pHand0, const char * pHand1));
	MOCK_CONST_METHOD3(TestHand, const bool(const char * pHand0, const char * pHand1, const char * pHand2));
	MOCK_CONST_METHOD4(TestHand, const bool(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3));
	MOCK_CONST_METHOD0(GetBalance, const double(void));
	MOCK_CONST_METHOD0(GetBigBlind, const double(void));
};

