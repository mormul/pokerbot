#include "StdAfx.h"
#include "FreerollStrategy.h"
#include "IOpenHoldemProvider.h"

FreerollStrategy::FreerollStrategy(void)
{
}

FreerollStrategy::~FreerollStrategy(void)
{
}

double FreerollStrategy::GetSwag(void) const
{
	if (GetAllin())
		return _pProvider->GetBalance();

	switch(TourneyStage)
	{
	case TourneyStageEarly:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (!IsRaisedPot()
						&& _pProvider->GetPreflopPosition() != PreflopPositionEarly
						&& _pProvider->TestHand("66", "77", "88", "99", "TT"))
						return _pProvider->GetBigBlind() * 4;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
				}
				break;
			}
		}
		break;
	case TourneyStageLate:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (!IsRaisedPot()
						&& (_pProvider->TestHand("AA", "KK", "QQ", "JJ", "TT", "AK")
							|| (_pProvider->GetPreflopPosition() != PreflopPositionEarly
								&& (_pProvider->TestHand("99", "AQ")
									|| _pProvider->TestHand("AJ", "KQ")))))
						return _pProvider->GetBigBlind() * 4;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
					if (_pProvider->GetNumberOpponentsChecking() == 1
						|| _pProvider->GetNumberOpponentsBetting() == 1
						|| _pProvider->GetNumberOpponentsCalling() == 1
						|| _pProvider->GetNumberOpponentsRaising() == 1)
						return (rand() % 2 + 1) * _pProvider->GetPot() / 3;
				}
				break;
			}
		}
		break;
	case TourneyStageFinal:
		break;
	}

	return 0;
}

double FreerollStrategy::GetSrai(void) const
{
	return GetSwag();
}

bool FreerollStrategy::GetCall(void) const
{
	if (GetSwag() > 0)
		return true;

	switch(TourneyStage)
	{
	case TourneyStageEarly:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (!IsRaisedPot()
						&& _pProvider->GetPreflopPosition() != PreflopPositionEarly
						&& _pProvider->GetPreflopPosition() != PreflopPositionMiddle
						&& (_pProvider->TestHand("22", "33", "44", "55", "AXs", "KQs")
							|| _pProvider->TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")))
						return true;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
				}
				break;
			}
		}
		break;
	case TourneyStageLate:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
				}
				break;
			}
		}
		break;
	case TourneyStageFinal:
		break;
	}

	return false;
}

bool FreerollStrategy::GetRais(void) const
{
	return GetSwag() > 0;
}

bool FreerollStrategy::GetAllin(void) const
{
	switch(TourneyStage)
	{
	case TourneyStageEarly:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (_pProvider->TestHand("AA", "KK", "QQ", "AK"))
						return true;
					else if (_pProvider->TestHand("JJ", "AQ") && !IsRaisedPot())
						return true;
					else if (!IsRaisedPot()
							&&_pProvider->GetPreflopPosition() != PreflopPositionEarly
							&& _pProvider->GetBalance() < 8 *_pProvider->GetBigBlind()
							&& _pProvider->TestHand("66", "77", "88", "99", "TT"))
						return true;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
					if (_pProvider->IsMonster())
						return true;
					else if ((_pProvider->IsFlashDro() || _pProvider->IsOESD())
						&& (_pProvider->GetNumberOpponentsBetting()
							+ _pProvider->GetNumberOpponentsCalling()
							+ _pProvider->GetNumberOpponentsRaising()) > 1)
						return true;
					else if ((_pProvider->IsTopPair() || _pProvider->IsOverPair())
						&& (_pProvider->GetNumberOpponentsBetting()
							+ _pProvider->GetNumberOpponentsCalling()
							+ _pProvider->GetNumberOpponentsRaising()) < 3)
						return true;
				}
				break;
			}
		}
		break;
	case TourneyStageLate:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (IsRaisedPot()
						&& (_pProvider->TestHand("AA", "KK", "QQ", "JJ", "TT", "AK")
							|| (_pProvider->GetPreflopPosition() != PreflopPositionEarly
								&& _pProvider->TestHand("99", "AQ"))))
						return true;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
					if (_pProvider->IsMonster()
						|| _pProvider->IsOverPair()
						|| _pProvider->IsTopPair()
						|| (_pProvider->GetRankHiCommonCard() < _pProvider->GetRankHiPocketCard()
							&& _pProvider->GetRankHiCommonCard() < _pProvider->GetRankLoPocketCard()))
						return true;
				}
				break;
			}
		}
		break;
	case TourneyStageFinal:
		break;
	}

	return false;
}

::TourneyStage FreerollStrategy::TourneyStage_get(void) const
{
	::TourneyStage result = TourneyStageUnknown;

	if (_pProvider->GetFlagButtonState(0))
	{
		result = TourneyStageFinal;
	}
	else if (_pProvider->GetBalance() > 25 * _pProvider->GetBigBlind())
	{
		result = TourneyStageEarly;
	}
	else
	{
		result = TourneyStageLate;
	}

	return result;
}

bool FreerollStrategy::IsRaisedPot(void) const
{
	double bigBlind = _pProvider->GetBigBlind();
	for (int i = 0; i < 10; ++i)
	{
		if (_pProvider->GetOpponentBet(i) > bigBlind)
		{
			return true;
		}
	}
	return false;
}