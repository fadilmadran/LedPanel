
/*******************************************************************************
 * HEADER FILE
 *
 * Name:		logger.h
 * Project:		validator-mngr
 * Author:		Bilal Hatipoglu
 * Date Created: 	2013-06-28
 *
 * Description:
 *			Header file of logger library API.
 *
 ******************************************************************************/

#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

// Std Includes
#include <stdio.h>

// Local Includes


// Global Constant Definitions


// Global Macro Definitions
//#define NO_DEBUG

#ifndef NO_DEBUG
#define LOG(_level, _arg...)	LoggerLog(_level, __FILE__, __func__, __LINE__, _arg)
#define TRACE(_arg...)		LOG(LEVEL_TRACE, _arg)
#define ERROR(_arg...)		LOG(LEVEL_ERROR, _arg)
#define DEBUG(_arg...)		LOG(LEVEL_DEBUG, _arg)
#define FATAL(_arg...)		LOG(LEVEL_FATAL, _arg)
#else	//NO_DEBUG
#define LOG(_level, _arg...)
#define TRACE(_arg...)
#define DEBUG(_arg...)
#define FATAL(_arg...)		fprintf(stderr, _arg)
#endif

// Global Data Types
typedef enum {
	LEVEL_NONE = 0,
	LEVEL_FATAL = 0,
	LEVEL_ERROR,
	LEVEL_WARNING,
	LEVEL_DEBUG,
	LEVEL_TRACE,
} LogLevel;

// Global Variable Externs


// Global Function Prototypes
int LoggerInit(LogLevel logLevel, const char *logFileName, unsigned int maxSize);
int LoggerGetLevel(void);
int LoggerSetLevel(LogLevel logLevel);
const char *LoggerGetLogFile(void);
int LoggerSetLogFile(const char *logFileName, unsigned int maxSiz);
int LoggerLog(LogLevel level, const char *file, const char *caller, const int line, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif	//_LOGGER_H_
