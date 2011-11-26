#pragma once

class OpenHoldemProvider;

class IOpenHoldemStrategy
{
public:
	IOpenHoldemStrategy(void) :_pProvider(NULL) { }
	virtual ~IOpenHoldemStrategy(void) = 0 { }
	void SetProvider(const OpenHoldemProvider * pProvider) { _pProvider = pProvider; }
	virtual double ProcessQuery(const char* pQuery) const = 0;

private:
	const OpenHoldemProvider * _pProvider;
};

