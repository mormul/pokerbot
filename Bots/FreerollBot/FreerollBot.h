// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FREEROLLBOT_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FREEROLLBOT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FREEROLLBOT_EXPORTS
#define FREEROLLBOT_API __declspec(dllexport)
#else
#define FREEROLLBOT_API __declspec(dllimport)
#endif

FREEROLLBOT_API double process_message(const char* pmessage, const void* param);