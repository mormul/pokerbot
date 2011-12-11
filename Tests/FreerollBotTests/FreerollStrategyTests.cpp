#include "StdAfx.h"
#include "OpenHoldemProviderMock.h"

#include "FreerollStrategy.cpp"

class FreerollStrategyTests : public ::testing::Test
{
protected:
	FreerollStrategyTests(void)
	{
		_strategy.SetProvider(&_provider);

		ON_CALL(_provider, GetBigBlind()).WillByDefault(Return(2));
		ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
		ON_CALL(_provider, GetBalance()).WillByDefault(Return(200));
		ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	}

	virtual ~FreerollStrategyTests(void) { }

	void ExpectFold()
	{
		EXPECT_EQ(_strategy.ProcessQuery("dll$swag"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$srai"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$call"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$rais"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$alli"), 0);
	}

	void ExpectCall()
	{
		EXPECT_EQ(_strategy.ProcessQuery("dll$swag"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$srai"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$call"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$rais"), 0);
		EXPECT_EQ(_strategy.ProcessQuery("dll$alli"), 0);
	}

	void ExpectRais4bb(void)
	{
		EXPECT_EQ(_strategy.ProcessQuery("dll$swag"), 8);
		EXPECT_EQ(_strategy.ProcessQuery("dll$srai"), 8);
		EXPECT_EQ(_strategy.ProcessQuery("dll$call"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$rais"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$alli"), 0);
	}

	void ExpectRais1or2of3ofPot(void)
	{
		double swag = _strategy.ProcessQuery("dll$swag");
		double srai = _strategy.ProcessQuery("dll$srai");
		EXPECT_EQ(swag > 3, true);
		EXPECT_EQ(swag < 7, true);
		EXPECT_EQ(srai > 3, true);
		EXPECT_EQ(srai < 7, true);
		EXPECT_EQ(_strategy.ProcessQuery("dll$call"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$rais"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$alli"), 0);
	}

	void ExpectAllIn(void)
	{
		double balance = _provider.GetBalance();
		EXPECT_EQ(_strategy.ProcessQuery("dll$swag"), balance);
		EXPECT_EQ(_strategy.ProcessQuery("dll$srai"), balance);
		EXPECT_EQ(_strategy.ProcessQuery("dll$call"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$rais"), 1);
		EXPECT_EQ(_strategy.ProcessQuery("dll$alli"), 1);
	}

	void ExpectEarlyStageLogRecord(const int times)
	{
		//EXPECT_CALL(_provider, WriteLog(StrEq("BigBlind(2) * 25 < Balance(200) => TourneyStageEarly"))).Times(times);
	}

	FreerollStrategy					_strategy;
	NiceMock<OpenHoldemProviderMock>	_provider;
};

//    Ранняя стадия: размеры стэков по сравнению с блайндами довольно велики. У вас больше 25 больших блайндов.
TEST_F(FreerollStrategyTests, should_return_early_stage_when_I_have_more_that_25_big_blindes)
{
	ExpectEarlyStageLogRecord(1);
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageEarly);
}

//    Поздняя стадия: блайнды заметно выросли. Размер вашего стэка опустился ниже отметки в 25 больших блайндов.

TEST_F(FreerollStrategyTests, should_return_late_stage_when_I_have_less_that_25_big_blindes)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(20));
	//EXPECT_CALL(_provider, WriteLog(StrEq("BigBlind(2) * 25 >= Balance(20) => TourneyStageLate")));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageLate);
}

TEST_F(FreerollStrategyTests, should_return_late_stage_when_I_have_25_big_blindes)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(50));
	//EXPECT_CALL(_provider, WriteLog(StrEq("BigBlind(2) * 25 >= Balance(50) => TourneyStageLate")));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageLate);
}

//    Финальный стол: решающая стадия. Вы достигли последнего стола.

TEST_F(FreerollStrategyTests, should_return_final_stage_when_I_press_f0)
{
	ON_CALL(_provider, GetFlagButtonState(0)).WillByDefault(Return(true));
	//EXPECT_CALL(_provider, WriteLog(StrEq("F0 pressed => TourneyStageFinal")));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageFinal);
}

//В какой бы позиции вы ни находились, сразу идите олл-ин с парами тузов, королей, дам и тузом королём,
//даже если перед вами был сделан рейз.

TEST_F(FreerollStrategyTests, should_always_went_allin_with_AA_KK_QQ_AK)
{
	ON_CALL(_provider, TestHand("AA", "KK", "QQ", "AK")).WillByDefault(Return(true));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("AA, KK, QQ, AK => always AllIn"))).Times(5);
	ExpectAllIn();
}

//В средней и поздней позициях добавьте к своему диапазону немедленного олл-ина пару валетов и туза даму, если перед вами ещё не было рейзов.

TEST_F(FreerollStrategyTests, should_went_allin_with_JJ_AQ_when_pot_is_not_raised)
{
	ON_CALL(_provider, TestHand("JJ", "AQ")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("JJ, AQ and Pot is not raised => AllIn"))).Times(5);
	ExpectAllIn();
}

TEST_F(FreerollStrategyTests, should_not_went_allin_with_JJ_AQ_when_pot_is_raised)
{
	ON_CALL(_provider, TestHand("JJ", "AQ")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));
	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=0,ch1=0,ch2=0,ch3=10, => IsRaisedPot=1"))).Times(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("JJ, AQ and Pot is raised => Fold"))).Times(5);
	ExpectFold();
}

//Далее с карманными парами, начиная с шестёрок, в средней и поздней позициях вы должны делать рейз
//приблизительно на 4 больших блайнда (можно и больше), если перед вами ещё не было рейзов.

TEST_F(FreerollStrategyTests, should_rais_to_4bb_with_66_77_88_99_TT_when_pot_is_not_raised)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(4);
	//EXPECT_CALL(_provider, WriteLog(StrEq("66, 77, 88, 99, TT and PreflopPosition is not Early and Pot is not raised => 4*bb(2)=8"))).Times(4);
	ExpectRais4bb();
}

TEST_F(FreerollStrategyTests, should_not_rais_with_66_77_88_99_TT_when_pot_is_raised)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));
	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=0,ch1=0,ch2=0,ch3=10, => IsRaisedPot=1"))).Times(4);
	//EXPECT_CALL(_provider, WriteLog(StrEq("66, 77, 88, 99, TT and PreflopPosition is not Early and Pot is raised => Fold"))).Times(4);
	ExpectFold();
}

TEST_F(FreerollStrategyTests, should_not_rais_with_66_77_88_99_TT_in_early_position)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(4);
	//EXPECT_CALL(_provider, WriteLog(StrEq("66, 77, 88, 99, TT and PreflopPosition is Early => Fold"))).Times(4);
	ExpectFold();
}

//В поздней позиции вы можете смотреть флоп со спекулятивными руками, если банк неповышенный.
//Это означает, что можно лимпить (платить только 1 большой блайнд) с 55, 44, 33, 22,
//одномастными тузами и с так называемыми одномастными коннекторами, средними и старшими.
//Это одномастные карты, находящиеся рядом друг с другом по ценности, например, 76 или QJ.

TEST_F(FreerollStrategyTests, should_call_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_dealer_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_late_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_big_blind_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_small_blind_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_fold_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_early_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectFold();
}

TEST_F(FreerollStrategyTests, should_fold_with_22_33_44_55_AXs_KQs_when_pot_is_not_raised_in_middle_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectFold();
}

TEST_F(FreerollStrategyTests, should_call_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_dealer_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_late_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_big_blind_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_call_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_small_blind_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectCall();
}

TEST_F(FreerollStrategyTests, should_fold_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_early_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectFold();
}

TEST_F(FreerollStrategyTests, should_fold_with_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_middle_position)
{
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectEarlyStageLogRecord(5);
	//EXPECT_CALL(_provider, WriteLog(StrEq("GetOpponentBet: ch0=2,ch1=2,ch2=2,ch3=2,ch4=2,ch5=2,ch6=2,ch7=2,ch8=2,ch9=2, => IsRaisedPot=0"))).Times(1);
	ExpectFold();
}

//После флопа всё упирается в один-единственный вопрос: у вас что-нибудь есть? Если нет, то вы сбрасываетесь на любую ставку.

TEST_F(FreerollStrategyTests, fold_on_postflop_if_you_have_nothing)
{
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(false));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ExpectFold();
}

//С двумя парами и более сильными комбинациями вы идёте олл-ин.

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_two_pair)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsTwoPair()).WillByDefault(Return(true));
	ExpectAllIn();
}

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_monster)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(true));
	ExpectAllIn();
}

//С флеш-дро или двусторонним стрейт-дро (OESD) вы идёте олл-ин тогда, когда несколько оппонентов перед вами
//уже продолжили инвестировать фишки в банк.

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_OESD_or_FlasDro_more_than_one_opponents)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsOESD()).WillByDefault(Return(true));
	ON_CALL(_provider, IsFlashDro()).WillByDefault(Return(true));

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	ExpectFold();
}

//С так называемой топ-парой, парой, состоящей из одной из ваших стартовых карт и самой старшей карты борда,
//вы идёте олл-ин только тогда, когда у вас максимум 2 оппонента. В остальных случаях будьте очень осторожны с этой комбинацией. 

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_top_pair_and_2_or_less_opponents)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsTopPair()).WillByDefault(Return(true));
	ON_CALL(_provider, IsOverPair()).WillByDefault(Return(true));

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(3));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(3));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(3));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	ExpectAllIn();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	ExpectAllIn();
}

//TODO

//Как играть в поздней стадии?
//Поздняя стадия начинается, когда у вас остаётся примерно 20-25 больших блайндов. Стратегия игры теперь называется "Рейз или Фолд".
//Это означает, что на префлопе вы либо делаете рейз, либо сбрасываетесь.
//Как только оппонент перед вами или после вас делает рейз или 3-бет, вы идёте олл-ин или сбрасываетесь.
//Олл-ин вы, конечно, идёте только с приемлемыми картами. Кнопка "Колл" теперь для вас под запретом.
//
//Особое внимание обратите на игроков с маленькими стэками.
//Против них вам нужна приличная рука, ведь они только и ждут момента, чтобы либо удвоиться, либо вылететь.
//
//Старайтесь не связываться с очень агрессивными соперниками, у которых стэк больше вашего. Против них вам также нужна сильная комбинация.

//Префлоп
//
//Из любой позиции делайте рейз размером примерно 4 больших блайнда с парами тузов, королей, дам, валетов, десяток или тузом королём.
//Если кто-то сделает против вас 3-бет, идите олл-ин.

TEST_F(FreerollStrategyTests, late_stage_rais_4bb_or_allin_with_AA_KK_QQ_JJ_TT_AK)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
	ON_CALL(_provider, TestHand("AA", "KK", "QQ", "JJ", "TT", "AK")).WillByDefault(Return(true));

	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));
	ExpectRais4bb();

	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	ExpectAllIn();
}

//В средней и поздней позициях добавьте к своему диапазону рейза такие руки, как пары девяток, король дама, туз валет и туз дама.
//Если после вас будет сделан 3-бет, вам следует идти олл-ин со всеми перечисленными руками,
//кроме короля дамы и туза валета, которые необходимо сбрасывать.

TEST_F(FreerollStrategyTests, late_stage_midle_and_late_position_rais_4bb_or_allin_with_99_AQ)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
	ON_CALL(_provider, TestHand("99", "AQ")).WillByDefault(Return(true));


	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectRais4bb();


	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectAllIn();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectAllIn();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectAllIn();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectAllIn();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectAllIn();
}

TEST_F(FreerollStrategyTests, late_stage_midle_and_late_position_rais_4bb_or_fold_with_AJ_KQ)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
	ON_CALL(_provider, TestHand("AJ", "KQ")).WillByDefault(Return(true));


	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectRais4bb();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectRais4bb();


	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	ExpectFold();

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	ExpectFold();
}

//Постфлоп
//
//В данной турнирной стадии розыгрыш заканчивается, как правило, самое позднее - на флопе,
//так как банки уже настолько велики, что уже на флопе становишься к ним привязан.
//
//Если вы сделали на префлопе рейз и у вас остался только один соперник, то на флопе с любой комбинацией ставьте от 1/2 до 2/3 банка.

TEST_F(FreerollStrategyTests, late_stage_rais_1_or_2_of_3_of_pot_with_one_opponent)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, GetPot()).WillByDefault(Return(10));

	ON_CALL(_provider, GetNumberOpponentsChecking()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsChecking()).WillByDefault(Return(1));
	ExpectRais1or2of3ofPot();

	ON_CALL(_provider, GetNumberOpponentsChecking()).WillByDefault(Return(2));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	ExpectRais1or2of3ofPot();

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	ExpectRais1or2of3ofPot();

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	ExpectFold();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	ExpectRais1or2of3ofPot();

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	ExpectFold();
}

//Если кто-то сделает рейз, то с топ-парой и лучше, а также с флеш-дро идите олл-ин.

TEST_F(FreerollStrategyTests, late_stage_allin_with_monster)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(true));
	ExpectAllIn();
}

TEST_F(FreerollStrategyTests, late_stage_allin_with_top_pair)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsTopPair()).WillByDefault(Return(true));
	ExpectAllIn();
}

TEST_F(FreerollStrategyTests, late_stage_allin_with_over_pair)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsOverPair()).WillByDefault(Return(true));
	ExpectAllIn();
}

//То же самое касается и ситуации, когда вы ни во что не попали, но обе ваши стартовые карты старше всех общих карт стола (оверкарты).

TEST_F(FreerollStrategyTests, late_stage_allin_with_two_hi_cards)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, GetRankHiPocketCard()).WillByDefault(Return(6));
	ON_CALL(_provider, GetRankLoPocketCard()).WillByDefault(Return(8));

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(5));
	ExpectAllIn();

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(9));
	ExpectFold();

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(7));
	ExpectFold();

	ON_CALL(_provider, GetRankHiPocketCard()).WillByDefault(Return(8));
	ON_CALL(_provider, GetRankLoPocketCard()).WillByDefault(Return(6));
	ExpectFold();
}

//Как играть за финальным столом?
//
//В целом, стратегия игры в поздней стадии работает и для финального стола, но несколько изменений всё же есть.
//Вы теперь должны гораздо более внимательно следить за позициями и стараться пользоваться ситуациями,
//когда перед вами все сбросились, и вы можете первым сделать рейз.
//
//Уважайте, однако, рейзы из ранних позиций. Чаще всего там действительно есть сильная рука.
//
//Если кто-то сделал рейз из ранней позиции, идите олл-ин с парами тузов, королей, дам, валетов и тузом королём.
//
//Если кто-то сделал рейз из средней позиции, то к своему диапазону олл-ина добавьте пары десяток и девяток, а также туза даму.
//
//Если ещё никто не сделал рейз, и вы находитесь в ранней или средней позиции, то идите олл-ин с любой парой от девяток и старше, а также с тузом королём, тузом дамой и тузом валетом.
//
//Если вы находитесь в поздней позиции, то вы можете также идти олл-ин с парами семёрок, восьмёрок и королём дамой, естественно, при условии, что перед вами не было рейзов.

//TEST_F(FreerollStrategyTests, should_fold_with_two_pairs_when_my_pair_is_low_then_common)
//{
//	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
//	ON_CALL(_provider, IsTwoPair()).WillByDefault(Return(true));
//	ON_CALL(_provider, GetRankHiPocketCard()).WillByDefault(Return(5));
//	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(13));
//	ExpectFold();
//}
