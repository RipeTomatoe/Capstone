#ifndef __LOGMANAGER_H_
#define __LOGMANAGER_H_

#include <cstdarg>
#include <stdio.h>
#include <string>
#include <time.h>
#include "Destroyer.h"

class LogManager
{
	friend class Destroyer<LogManager>;

public:
	static LogManager *getInstance(void)
	{
		if(_instance == NULL)
		{
			_instance = new LogManager();
			_destroyer.setSingleton(_instance);
		}
		return _instance;
	}
	char* getTime(void)
	{
		time_t rawtime;
		struct tm *timeinfo;
		timeinfo = new tm();
		time(&rawtime);
		localtime_s(timeinfo, &rawtime);
		asctime_s(cTime, timeinfo);
		int i = strlen(cTime);
		cTime[i-1] = '\0';

		return cTime;
	}
	void trace(char* output, ...)
	{
		fopen_s(&_logFile, "GameLog.log", "a");

		if(_logFile)
		{
			getTime();

			char final[2048];

			va_list arglist;
			va_start(arglist, output);
			vsprintf_s(final, output, arglist);
			va_end(arglist);

			fprintf(_logFile, "[%s] : %s\n", cTime, final);

			fclose(_logFile);
		}
	}

private:
	FILE *_logFile;
	static Destroyer<LogManager> _destroyer;
	static LogManager *_instance;
	char cTime[64];

	LogManager(void)
	{
		_logFile = NULL;
		fopen_s(&_logFile, "GameLog.log", "a");
		if(_logFile)
		{
			fprintf_s(_logFile, " ------START------ \n\n");
			fclose(_logFile);
		}
	}
	virtual ~LogManager(void)
	{
		fopen_s(&_logFile, "GameLog.log", "a");
		if(_logFile)
		{
			fprintf(_logFile, "\n -------END------- \n\n");
			fclose(_logFile);
		}
	}
};

#endif