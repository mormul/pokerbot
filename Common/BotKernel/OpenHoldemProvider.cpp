#include "StdAfx.h"

#include <Reference User DLL\user.h>
#include <poker_defs.h>
#include <deck_std.h>

#include "OpenHoldemProvider.h"
#include "IOpenHoldemStrategy.h"

class OpenHoldemProviderImpl
{
public:
	OpenHoldemProviderImpl() : 	_p_get_symbol(0), _ndx(0) { }
	~OpenHoldemProviderImpl() { }
	void SetCallBack(p_getsym_t p_get_symbol) { _p_get_symbol = p_get_symbol; }

	void ProcessState(holdem_state* pState)
	{
		if(pState != NULL)
		{
			_holdem_state[(++_ndx) & 0xff] = *pState;
		}
	}

	double GetSymbol(int chair, const char* name, bool& iserr) const
	{
		return (*_p_get_symbol)(chair,name,iserr);
	} 

	double GetSymbol(const char* name) const
	{
		bool iserr;
		return GetSymbol(GetMyChair(), name, iserr);
	}

	const int GetMyChair(void) const
	{
		bool iserr;
		return static_cast<int>(GetSymbol(0, "userchair", iserr));
	}

private:
	OpenHoldemProviderImpl(OpenHoldemProviderImpl &);
	OpenHoldemProviderImpl & operator=(OpenHoldemProviderImpl &);

	p_getsym_t		_p_get_symbol;
	holdem_state	_holdem_state[256]; 
	unsigned char	_ndx;
};

OpenHoldemProvider::OpenHoldemProvider(void)
	: _pImpl(NULL), _pStrategy(NULL)
{
	_pImpl = new OpenHoldemProviderImpl();
}


OpenHoldemProvider::~OpenHoldemProvider(void)
{
	delete _pImpl;
}

double OpenHoldemProvider::ProcessMessage(const char* pmessage, const void* param) const
{
	if(pmessage == NULL)
	{
		return 0;
	}

	if(param == NULL)
	{
		return 0;
	}

	if(strcmp(pmessage, "state") == 0)
	{
		_pImpl->ProcessState(reinterpret_cast<holdem_state*>(const_cast<void*>(param)));
		return 0;
	}

	if(strcmp(pmessage, "query") == 0)
	{ 
		return _pStrategy->ProcessQuery(static_cast<const char*>(param));
	}

	if(strcmp(pmessage, "pfgws") == 0)
	{
		_pImpl->SetCallBack(reinterpret_cast<p_getsym_t>(const_cast<void*>(param)));
		return 0;
	}

	return 0;
}

const bool OpenHoldemProvider::IsSeatIn(void) const
{
	return _pImpl->GetSymbol("rankbits") != 0;
}

const double OpenHoldemProvider::GetActiveSeatsCount(void) const
{
	return _pImpl->GetSymbol("nplayersactive");
}

const double OpenHoldemProvider::GetBalance(void) const
{
	return _pImpl->GetSymbol("balance");
	//return _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_player[GetMyChair()].m_balance;
}

const double OpenHoldemProvider::GetBigBlind(void) const
{
	return _pImpl->GetSymbol("bblind");
}

const BetRound OpenHoldemProvider::GetBetRound(void) const
{
	return static_cast<BetRound>(static_cast<int>(_pImpl->GetSymbol("betround")));
}

const int OpenHoldemProvider::GetPosition(void) const
{
	return static_cast<int>(_pImpl->GetSymbol("originaldealposition"));
}

const int OpenHoldemProvider::GetDealPosition(void) const
{
	return static_cast<int>(_pImpl->GetSymbol("dealposition"));
}

#include <string>

template <typename T>
std::string Format(std::string format, T val1)
{
	char buffer[512];
	sprintf_s(buffer, sizeof(buffer), format.c_str(), val1);
	return buffer;
}

const bool OpenHoldemProvider::IsList(int number) const
{
	return _pImpl->GetSymbol(Format("islist%d", number).c_str()) != 0;
}

const double OpenHoldemProvider::GetNumberOpponentsChecking(void) const
{
	return _pImpl->GetSymbol("nopponentschecking");
}

const double OpenHoldemProvider::GetNumberOpponentsCalling(void) const
{
	return _pImpl->GetSymbol("nopponentscalling");
}

const double OpenHoldemProvider::GetNumberOpponentsRaising(void) const
{
	return _pImpl->GetSymbol("nopponentsraising");
}

const double OpenHoldemProvider::GetNumberOpponentsBetting(void) const
{
	return _pImpl->GetSymbol("nopponentsbetting");
}

const double OpenHoldemProvider::GetNumberOpponentsFolded(void) const
{
	return _pImpl->GetSymbol("nopponentsfolded");
}

const double OpenHoldemProvider::GetNeedBetsToCall(void) const
{
	return _pImpl->GetSymbol("nbetstocall");
}

const double OpenHoldemProvider::GetCurrentBet(void) const
{
	return GetPlayerCurrentBet(GetMyChair());
}

const int OpenHoldemProvider::GetMyChair(void) const
{
	return _pImpl->GetMyChair();
}

const int OpenHoldemProvider::GetDealerChair(void) const
{
	return _pImpl->GetSymbol("dealerchair");
	//return _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_dealer_chair;
}

//const bool OpenHoldemProvider::IsPlayerFold(int chair) const
//{
//	//foldbitsx
//	const unsigned char *cards = _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_player[chair].m_cards;
//	return cards[0] == 0x00 && cards[1] == 0x00;
//}

const double OpenHoldemProvider::GetPlayerCurrentBet(int chair) const
{
	return _pImpl->GetSymbol(Format("currentbet%d", chair).c_str());
	//return _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_player[chair].m_currentbet;
}

const bool OpenHoldemProvider::WasPlayerDidRaiseOnPreflop() const
{
	return _pImpl->GetSymbol("didraisround1") != 0;
}

const bool OpenHoldemProvider::WasPlayerDidRaiseOnFlop() const
{
	return _pImpl->GetSymbol("didraisround2") != 0;
}

const bool OpenHoldemProvider::IsMiddlePair() const
{
	return (_pImpl->GetSymbol("ismidpair") == 1) && (_pImpl->GetSymbol("rankhicommon") < 14);
}

const bool OpenHoldemProvider::IsTopPair() const
{
	return (_pImpl->GetSymbol("ishipair") == 1) && (_pImpl->GetSymbol("nrankedcommon") != 2);
}

const bool OpenHoldemProvider::IsOverPair() const
{
	return (_pImpl->GetSymbol("rankhiplayer") > _pImpl->GetSymbol("rankhicommon")) && _pImpl->GetSymbol("ispair");
}

const bool OpenHoldemProvider::IsOESD() const
{
	return _pImpl->GetSymbol("nstraight") == 4;
}

const bool OpenHoldemProvider::IsFlashDro() const
{
	return _pImpl->GetSymbol("nsuited") == 4;
}

const bool OpenHoldemProvider::IsMonster() const
{
	return _pImpl->GetSymbol("istwopair") == 1
		|| _pImpl->GetSymbol("isthreeofakind") == 1
		|| _pImpl->GetSymbol("isstraight") == 1
		|| _pImpl->GetSymbol("isflush") == 1
		|| _pImpl->GetSymbol("isfullhouse")
		|| _pImpl->GetSymbol("isfourofakind") == 1
		|| _pImpl->GetSymbol("isstraightflush") == 1
		|| _pImpl->GetSymbol("isroyalflush") == 1;
}

const int OpenHoldemProvider::GetKicker() const
{
	return static_cast<int>(_pImpl->GetSymbol("mh_kickerrank"));
}

const double OpenHoldemProvider::GetPot() const
{
	return _pImpl->GetSymbol("pot");
}
