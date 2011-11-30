#pragma once

class IOpenHoldemStrategy;

enum BetRound
{
	BetRoundUnknown = 0,
	BetRoundPreflop = 1,
	BetRoundFlop = 2,
	BetRoundTurn = 3,
	BetRoundRiver = 4
};

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
	virtual const BetRound GetBetRound(void) const = 0;

	virtual const double GetBalance(void) const = 0;
	virtual const double GetOpponentBalance(int chair) const = 0;
	virtual const double GetBet(void) const = 0;
	virtual const double GetOpponentBet(int chair) const = 0;

	virtual const PreflopPosition GetPreflopPosition(void) const = 0;

	virtual const double GetNumberOpponentsChecking(void) const = 0;
	virtual const double GetNumberOpponentsCalling(void) const = 0;
	virtual const double GetNumberOpponentsRaising(void) const = 0;
	virtual const double GetNumberOpponentsBetting(void) const = 0;
	virtual const double GetNumberOpponentsFolded(void) const = 0;

	virtual const bool IsMiddlePair() const = 0;
	virtual const bool IsTopPair() const = 0;
	virtual const bool IsOverPair() const = 0;
	virtual const bool IsOESD() const = 0;
	virtual const bool IsFlashDro() const = 0;
	virtual const bool IsMonster() const = 0;

protected:
	const IOpenHoldemStrategy * _pStrategy;
};
