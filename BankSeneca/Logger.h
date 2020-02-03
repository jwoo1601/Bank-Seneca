#pragma once

#include "CommonIncludes.h"

enum class ELogDevice : unsigned char
{
	Console,
	File
};

class Logger
{
public:

	static void Iniitialize();
	static void Finalize();

	static void SetLogDevice(ELogDevice inDeviceType);

	static void Log(const char *message, ...);
	static void LogError(const char *message, ...);
	static void LogErrorCode(const char *message, int errorCode, ...);

private:

};