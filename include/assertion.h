#ifndef MM_ASSERTING_H
#define MM_ASSERTING_H

#include "logger.h"

#ifdef __LOGGER__
#define ASSERT(condition, message, return_code) {       \
    if(!(condition)) {                                  \
        LOG_PRINT("Bad assertion: %s", message);        \
        return return_code;                             \
    }                                                   \
};

#define ASSERT_FILE(descr, condition, message, return_code) {   \
    if(!(condition)) {                                          \
        LOG_PRINT("Bad assertion: %s", message);                \
        close(descr);                                           \
        return return_code;                                     \
    }                                                           \
};
#else
#define ASSERT(condition, message, return_code) {       \
    if(!(condition)) {                                  \
        return return_code;                             \
    }                                                   \
};

#define ASSERT_FILE(descr, condition, message, return_code) {   \
    if(!(condition)) {                                          \
        close(descr);                                           \
        return return_code;                                     \
    }                                                           \
};
#endif // __LOGGER__

#endif // MM_ASSERTING_H
