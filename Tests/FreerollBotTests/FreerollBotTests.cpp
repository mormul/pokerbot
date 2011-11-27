// FreerollBotTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <src/gtest-all.cc>
#include <src/gmock-all.cc>

int _tmain(int argc, _TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}