#pragma once
#include "IOpenHoldemProvider.h"

class OpenHoldemProviderImpl;

enum BetRound
{
	BetRoundPreflop = 1,
	BetRoundFlop = 2,
	BetRoundTurn = 3,
	BetRoundRiver = 4
};

class OpenHoldemProvider : public IOpenHoldemProvider
{
public:
	OpenHoldemProvider(void);
	virtual ~OpenHoldemProvider(void);


	double ProcessMessage(const char* pmessage, const void* param) const;

	virtual const bool GetFlagButtonState(int index) const;
	virtual const bool TestHand(const char * pHand) const;
	virtual const bool TestHand(const char * pHand0, const char * pHand1) const;
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2) const;
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3) const;
	const bool IsSeatIn(void) const;
	const double GetActiveSeatsCount(void) const;
	virtual const double GetBalance(void) const;
	virtual const double GetBigBlind(void) const;
	const BetRound GetBetRound(void) const;
	const int GetPosition(void) const;
	const int GetDealPosition(void) const;
	const bool IsList(int number) const;
	const double GetNumberOpponentsChecking(void) const;
	const double GetNumberOpponentsCalling(void) const;
	const double GetNumberOpponentsRaising(void) const;
	const double GetNumberOpponentsBetting(void) const;
	const double GetNumberOpponentsFolded(void) const;
	const double GetNeedBetsToCall(void) const;
	const double GetCurrentBet(void) const;
	const int GetMyChair(void) const;
	const int GetDealerChair(void) const;
	const bool IsPlayerFold(int chair) const;
	const double GetPlayerCurrentBet(int chair) const;
	const bool WasPlayerDidRaiseOnPreflop() const;
	const bool WasPlayerDidRaiseOnFlop() const;
	const bool IsMiddlePair() const;
	const bool IsTopPair() const;
	const bool IsOverPair() const;
	const bool IsOESD() const;
	const bool IsFlashDro() const;
	const bool IsMonster() const;
	const int GetKicker() const;
	const double GetPot() const;

private:
	OpenHoldemProvider(OpenHoldemProvider &);
	OpenHoldemProvider & operator=(OpenHoldemProvider &);

	OpenHoldemProviderImpl * _pImpl;
};

