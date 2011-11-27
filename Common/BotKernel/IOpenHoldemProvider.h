#pragma once

class IOpenHoldemStrategy;

class IOpenHoldemProvider
{
public:
	IOpenHoldemProvider(void) : _pStrategy(NULL) { }
	virtual ~IOpenHoldemProvider(void) = 0 { }
	void SetStrategy(const IOpenHoldemStrategy * pStrategy) { _pStrategy = pStrategy; }

	virtual const bool GetFlagButtonState(int index) const = 0;
	virtual const bool TestHand(const char * pHand) const = 0;
	virtual const bool TestHand(const char * pHand0, const char * pHand1) const = 0;
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2) const = 0;
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3) const = 0;
	virtual const double GetBalance(void) const = 0;
	virtual const double GetBigBlind(void) const = 0;

protected:
	const IOpenHoldemStrategy * _pStrategy;
};
