#pragma once

class ILogger
{
public:
	ILogger(void) { }
	virtual ~ILogger(void) = 0 { };
	virtual void WriteLog(const char * pMessage) const = 0;
};