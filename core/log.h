
#ifndef LOG_H
#define LOG_H

#include <sstream>
#include "settings.h"

// Console colors
#define COLOR_NORMAL    "\x1B[0m"
#define COLOR_BLACK     "\x1B[30m"
#define COLOR_RED       "\x1B[31m"
#define COLOR_GREEN     "\x1B[32m"
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE      "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_CYAN      "\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"

// Console background colors
#define B_COLOR_BLACK     "\x1B[40m"
#define B_COLOR_RED       "\x1B[41m"
#define B_COLOR_GREEN     "\x1B[42m"
#define B_COLOR_YELLOW    "\x1B[43m"
#define B_COLOR_BLUE      "\x1B[44m"
#define B_COLOR_MAGENTA   "\x1B[45m"
#define B_COLOR_CYAN      "\x1B[46m"
#define B_COLOR_WHITE     "\x1B[47m"

/* log_level
0   debug
1   info
2   warning
3   error
4   critical error
5   nothing
*/

// Don't use these.
void log_debug(const char function[], const int line, std::string m);
void log_info(const char function[], const int line, std::string m);
void log_warning(const char function[], const int line, std::string m);
void log_error(const char function[], const int line, std::string m);
void log_critical_error(const char function[], const int line, std::string m);

// Use these.
#define debug(m) log_debug(__func__, __LINE__, m)
#define info(m) log_info(__func__, __LINE__, m)
#define warning(m) log_warning(__func__, __LINE__, m)
#define error(m) log_error(__func__, __LINE__, m)
#define critical_error(m) log_critical_error(__func__, __LINE__, m)
#define outOfMemory() critical_error("Out of memory.")

#endif // LOG_H
