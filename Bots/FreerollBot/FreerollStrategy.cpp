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
	if (_pProvider->TestHand("AA", "KK", "QQ", "AK"))
		return _pProvider->GetBalance();
	else
		return 0;
}

double FreerollStrategy::GetSrai(void) const
{
	return 0;
}

bool FreerollStrategy::GetCall(void) const
{
	return false;
}

bool FreerollStrategy::GetRais(void) const
{
	return false;
}

bool FreerollStrategy::GetAllin(void) const
{
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