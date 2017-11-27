#ifndef MM_LOGGER_H
#define MM_LOGGER_H

// #define __LOGGER__

void log_print(char *fmt, ...);

/*
 * print format:    [<time>] <message>\n
 */

#define LOG_PRINT(...) log_print(__VA_ARGS__)

#endif //MM_LOGGER_H
