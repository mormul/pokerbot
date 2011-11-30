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
	else if (_pProvider->GetPreflopPosition() != PreflopPositionEarly
			&& _pProvider->TestHand("66", "77", "88", "99", "TT")
			&& !IsRaisedPot())
		return _pProvider->GetBigBlind() * 4;
	else
		return 0;
}

double FreerollStrategy::GetSrai(void) const
{
	return GetSwag();
}

bool FreerollStrategy::GetCall(void) const
{
	return GetSwag() > 0
		|| 	(!IsRaisedPot()
			&& _pProvider->GetPreflopPosition() != PreflopPositionEarly
			&& _pProvider->GetPreflopPosition() != PreflopPositionMiddle
			&& (_pProvider->TestHand("22", "33", "44", "55", "AXs")
				|| _pProvider->TestHand("KQs", "QJs", "JTs")
				|| _pProvider->TestHand("T9s", "98s", "87s", "76s")));
}

bool FreerollStrategy::GetRais(void) const
{
	return GetSwag() > 0;
}

bool FreerollStrategy::GetAllin(void) const
{
	switch(_pProvider->GetBetRound())
	{
	case BetRoundPreflop:
		{
			if (_pProvider->TestHand("AA", "KK", "QQ", "AK"))
				return true;
			else if (_pProvider->TestHand("JJ", "AQ") && !IsRaisedPot())
				return true;
			else if (_pProvider->GetPreflopPosition() != PreflopPositionEarly
					&& _pProvider->GetBalance() < 8 *_pProvider->GetBigBlind()
					&& _pProvider->TestHand("66", "77", "88", "99", "TT")
					&& !IsRaisedPot())
				return true;
		}
	case BetRoundFlop:
	case BetRoundRiver:
	case BetRoundTurn:
		{
			if (_pProvider->IsMonster())
				return true;
			else if ((_pProvider->IsFlashDro() || _pProvider->IsOESD())
				&& _pProvider->GetNumberOpponentsBetting() > 1)
				return true;
			else if ((_pProvider->IsTopPair() || _pProvider->IsOverPair())
				&& _pProvider->GetNumberOpponentsBetting() < 3)
				return true;
		}
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