#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "log_config.h"
#include "proj_config.h"

#ifndef LOG_TAG
#define LOG_TAG "log"
#endif

#ifndef LOG_OUTPUT_LEVEL
#define LOG_OUTPUT_LEVEL 4
#endif

#ifndef LOG_TIMESTAMP_EN
#define LOG_TIMESTAMP_EN 0
#endif

#ifndef LOG_FUNCTION_EN
#define LOG_FUNCTION_EN 0
#endif

    /**
     * print level:
     **/

#define LOG_ASSERT 1
#define LOG_ERROR 2
#define LOG_WARINING 3
#define LOG_INFO 4
#define LOG_DEBUG 5

    extern char *LOG_LEVEL_TAGS[];

#if LOG_OUTPUT_MAX_LEN < 80
#error "LOG_OUTPUT_MAX_LEN >= 80"
#endif

#define __log_with_level(level, ...)                                                                                                                                \
    do                                                                                                                                                              \
    {                                                                                                                                                               \
        char log_str[LOG_OUTPUT_MAX_LEN];                                                                                                                           \
        int len = 0;                                                                                                                                                \
        if (DEBUG_LOG_SHELL_METHOD == DEBUG_RTT)                                                                                                                    \
        {                                                                                                                                                           \
            if ((level <= LOG_OUTPUT_LEVEL) && (level <= get_global_log_level()))                                                                                   \
            {                                                                                                                                                       \
                if (level == LOG_ERROR)                                                                                                                             \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_TEXT_BRIGHT_RED);                                                             \
                }                                                                                                                                                   \
                else                                                                                                                                                \
                {                                                                                                                                                   \
                    len = snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_RESET);                                                                        \
                }                                                                                                                                                   \
                if (LOG_TIMESTAMP_EN)                                                                                                                               \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_YELLOW "[%d.%03d]", (int)get_time_ms() / 1000, (int)get_time_ms() % 1000); \
                }                                                                                                                                                   \
                len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_MAGENTA "[%s]", LOG_LEVEL_TAGS[level]);                                        \
                len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_GREEN "[%s]", LOG_TAG);                                                        \
                if (LOG_FUNCTION_EN)                                                                                                                                \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_BLUE "[%s]", __FUNCTION__);                                                \
                }                                                                                                                                                   \
                if ((LOG_FILE_LINE_EN) && (level <= LOG_ERROR))                                                                                                     \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_BLACK "(%s,%d)", __FILE__, __LINE__);                                      \
                }                                                                                                                                                   \
                len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, RTT_CTRL_BG_BLACK);                                                                        \
                len += log_printf_to_buffer((char *)&log_str[len], LOG_OUTPUT_MAX_LEN - len, __VA_ARGS__);                                                          \
                if (strstr(log_str, "\r\n") == NULL)                                                                                                                \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "\r\n");                                                                               \
                }                                                                                                                                                   \
                __log_output((uint8_t *)log_str, len);                                                                                                              \
            }                                                                                                                                                       \
        }                                                                                                                                                           \
        else                                                                                                                                                        \
        {                                                                                                                                                           \
            if ((level <= LOG_OUTPUT_LEVEL) && (level <= get_global_log_level()))                                                                                   \
            {                                                                                                                                                       \
                if (LOG_TIMESTAMP_EN)                                                                                                                               \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "[%d.%03d]", (int)get_time_ms() / 1000, (int)get_time_ms() % 1000);                    \
                }                                                                                                                                                   \
                len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "[%s]", LOG_LEVEL_TAGS[level]);                                                            \
                len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "[%s]", LOG_TAG);                                                                          \
                if (LOG_FUNCTION_EN)                                                                                                                                \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "[%s]", __FUNCTION__);                                                                 \
                }                                                                                                                                                   \
                if ((LOG_FILE_LINE_EN) && (level <= LOG_ERROR))                                                                                                     \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "(%s,%d)", __FILE__, __LINE__);                                                        \
                }                                                                                                                                                   \
                len += log_printf_to_buffer((char *)&log_str[len], LOG_OUTPUT_MAX_LEN - len, __VA_ARGS__);                                                          \
                if (strstr(log_str, "\r\n") == NULL)                                                                                                                \
                {                                                                                                                                                   \
                    len += snprintf(&log_str[len], LOG_OUTPUT_MAX_LEN - len, "\r\n");                                                                               \
                }                                                                                                                                                   \
                __log_output((uint8_t *)log_str, len);                                                                                                              \
            }                                                                                                                                                       \
        }                                                                                                                                                           \
    } while (0)

#if defined(LOG_ASSERT_EN) && (LOG_ASSERT_EN == 1)
#define log_assert(...) __log_with_level(LOG_ASSERT, __VA_ARGS__)
#else
#define log_assert(...)
#endif

#if defined(LOG_ERROR_EN) && (LOG_ERROR_EN == 1)
#define log_e(...) __log_with_level(LOG_ERROR, __VA_ARGS__)
#else
#define log_e(...)
#endif

#if defined(LOG_WARINING_EN) && (LOG_WARINING_EN == 1)
#define log_w(...) __log_with_level(LOG_WARINING, __VA_ARGS__)
#else
#define log_w(...)
#endif

#if defined(LOG_INFO_EN) && (LOG_INFO_EN == 1)
#define log_i(...) __log_with_level(LOG_INFO, __VA_ARGS__)
#else
#define log_i(...)
#endif

#if defined(LOG_DEBUG_EN) && (LOG_DEBUG_EN == 1)
#define log_d(...) __log_with_level(LOG_DEBUG, __VA_ARGS__)
#else
#define log_d(...)
#endif

    void set_global_log_level(uint8_t level);
    uint8_t get_global_log_level(void);

    void log_printf(char *fmt, ...);
    int log_printf_to_buffer(char *buff, int size, char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
