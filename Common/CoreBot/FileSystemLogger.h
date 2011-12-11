#pragma once
#include "ILogger.h"

class FileSystemLogger : public ILogger
{
public:
	FileSystemLogger(void);
	virtual ~FileSystemLogger(void);
	virtual void WriteLog(const char * pMessage) const;
};

