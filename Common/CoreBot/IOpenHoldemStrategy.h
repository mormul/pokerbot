#pragma once

class IOpenHoldemProvider;

class IOpenHoldemStrategy
{
public:
	IOpenHoldemStrategy(void) :_pProvider(NULL) { }
	virtual ~IOpenHoldemStrategy(void) = 0 { }

	virtual void SetProvider(const IOpenHoldemProvider * pProvider) { _pProvider = pProvider; }
	virtual int GetPlay(void) const { return -1; }
	virtual double GetSwag(void) const { return 0; }
	virtual double GetSrai(void) const { return 0; }
	virtual bool GetCall(void) const { return false; }
	virtual bool GetRais(void) const { return false; }
	virtual bool GetAllin(void) const { return false; }

	virtual double ProcessQuery(const char* pQuery) const
	{
		if(!pQuery) return 0;
		else if(strcmp(pQuery, "dll$play") == 0) return GetPlay();
		else if(strcmp(pQuery, "dll$swag") == 0) return GetSwag();
		else if(strcmp(pQuery, "dll$srai") == 0) return GetSrai();
		else if(strcmp(pQuery, "dll$call") == 0) return GetCall();
		else if(strcmp(pQuery, "dll$rais") == 0) return GetRais();
		else if(strcmp(pQuery, "dll$alli") == 0) return GetAllin();
		return 0;
	}

protected:
	const IOpenHoldemProvider * _pProvider;
};
