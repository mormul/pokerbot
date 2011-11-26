// FreerollBotTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FreerollStrategy.h"

#include <src/gtest-all.cc>
#include <src/gmock-all.cc>

int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

class FreerollSrategyTests : public ::testing::Test
{
protected:
	FreerollSrategyTests(void) { }
};

TEST_F(FreerollSrategyTests, first_test)
{
	ASSERT_EQ(0, 0);
}
