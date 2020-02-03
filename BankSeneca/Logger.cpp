#include "Logger.h"

#include <cstdio>

#pragma warning(disable: 4996)
#pragma warning(disable: 4474)
#pragma warning(disable: 4477)

static ELogDevice deviceType;

static FILE *fpOutputDevice;
static FILE *fpErrorDevice;

static const char *LogFileName = "output.log";
static const char *ErrorFileName = "error.log";

/*
const char* Logger::LogFileName = "output.log";
const char* Logger::ErrorFileName = "error.log";
*/

void Logger::Iniitialize()
{
	SetLogDevice(ELogDevice::Console);
}

void Logger::Finalize()
{
	if (deviceType == ELogDevice::File)
	{
		fclose(fpOutputDevice);
		fclose(fpErrorDevice);
	}
}

void Logger::SetLogDevice(ELogDevice inDeviceType)
{
	Finalize();

	deviceType = inDeviceType;

	switch (inDeviceType)
	{
	case ELogDevice::Console:
		fpOutputDevice = stdout;
		fpErrorDevice = stderr;
		break;

	case ELogDevice::File:
		fpOutputDevice = fopen(LogFileName, "w");
		fpErrorDevice = fopen(ErrorFileName, "w");
		break;
	}
}

void Logger::Log(const char *message, ...)
{
	char buffer[2048];

	va_list args;
	va_start(args, message);

	sprintf(buffer, "[INFO] % s\n", message);
	vfprintf(fpOutputDevice, buffer, args);

	va_end(args);
}

void Logger::LogError(const char *message, ...)
{
	va_list args;
	va_start(args, message);

	fprintf(fpErrorDevice, "[ERROR] %s\n", message, args);

	va_end(args);
}

void Logger::LogErrorCode(const char *message, int errorCode, ...)
{
	va_list args;
	va_start(args, message);

	fprintf(fpErrorDevice, "[ERROR] %s --errorCode: %d\n", message, errorCode, args);

	va_end(args);
}