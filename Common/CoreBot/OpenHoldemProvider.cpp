#include "StdAfx.h"

#include <Reference User DLL\user.h>
#include <poker_defs.h>
#include <deck_std.h>

#include "OpenHoldemProvider.h"
#include "IOpenHoldemStrategy.h"

class OpenHoldemProviderImpl
{
public:
	OpenHoldemProviderImpl() : 	_p_get_symbol(0), _ndx(0)
	{
	}

	~OpenHoldemProviderImpl()
	{
	}

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

	template <typename T>
	T GetSymbol(const char* name) const
	{
		return static_cast<T>(GetSymbol(name));
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
	:_pImpl(NULL)
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

const bool OpenHoldemProvider::GetFlagButtonState(int index) const
{
	std::ostringstream s;
	s << "f" << index;
	return _pImpl->GetSymbol(s.str().c_str()) != 0;
}

const bool OpenHoldemProvider::TestHand(const char * pHand) const
{
	std::ostringstream s;
	s << "$" << pHand;
	return _pImpl->GetSymbol(s.str().c_str()) != 0;
}

const bool OpenHoldemProvider::TestHand(const char * pHand0, const char * pHand1) const
{
	return TestHand(pHand0) || TestHand(pHand1);
}

const bool OpenHoldemProvider::TestHand(const char * pHand0, const char * pHand1, const char * pHand2) const
{
	return TestHand(pHand0) || TestHand(pHand1) || TestHand(pHand2);
}

const bool OpenHoldemProvider::TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3) const
{
	return TestHand(pHand0) || TestHand(pHand1) || TestHand(pHand2) || TestHand(pHand3);
}

const bool OpenHoldemProvider::TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4) const
{
	return TestHand(pHand0) || TestHand(pHand1) || TestHand(pHand2) || TestHand(pHand3) || TestHand(pHand4);
}

const bool OpenHoldemProvider::TestHand(const char * pHand0, const char * pHand1, const char * pHand2, const char * pHand3, const char * pHand4, const char * pHand5) const
{
	return TestHand(pHand0) || TestHand(pHand1) || TestHand(pHand2) || TestHand(pHand3) || TestHand(pHand4) || TestHand(pHand5);
}

const double OpenHoldemProvider::GetBigBlind(void) const
{
	return _pImpl->GetSymbol("bblind");
}

const double OpenHoldemProvider::GetPot(void) const
{
	return _pImpl->GetSymbol("pot");
}

const BetRound OpenHoldemProvider::GetBetRound(void) const
{
	return (BetRound) _pImpl->GetSymbol<int>("betround");
}

const double OpenHoldemProvider::GetBalance(void) const
{
	return _pImpl->GetSymbol("balance");
}

const double OpenHoldemProvider::GetOpponentBalance(int chair) const
{
	std::ostringstream s;
	s << "balance" << chair;
	return _pImpl->GetSymbol(s.str().c_str());
}

const double OpenHoldemProvider::GetBet(void) const
{
	return _pImpl->GetSymbol("currentbet");
}

const double OpenHoldemProvider::GetOpponentBet(int chair) const
{
	std::ostringstream s;
	s << "currentbet" << chair;
	return _pImpl->GetSymbol(s.str().c_str());
}

const PreflopPosition OpenHoldemProvider::GetPreflopPosition(void) const
{
	return (PreflopPosition) _pImpl->GetSymbol<int>("ac_preflop_pos");
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

const bool OpenHoldemProvider::IsMiddlePair(void) const
{
	return (_pImpl->GetSymbol("ismidpair") == 1);
}

const bool OpenHoldemProvider::IsTopPair(void) const
{
	return (_pImpl->GetSymbol("nrankedcommon") != 2) && _pImpl->GetSymbol("ishipair");
}

const bool OpenHoldemProvider::IsOverPair(void) const
{
	return (GetRankHiPocketCard() > GetRankHiCommonCard()) && _pImpl->GetSymbol("ispair");
}

const bool OpenHoldemProvider::IsOESD(void) const
{
	return _pImpl->GetSymbol("nstraight") == 4;
}

const bool OpenHoldemProvider::IsFlashDro(void) const
{
	return _pImpl->GetSymbol("nsuited") == 4;
}

const bool OpenHoldemProvider::IsTwoPair(void) const
{
	return _pImpl->GetSymbol("istwopair") == 1;
}

const bool OpenHoldemProvider::IsMonster(void) const
{
	return _pImpl->GetSymbol("isthreeofakind") == 1
		|| _pImpl->GetSymbol("isstraight") == 1
		|| _pImpl->GetSymbol("isflush") == 1
		|| _pImpl->GetSymbol("isfullhouse")
		|| _pImpl->GetSymbol("isfourofakind") == 1
		|| _pImpl->GetSymbol("isstraightflush") == 1
		|| _pImpl->GetSymbol("isroyalflush") == 1;
}

const int OpenHoldemProvider::GetRankHiPocketCard(void) const
{
	return _pImpl->GetSymbol<int>("rankhiplayer");
}

const int OpenHoldemProvider::GetRankLoPocketCard(void) const
{
	return _pImpl->GetSymbol<int>("rankloplayer");
}

const int OpenHoldemProvider::GetRankHiCommonCard(void) const
{
	return _pImpl->GetSymbol<int>("rankhicommon");
}

//const bool OpenHoldemProvider::IsSeatIn(void) const
//{
//	return _pImpl->GetSymbol("rankbits") != 0;
//}
//
//const double OpenHoldemProvider::GetActiveSeatsCount(void) const
//{
//	return _pImpl->GetSymbol("nplayersactive");
//}
//
//const int OpenHoldemProvider::GetPosition(void) const
//{
//	return static_cast<int>(_pImpl->GetSymbol("originaldealposition"));
//}
//
//const int OpenHoldemProvider::GetDealPosition(void) const
//{
//	return static_cast<int>(_pImpl->GetSymbol("dealposition"));
//}
//
//const bool OpenHoldemProvider::IsList(int number) const
//{
//	return _pImpl->GetSymbol(Format("islist%d", number).c_str()) != 0;
//}
//
//const double OpenHoldemProvider::GetNeedBetsToCall(void) const
//{
//	return _pImpl->GetSymbol("nbetstocall");
//}
//
//const int OpenHoldemProvider::GetMyChair(void) const
//{
//	return _pImpl->GetMyChair();
//}
//
//const int OpenHoldemProvider::GetDealerChair(void) const
//{
//	return _pImpl->GetSymbol("dealerchair");
//	//return _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_dealer_chair;
//}
//
////const bool OpenHoldemProvider::IsPlayerFold(int chair) const
////{
////	//foldbitsx
////	const unsigned char *cards = _pImpl->_holdem_state[_pImpl->_ndx & 0xff].m_player[chair].m_cards;
////	return cards[0] == 0x00 && cards[1] == 0x00;
////}
//
//const bool OpenHoldemProvider::WasPlayerDidRaiseOnPreflop() const
//{
//	return _pImpl->GetSymbol("didraisround1") != 0;
//}
//
//const bool OpenHoldemProvider::WasPlayerDidRaiseOnFlop() const
//{
//	return _pImpl->GetSymbol("didraisround2") != 0;
//}
//
//const int OpenHoldemProvider::GetKicker() const
//{
//	return static_cast<int>(_pImpl->GetSymbol("mh_kickerrank"));
//}
