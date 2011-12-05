#pragma once
#include "IOpenHoldemProvider.h"

class OpenHoldemProviderImpl;

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
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4) const;
	virtual const bool TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4, const char * pHand5) const;

	virtual const double GetBigBlind(void) const;
	virtual const double GetPot(void) const;
	virtual const BetRound GetBetRound(void) const;

	virtual const double GetBalance(void) const;
	virtual const double GetOpponentBalance(int chair) const;
	virtual const double GetBet(void) const;
	virtual const double GetOpponentBet(int chair) const;

	virtual const PreflopPosition GetPreflopPosition(void) const;

	virtual const double GetNumberOpponentsChecking(void) const;
	virtual const double GetNumberOpponentsCalling(void) const;
	virtual const double GetNumberOpponentsRaising(void) const;
	virtual const double GetNumberOpponentsBetting(void) const;
	virtual const double GetNumberOpponentsFolded(void) const;

	virtual const bool IsMiddlePair(void) const;
	virtual const bool IsTopPair(void) const;
	virtual const bool IsOverPair(void) const;
	virtual const bool IsOESD(void) const;
	virtual const bool IsFlashDro(void) const;
	virtual const bool IsTwoPair(void) const;
	virtual const bool IsMonster(void) const;

	virtual const int GetRankHiPocketCard(void) const;
	virtual const int GetRankLoPocketCard(void) const;
	virtual const int GetRankHiCommonCard(void) const;

	//const bool IsSeatIn(void) const;
	//const double GetActiveSeatsCount(void) const;
	//const int GetPosition(void) const;
	//const int GetDealPosition(void) const;
	//const bool IsList(int number) const;
	//const double GetNeedBetsToCall(void) const;
	//const double GetCurrentBet(void) const;
	//const int GetMyChair(void) const;
	//const int GetDealerChair(void) const;
	//const bool IsPlayerFold(int chair) const;
	//const double GetPlayerCurrentBet(int chair) const;
	//const bool WasPlayerDidRaiseOnPreflop() const;
	//const bool WasPlayerDidRaiseOnFlop() const;
	//const int GetKicker() const;

private:
	OpenHoldemProvider(OpenHoldemProvider &);
	OpenHoldemProvider & operator=(OpenHoldemProvider &);

	OpenHoldemProviderImpl * _pImpl;
};

