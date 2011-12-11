#pragma once
#include "ILogger.h"

class LoggerMock : public ILogger
{
public:
	LoggerMock(void) { }
	virtual ~LoggerMock(void) { }
	virtual void WriteLog(const char * pMessage) const { };
};