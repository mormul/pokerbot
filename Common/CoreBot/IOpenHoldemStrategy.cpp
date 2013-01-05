#include "stdafx.h"
#include "IOpenHoldemStrategy.h"

double IOpenHoldemStrategy::ProcessQuery(const char* pQuery) const
{
	ClearCache();
	PrepareHeaderLog();

	_log << "BetRound=";
	switch (_pProvider->GetBetRound())
	{
	case BetRoundPreflop:
		{
			_log << "Preflop";
			_log << ", PreflopPosition=";
			switch (_pProvider->GetPreflopPosition())
			{
			case PreflopPositionDealer:
				_log << "Dealer";
				break;
			case PreflopPositionLate:
				_log << "Late";
				break;
			case PreflopPositionMiddle:
				_log << "Middle";
				break;
			case PreflopPositionEarly:
				_log << "Early";
				break;
			case PreflopPositionBigBlind:
				_log << "BigBlind";
				break;
			case PreflopPositionSmallBlind:
				_log << "SmallBlind";
				break;
			case PreflopPositionUnknown:
			default:
				_log << "Unknown";
				break;
			}
		}
		break;
	case BetRoundFlop:
		_log << "Flop";
		break;
	case BetRoundTurn:
		_log << "Turn";
		break;
	case BetRoundRiver:
		_log << "River";
		break;
	case BetRoundUnknown:
	default:
		_log << "Unknown";
		break;
	}

	_log << ", BigBlind=" << _pProvider->GetBigBlind();
	_log << ", Balance=" << _pProvider->GetBalance();

	FlushLog();

	double result = 0;
	if(pQuery)
	{
		if(strcmp(pQuery, "dll$play") == 0)
		{
			result = GetPlay();
			_log << "$play=" << result;
		}
		else if(strcmp(pQuery, "dll$swag") == 0)
		{
			result = GetSwag();
			_log << "$swag=" << result;
		}
		else if(strcmp(pQuery, "dll$srai") == 0)
		{
			result = GetSrai();
			_log << "$srai=" << result;
		}
		else if(strcmp(pQuery, "dll$call") == 0)
		{
			result = GetCall();
			_log << "$call=" << result;
		}
		else if(strcmp(pQuery, "dll$rais") == 0)
		{
			result = GetRais();
			_log << "$rais=" << result;
		}
		else if(strcmp(pQuery, "dll$alli") == 0)
		{
			result = GetAllin();
			_log << "$alli=" << result;
		}
	}

	FlushLog();
	_pProvider->WriteLog("");
	return result;
}

void IOpenHoldemStrategy::FlushLog(void) const
{
	if (_log.str().size())
	{
		_pProvider->WriteLog(_log);
		_log.str("");
	}
}
