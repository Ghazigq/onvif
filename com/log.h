#ifndef __LOG_H__
#define __LOG_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* output log's level */
typedef enum {
    LOG_LVL_ASSERT = 0,
    LOG_LVL_ERROR,
    LOG_LVL_WARN,
    LOG_LVL_INFO,
    LOG_LVL_DEBUG,
    LOG_LVL_VERBOSE,
    LOG_LVL_MAX,
} LOG_LEVEL;

/* the output silent level and all level for filter setting */
#define LOG_FILTER_LVL_SILENT LOG_LVL_ASSERT
#define LOG_FILTER_LVL_ALL LOG_LVL_VERBOSE

#define LOG_ASSERT(EXPR)                                                                           \
    if (!(EXPR)) {                                                                                 \
        if (log_assert_hook == NULL) {                                                             \
            log_assert("log", "(%s) has assert failed at %s:%ld.", #EXPR, __FUNCTION__, __LINE__); \
            while (1)                                                                              \
                ;                                                                                  \
        } else {                                                                                   \
            log_assert_hook(#EXPR, __FUNCTION__, __LINE__);                                        \
        }                                                                                          \
    }

#define LOG_CHECK(condition, action)                         \
    do {                                                     \
        if (condition) {                                     \
            log_assert(LOG_TAG, "check [" #condition "]\n"); \
            action;                                          \
        }                                                    \
    } while (0)

#define log_assert(tag, ...) \
    log_output(LOG_LVL_ASSERT, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_error(tag, ...) \
    log_output(LOG_LVL_ERROR, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_warn(tag, ...) \
    log_output(LOG_LVL_WARN, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_info(tag, ...) \
    log_output(LOG_LVL_INFO, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_debug(tag, ...) \
    log_output(LOG_LVL_DEBUG, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define log_verbose(tag, ...) \
    log_output(LOG_LVL_VERBOSE, tag, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

extern void (*log_assert_hook)(const char* expr, const char* func, size_t line);
extern void log_output(uint8_t level, const char* tag, const char* file, const char* func,
                       const long line, const char* format, ...);

#if !defined(LOG_TAG)
#define LOG_TAG "NO_TAG"
#endif

#if !defined(LOG_LVL)
#define LOG_LVL LOG_LVL_VERBOSE
#endif
#if LOG_LVL >= LOG_LVL_ASSERT
#define log_a(...) log_assert(LOG_TAG, __VA_ARGS__)
#else
#define log_a(...) ((void)0);
#endif
#if LOG_LVL >= LOG_LVL_ERROR
#define log_e(...) log_error(LOG_TAG, __VA_ARGS__)
#else
#define log_e(...) ((void)0);
#endif
#if LOG_LVL >= LOG_LVL_WARN
#define log_w(...) log_warn(LOG_TAG, __VA_ARGS__)
#else
#define log_w(...) ((void)0);
#endif
#if LOG_LVL >= LOG_LVL_INFO
#define log_i(...) log_info(LOG_TAG, __VA_ARGS__)
#else
#define log_i(...) ((void)0);
#endif
#if LOG_LVL >= LOG_LVL_DEBUG
#define log_d(...) log_debug(LOG_TAG, __VA_ARGS__)
#else
#define log_d(...) ((void)0);
#endif
#if LOG_LVL >= LOG_LVL_VERBOSE
#define log_v(...) log_verbose(LOG_TAG, __VA_ARGS__)
#else
#define log_v(...) ((void)0);
#endif

void log_set_output_enabled(bool enabled);
void log_raw(const char* format, ...);
void log_hexdump(const char* name, uint8_t width, uint8_t* buf, uint16_t size);
void log_assert_set_hook(void (*hook)(const char* expr, const char* func, size_t line));

void log_set_file_output_enabled(bool enabled);
void log_set_file_name(const char* name); /* name set before file_output enable */

void log_set_filter(uint8_t level, const char* tag, const char* keyword);
void log_set_filter_lvl(uint8_t level);
void log_set_filter_tag(const char* tag);
void log_set_filter_kw(const char* keyword);
void log_set_filter_tag_lvl(const char* tag, uint8_t level);
int log_get_filter_tag_lvl(const char* tag);
int log_find_lvl(const char* log);
const char* log_find_tag(const char* log, uint8_t lvl, size_t* tag_len);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
