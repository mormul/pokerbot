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

//    ������ ������: ������� ������ �� ��������� � ��������� �������� ������. � ��� ������ 25 ������� ��������.
TEST_F(FreerollStrategyTests, should_return_early_stage_when_I_have_more_that_25_big_blindes)
{
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageEarly);
}

//    ������� ������: ������� ������� �������. ������ ������ ����� ��������� ���� ������� � 25 ������� ��������.

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

//    ��������� ����: �������� ������. �� �������� ���������� �����.

TEST_F(FreerollStrategyTests, should_return_final_stage_when_I_press_f0)
{
	ON_CALL(_provider, GetFlagButtonState(0)).WillByDefault(Return(true));
	EXPECT_EQ(_strategy.TourneyStage, TourneyStageFinal);
}

//� ����� �� ������� �� �� ����������, ����� ����� ���-�� � ������ �����, �������, ��� � ����� ������,
//���� ���� ����� ���� ��� ������ ����.

TEST_F(FreerollStrategyTests, should_always_went_allin_with_AA_KK_QQ_AK)
{
	ON_CALL(_provider, TestHand("AA", "KK", "QQ", "AK")).WillByDefault(Return(true));
	EXPECT_EQ(_strategy.GetSwag(), 200);
	EXPECT_EQ(_strategy.GetSrai(), 200);
	EXPECT_EQ(_strategy.GetCall(), true);
	EXPECT_EQ(_strategy.GetRais(), true);
	EXPECT_EQ(_strategy.GetAllin(), true);
}

//� ������� � ������� �������� �������� � ������ ��������� ������������ ���-��� ���� ������� � ���� ����, ���� ����� ���� ��� �� ���� ������.

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

//����� � ���������� ������, ������� � �������, � ������� � ������� �������� �� ������ ������ ����
//�������������� �� 4 ������� ������� (����� � ������), ���� ����� ���� ��� �� ���� ������.

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

//� ������� ������� �� ������ �������� ���� �� �������������� ������, ���� ���� ������������.
//��� ��������, ��� ����� ������� (������� ������ 1 ������� ������) � 55, 44, 33, 22,
//������������ ������ � � ��� ����������� ������������ ������������, �������� � ��������.
//��� ����������� �����, ����������� ����� ���� � ������ �� ��������, ��������, 76 ��� QJ.

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

//����� ����� �� ��������� � ����-������������ ������: � ��� ���-������ ����? ���� ���, �� �� ������������� �� ����� ������.

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

//� ����� ������ � ����� �������� ������������ �� ���� ���-��.

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

//� ����-��� ��� ������������ ������-��� (OESD) �� ���� ���-�� �����, ����� ��������� ���������� ����� ����
//��� ���������� ������������� ����� � ����.

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

//� ��� ���������� ���-�����, �����, ��������� �� ����� �� ����� ��������� ���� � ����� ������� ����� �����,
//�� ���� ���-�� ������ �����, ����� � ��� �������� 2 ���������. � ��������� ������� ������ ����� ��������� � ���� �����������. 

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

//��� ������ � ������� ������?
//������� ������ ����������, ����� � ��� ������� �������� 20-25 ������� ��������. ��������� ���� ������ ���������� "���� ��� ����".
//��� ��������, ��� �� �������� �� ���� ������� ����, ���� �������������.
//��� ������ �������� ����� ���� ��� ����� ��� ������ ���� ��� 3-���, �� ���� ���-�� ��� �������������.
//���-�� ��, �������, ���� ������ � ����������� �������. ������ "����" ������ ��� ��� ��� ��������.
//
//������ �������� �������� �� ������� � ���������� �������.
//������ ��� ��� ����� ��������� ����, ���� ��� ������ � ���� �������, ����� ���� ���������, ���� ��������.
//
//���������� �� ����������� � ����� ������������ �����������, � ������� ���� ������ ������. ������ ��� ��� ����� ����� ������� ����������.

//�������
//
//�� ����� ������� ������� ���� �������� �������� 4 ������� ������� � ������ �����, �������, ���, �������, ������� ��� ����� ������.
//���� ���-�� ������� ������ ��� 3-���, ����� ���-��.

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

//� ������� � ������� �������� �������� � ������ ��������� ����� ����� ����, ��� ���� �������, ������ ����, ��� ����� � ��� ����.
//���� ����� ��� ����� ������ 3-���, ��� ������� ���� ���-�� �� ����� �������������� ������,
//����� ������ ���� � ���� ������, ������� ���������� ����������.

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

//��������
//
//� ������ ��������� ������ �������� �������������, ��� �������, ����� ������� - �� �����,
//��� ��� ����� ��� ��������� ������, ��� ��� �� ����� ����������� � ��� ��������.
//
//���� �� ������� �� �������� ���� � � ��� ������� ������ ���� ��������, �� �� ����� � ����� ����������� ������� �� 1/2 �� 2/3 �����.

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

//���� ���-�� ������� ����, �� � ���-����� � �����, � ����� � ����-��� ����� ���-��.

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

//�� �� ����� �������� � ��������, ����� �� �� �� ��� �� ������, �� ��� ���� ��������� ����� ������ ���� ����� ���� ����� (���������).

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

//��� ������ �� ��������� ������?
//
//� �����, ��������� ���� � ������� ������ �������� � ��� ���������� �����, �� ��������� ��������� �� �� ����. �� ������ ������ ������� ����� ����������� ������� �� ��������� � ��������� ������������ ����������, ����� ����� ���� ��� ����������, � �� ������ ������ ������� ����.
//
//��������, ������, ����� �� ������ �������. ���� ����� ��� ������������� ���� ������� ����.
//
//���� ���-�� ������ ���� �� ������ �������, ����� ���-�� � ������ �����, �������, ���, ������� � ����� ������.
//
//���� ���-�� ������ ���� �� ������� �������, �� � ������ ��������� ���-��� �������� ���� ������� � �������, � ����� ���� ����.
//
//���� ��� ����� �� ������ ����, � �� ���������� � ������ ��� ������� �������, �� ����� ���-�� � ����� ����� �� ������� � ������, � ����� � ����� ������, ����� ����� � ����� �������.
//
//���� �� ���������� � ������� �������, �� �� ������ ����� ���� ���-�� � ������ ������, �������� � ������ �����, �����������, ��� �������, ��� ����� ���� �� ���� ������.
