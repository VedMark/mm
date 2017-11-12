#ifndef MEMOMY_MANAGER_LOGGER_H

void log_print(char *fmt, ...);

/*
 * print format:    [<time>] <message>\n
 */

#define LOG_PRINT(...) log_print(__VA_ARGS__)

#define MEMOMY_MANAGER_LOGGER_H

#endif //MEMOMY_MANAGER_LOGGER_H
