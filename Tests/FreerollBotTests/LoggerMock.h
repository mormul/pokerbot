#pragma once
#include "ILogger.h"

class LoggerMock : public ILogger
{
public:
	LoggerMock(void) : index(0) { }
	virtual ~LoggerMock(void) { }
	virtual void WriteLog(const char * pMessage) const { log.push_back(pMessage); };

	mutable std::vector<std::string> log;
	mutable unsigned int index;
};

#define EXPECT_LOG(expected) \
	ASSERT_GT(_logger.log.size(), _logger.index); \
	EXPECT_EQ(expected, _logger.log[_logger.index++]);