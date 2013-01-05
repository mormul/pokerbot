#include "stdafx.h"
#include "FileSystemLogger.h"

#pragma warning ( push, 2 )
#define GOOGLE_GLOG_DLL_DECL
#include <glog/logging.h>
#pragma warning ( pop )
#pragma comment(lib, "libglog_static.lib")

FileSystemLogger::FileSystemLogger(void)
{
	google::InitGoogleLogging("");
	google::SetLogDestination(google::INFO, "D:\\PokerBot");
}


FileSystemLogger::~FileSystemLogger(void)
{
	google::ShutdownGoogleLogging();
}

void FileSystemLogger::WriteLog(const char * pMessage) const
{
	LOG(INFO) << pMessage;
}
