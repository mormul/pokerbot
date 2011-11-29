#pragma once

class IOpenHoldemStrategy;

enum PreflopPosition
{
	PreflopPositionUnknown = 0,
	PreflopPositionSmallBlind = 1,
	PreflopPositionBigBlind = 2,
	PreflopPositionEarly = 3,
	PreflopPositionMiddle = 4,
	PreflopPositionLate = 5,
	PreflopPositionDealer = 6
};

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
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4) const = 0;

	virtual const double GetBigBlind(void) const = 0;

	virtual const double GetBalance(void) const = 0;
	virtual const double GetOpponentBalance(int chair) const = 0;
	virtual const double GetBet(void) const = 0;
	virtual const double GetOpponentBet(int chair) const = 0;

	virtual const PreflopPosition GetPreflopPosition(void) const = 0;

protected:
	const IOpenHoldemStrategy * _pStrategy;
};
