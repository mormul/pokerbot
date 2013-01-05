#pragma once
#include "IOpenHoldemProvider.h"

class IOpenHoldemStrategy
{
public:
	IOpenHoldemStrategy(void) :_pProvider(NULL) { }
	virtual ~IOpenHoldemStrategy(void) = 0 { }

	virtual void ClearCache(void) const = 0;

	virtual void SetProvider(const IOpenHoldemProvider * pProvider) { _pProvider = pProvider; }

	virtual double ProcessQuery(const char* pQuery) const;

protected:
	virtual int GetPlay(void) const { return -1; }
	virtual double GetSwag(void) const { return 0; }
	virtual double GetSrai(void) const { return 0; }
	virtual bool GetCall(void) const { return false; }
	virtual bool GetRais(void) const { return false; }
	virtual bool GetAllin(void) const { return false; }

	virtual void PrepareHeaderLog(void) const { }
	void FlushLog(void) const;

	mutable std::ostringstream _log;
	const IOpenHoldemProvider * _pProvider;
};
