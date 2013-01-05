#pragma once
#include "IOpenHoldemStrategy.h"

enum TourneyStage
{
	TourneyStageUnknown,
	TourneyStageEarly,
	TourneyStageLate,
	TourneyStageFinal
};

class FreerollStrategy : public IOpenHoldemStrategy
{
public:
	FreerollStrategy(void);
	virtual ~FreerollStrategy(void);

	virtual void ClearCache(void) const;

	__declspec(property(get=TourneyStage_get)) TourneyStage TourneyStage;
	::TourneyStage TourneyStage_get(void) const;

protected:
	virtual double GetSwag(void) const;
	virtual double GetSrai(void) const;
	virtual bool GetCall(void) const;
	virtual bool GetRais(void) const;
	virtual bool GetAllin(void) const;

	virtual void PrepareHeaderLog(void) const;

private:
	bool IsRaisedPot(void) const;

	mutable ::TourneyStage	_tourneyStageCache;
	mutable short			_isRaisedPotCache;
};

