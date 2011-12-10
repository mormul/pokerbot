#pragma once
#include "IOpenHoldemProvider.h"

class OpenHoldemProviderMock : public IOpenHoldemProvider
{
public:
	OpenHoldemProviderMock(void) { }
	~OpenHoldemProviderMock(void) { }

	MOCK_CONST_METHOD1(WriteLog, void (const char * pMessage));

	MOCK_CONST_METHOD1(GetFlagButtonState, const bool(int index));

	MOCK_CONST_METHOD1(TestHand, const bool (const char * pHand));
	MOCK_CONST_METHOD2(TestHand, const bool (const char * pHand0, const char * pHand1));
	MOCK_CONST_METHOD3(TestHand, const bool (const char * pHand0, const char * pHand1, const char * pHand2));
	MOCK_CONST_METHOD4(TestHand, const bool (const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3));
	MOCK_CONST_METHOD5(TestHand, const bool (const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4));
	MOCK_CONST_METHOD6(TestHand, const bool (const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4, const char * pHand5));

	MOCK_CONST_METHOD0(GetBigBlind, const double (void));
	MOCK_CONST_METHOD0(GetPot, const double (void));
	MOCK_CONST_METHOD0(GetBetRound, const BetRound (void));

	MOCK_CONST_METHOD0(GetBalance, const double (void));
	MOCK_CONST_METHOD1(GetOpponentBalance, const double (int chair));
	MOCK_CONST_METHOD0(GetBet, const double (void));
	MOCK_CONST_METHOD1(GetOpponentBet, const double (int chair));

	MOCK_CONST_METHOD0(GetPreflopPosition, const PreflopPosition (void));

	MOCK_CONST_METHOD0(GetNumberOpponentsChecking, const double (void));
	MOCK_CONST_METHOD0(GetNumberOpponentsCalling, const double (void));
	MOCK_CONST_METHOD0(GetNumberOpponentsRaising, const double (void));
	MOCK_CONST_METHOD0(GetNumberOpponentsBetting, const double (void));
	MOCK_CONST_METHOD0(GetNumberOpponentsFolded, const double (void));

	MOCK_CONST_METHOD0(IsMiddlePair, const bool (void));
	MOCK_CONST_METHOD0(IsTopPair, const bool (void));
	MOCK_CONST_METHOD0(IsOverPair, const bool (void));
	MOCK_CONST_METHOD0(IsOESD, const bool (void));
	MOCK_CONST_METHOD0(IsFlashDro, const bool (void));
	MOCK_CONST_METHOD0(IsTwoPair, const bool (void));
	MOCK_CONST_METHOD0(IsMonster, const bool (void));

	MOCK_CONST_METHOD0(GetRankHiPocketCard, const int (void));
	MOCK_CONST_METHOD0(GetRankLoPocketCard, const int (void));
	MOCK_CONST_METHOD0(GetRankHiCommonCard, const int (void));
};

