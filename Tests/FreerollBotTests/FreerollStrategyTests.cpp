#include "StdAfx.h"
#include "OpenHoldemProviderMock.h"
using ::testing::NiceMock;

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

	FreerollStrategy					_strategy;
	NiceMock<OpenHoldemProviderMock>	_provider;
};

//    Ранняя стадия: размеры стэков по сравнению с блайндами довольно велики. У вас больше 25 больших блайндов.
TEST_F(FreerollStrategyTests, should_return_early_stage_when_I_have_more_that_25_big_blindes)
{
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageEarly);
}

//    Поздняя стадия: блайнды заметно выросли. Размер вашего стэка опустился ниже отметки в 25 больших блайндов.

TEST_F(FreerollStrategyTests, should_return_late_stage_when_I_have_less_that_25_big_blindes)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(20));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageLate);
}

TEST_F(FreerollStrategyTests, should_return_late_stage_when_I_have_25_big_blindes)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(50));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageLate);
}

//    Финальный стол: решающая стадия. Вы достигли последнего стола.

TEST_F(FreerollStrategyTests, should_return_final_stage_when_I_press_f0)
{
	ON_CALL(_provider, GetFlagButtonState(0)).WillByDefault(Return(true));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageFinal);
}

//В какой бы позиции вы ни находились, сразу идите олл-ин с парами тузов, королей, дам и тузом королём,
//даже если перед вами был сделан рейз.

TEST_F(FreerollStrategyTests, should_always_went_allin_with_AA_KK_QQ_AK)
{
	ON_CALL(_provider, TestHand("AA", "KK", "QQ", "AK")).WillByDefault(Return(true));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

//В средней и поздней позициях добавьте к своему диапазону немедленного олл-ина пару валетов и туза даму, если перед вами ещё не было рейзов.

TEST_F(FreerollStrategyTests, should_went_allin_with_JJ_AQ_when_pot_is_not_raised)
{
	ON_CALL(_provider, TestHand("JJ", "AQ")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

TEST_F(FreerollStrategyTests, should_not_went_allin_with_JJ_AQ_when_pot_is_raised)
{
	ON_CALL(_provider, TestHand("JJ", "AQ")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));
	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//Далее с карманными парами, начиная с шестёрок, в средней и поздней позициях вы должны делать рейз
//приблизительно на 4 больших блайнда (можно и больше), если перед вами ещё не было рейзов.

TEST_F(FreerollStrategyTests, should_rais_to_4bb_with_66_77_88_99_TT_when_pot_is_not_raised)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

TEST_F(FreerollStrategyTests, should_not_rais_with_66_77_88_99_TT_when_pot_is_raised)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));
	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

TEST_F(FreerollStrategyTests, should_not_rais_with_66_77_88_99_TT_in_early_position)
{
	ON_CALL(_provider, TestHand("66", "77", "88", "99", "TT")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//В поздней позиции вы можете смотреть флоп со спекулятивными руками, если банк неповышенный.
//Это означает, что можно лимпить (платить только 1 большой блайнд) с 55, 44, 33, 22,
//одномастными тузами и с так называемыми одномастными коннекторами, средними и старшими.
//Это одномастные карты, находящиеся рядом друг с другом по ценности, например, 76 или QJ.

TEST_F(FreerollStrategyTests, should_call_with_22_33_44_55_AXs_KQs_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_late_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

TEST_F(FreerollStrategyTests, should_fold_with_22_33_44_55_AXs_KQs_QJs_JTs_T9s_98s_87s_76s_when_pot_is_not_raised_in_early_or_middle_position)
{
	ON_CALL(_provider, TestHand("22", "33", "44", "55", "AXs", "KQs")).WillByDefault(Return(true));
	ON_CALL(_provider, TestHand("QJs", "JTs", "T9s", "98s", "87s", "76s")).WillByDefault(Return(true));
	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(2));
	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//После флопа всё упирается в один-единственный вопрос: у вас что-нибудь есть? Если нет, то вы сбрасываетесь на любую ставку.

TEST_F(FreerollStrategyTests, fold_on_postflop_if_you_have_nothing)
{
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(false));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//С двумя парами и более сильными комбинациями вы идёте олл-ин.

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_monster)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(true));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

//С флеш-дро или двусторонним стрейт-дро (OESD) вы идёте олл-ин тогда, когда несколько оппонентов перед вами
//уже продолжили инвестировать фишки в банк.

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_OESD_or_FlasDro_more_than_one_opponents)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsOESD()).WillByDefault(Return(true));
	ON_CALL(_provider, IsFlashDro()).WillByDefault(Return(true));
	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//С так называемой топ-парой, парой, состоящей из одной из ваших стартовых карт и самой старшей карты борда,
//вы идёте олл-ин только тогда, когда у вас максимум 2 оппонента. В остальных случаях будьте очень осторожны с этой комбинацией. 

TEST_F(FreerollStrategyTests, allin_on_postflop_if_you_have_top_pair_and_2_or_less_opponents)
{
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsTopPair()).WillByDefault(Return(true));
	ON_CALL(_provider, IsOverPair()).WillByDefault(Return(true));

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(3));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(3));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(3));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

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
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

//В средней и поздней позициях добавьте к своему диапазону рейза такие руки, как пары девяток, король дама, туз валет и туз дама.
//Если после вас будет сделан 3-бет, вам следует идти олл-ин со всеми перечисленными руками,
//кроме короля дамы и туза валета, которые необходимо сбрасывать.

TEST_F(FreerollStrategyTests, late_stage_midle_and_late_position_rais_4bb_or_allin_with_99_AQ)//_and_rais_only_with_AJ_KQ)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
	ON_CALL(_provider, TestHand("99", "AQ")).WillByDefault(Return(true));


	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);


	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

TEST_F(FreerollStrategyTests, late_stage_midle_and_late_position_rais_4bb_or_fold_with_AJ_KQ)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundPreflop));
	ON_CALL(_provider, TestHand("AJ", "KQ")).WillByDefault(Return(true));


	ON_CALL(_provider, GetOpponentBet(_)).WillByDefault(Return(0));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	EXPECT_EQ(_strategy.GetSwag(), 8);
	EXPECT_EQ(_strategy.GetSrai(), 8);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);


	ON_CALL(_provider, GetOpponentBet(3)).WillByDefault(Return(10));

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionDealer));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionLate));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionMiddle));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionEarly));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionBigBlind));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetPreflopPosition()).WillByDefault(Return(PreflopPositionSmallBlind));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
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
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsChecking()).WillByDefault(Return(1));
	double swag = _strategy.GetSwag();
	double srai = _strategy.GetSrai();
	EXPECT_EQ(swag > 3, true);
	EXPECT_EQ(swag < 7, true);
	EXPECT_EQ(srai > 3, true);
	EXPECT_EQ(srai < 7, true);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsChecking()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(1));
	swag = _strategy.GetSwag();
	srai = _strategy.GetSrai();
	EXPECT_EQ(swag > 3, true);
	EXPECT_EQ(swag < 7, true);
	EXPECT_EQ(srai > 3, true);
	EXPECT_EQ(srai < 7, true);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsBetting()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(1));
	swag = _strategy.GetSwag();
	srai = _strategy.GetSrai();
	EXPECT_EQ(swag > 3, true);
	EXPECT_EQ(swag < 7, true);
	EXPECT_EQ(srai > 3, true);
	EXPECT_EQ(srai < 7, true);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsCalling()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(0));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(1));
	swag = _strategy.GetSwag();
	srai = _strategy.GetSrai();
	EXPECT_EQ(swag > 3, true);
	EXPECT_EQ(swag < 7, true);
	EXPECT_EQ(srai > 3, true);
	EXPECT_EQ(srai < 7, true);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetNumberOpponentsRaising()).WillByDefault(Return(2));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//Если кто-то сделает рейз, то с топ-парой и лучше, а также с флеш-дро идите олл-ин.

TEST_F(FreerollStrategyTests, late_stage_allin_with_monster)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsMonster()).WillByDefault(Return(true));

	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

TEST_F(FreerollStrategyTests, late_stage_allin_with_top_pair)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsTopPair()).WillByDefault(Return(true));

	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

TEST_F(FreerollStrategyTests, late_stage_allin_with_over_pair)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, IsOverPair()).WillByDefault(Return(true));

	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

//То же самое касается и ситуации, когда вы ни во что не попали, но обе ваши стартовые карты старше всех общих карт стола (оверкарты).

TEST_F(FreerollStrategyTests, late_stage_allin_with_two_hi_cards)
{
	ON_CALL(_provider, GetBalance()).WillByDefault(Return(35));
	ON_CALL(_provider, GetBetRound()).WillByDefault(Return(BetRoundFlop));
	ON_CALL(_provider, GetRankHiPocketCard()).WillByDefault(Return(6));
	ON_CALL(_provider, GetRankLoPocketCard()).WillByDefault(Return(8));

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(5));
	EXPECT_EQ(_strategy.GetSwag(), 35);
	EXPECT_EQ(_strategy.GetSrai(), 35);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(9));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetRankHiCommonCard()).WillByDefault(Return(7));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);

	ON_CALL(_provider, GetRankHiPocketCard()).WillByDefault(Return(8));
	ON_CALL(_provider, GetRankLoPocketCard()).WillByDefault(Return(6));
	EXPECT_EQ(_strategy.GetSwag(), 0);
	EXPECT_EQ(_strategy.GetSrai(), 0);
	EXPECT_EQ(_strategy.GetCall(), false);
	EXPECT_EQ(_strategy.GetRais(), false);
	EXPECT_EQ(_strategy.GetAllin(), false);
}

//Как играть за финальным столом?
//
//В целом, стратегия игры в поздней стадии работает и для финального стола, но несколько изменений всё же есть. Вы теперь должны гораздо более внимательно следить за позициями и стараться пользоваться ситуациями, когда перед вами все сбросились, и вы можете первым сделать рейз.
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
