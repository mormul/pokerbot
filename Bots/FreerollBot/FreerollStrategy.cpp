#include "StdAfx.h"
#include "FreerollStrategy.h"
#include "IOpenHoldemProvider.h"

FreerollStrategy::FreerollStrategy(void)
{
	ClearCache();
}

FreerollStrategy::~FreerollStrategy(void)
{
}

void FreerollStrategy::ClearCache(void) const
{
	_tourneyStageCache = TourneyStageUnknown;
	_isRaisedPotCache = -1;
}

double FreerollStrategy::GetSwag(void) const
{
	double result = 0;
	double bb = _pProvider->GetBigBlind();

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
					if (_pProvider->TestHand("66", "77", "88", "99", "TT"))
					{
						bool isRaisedPot = IsRaisedPot();
						_log << "66, 77, 88, 99, TT";
						if (_pProvider->GetPreflopPosition() != PreflopPositionEarly)
						{
							_log << " and PreflopPosition is not Early";
							if (!isRaisedPot)
							{
								result = bb * 4;
								_log << " and Pot is not raised => 4*bb(" << bb << ")=" << result;
							}
							else _log << " and Pot is raised => Fold";
						}
						else _log << " and PreflopPosition is Early => Fold";
					}
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
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

	FlushLog();
	return result;
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
	bool result = false;

	switch(TourneyStage)
	{
	case TourneyStageEarly:
		{
			switch(_pProvider->GetBetRound())
			{
			case BetRoundPreflop:
				{
					if (_pProvider->TestHand("AA", "KK", "QQ", "AK"))
					{
						_log << "AA, KK, QQ, AK => always AllIn";
						result = true;
					}
					else if (_pProvider->TestHand("JJ", "AQ"))
					{
						if (result = !IsRaisedPot())
							_log << "JJ, AQ and Pot is not raised => AllIn";
						else
							_log << "JJ, AQ and Pot is raised => Fold";
					}
					//else if (!IsRaisedPot()
					//		&&_pProvider->GetPreflopPosition() != PreflopPositionEarly
					//		&& _pProvider->GetBalance() < 8 *_pProvider->GetBigBlind()
					//		&& _pProvider->TestHand("66", "77", "88", "99", "TT"))
					//	return true;
				}
				break;
			case BetRoundFlop:
			case BetRoundRiver:
			case BetRoundTurn:
				{
					if (_pProvider->IsMonster())
						return true;
					if (_pProvider->IsTwoPair())
						//&& (_pProvider->GetRankHiPocketCard() >= _pProvider->GetRankHiCommonCard()))
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

	FlushLog();
	return result;
}

::TourneyStage FreerollStrategy::TourneyStage_get(void) const
{
	if (_tourneyStageCache == TourneyStageUnknown)
	{
		double balance = _pProvider->GetBalance();
		double bigBlind = _pProvider->GetBigBlind();

		if (_pProvider->GetFlagButtonState(0))
		{
			_tourneyStageCache = TourneyStageFinal;
			_log << "F0 pressed => TourneyStageFinal";
		}
		else if (balance > 25 * bigBlind)
		{
			_tourneyStageCache = TourneyStageEarly;
			_log << "BigBlind(" << bigBlind << ") * 25 < Balance(" << balance << ") => TourneyStageEarly";
		}
		else
		{
			_tourneyStageCache = TourneyStageLate;
			_log << "BigBlind(" << bigBlind << ") * 25 >= Balance(" << balance << ") => TourneyStageLate";
		}
		FlushLog();
	}

	return _tourneyStageCache;
}

bool FreerollStrategy::IsRaisedPot(void) const
{
	if (_isRaisedPotCache == -1)
	{
		_isRaisedPotCache = 0;
		_log << "GetOpponentBet: ";
		double bigBlind = _pProvider->GetBigBlind();
		for (int i = 0; i < 10; ++i)
		{
			double bet = _pProvider->GetOpponentBet(i);
			_log << "ch" << i << "=" << bet << ",";
			if (bet > bigBlind)
			{
				_isRaisedPotCache = 1;
				break;
			}
		}
		_log << " => IsRaisedPot=" << _isRaisedPotCache;
		FlushLog();
	}

	return _isRaisedPotCache == 1;
}

void FreerollStrategy::FlushLog(void) const
{
	if (_log.str().size())
	{
		_pProvider->WriteLog(_log);
		_log.str("");
	}
}
