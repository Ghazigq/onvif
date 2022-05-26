
#define LOG_TAG "log"
#define LOG_LVL LOG_LVL_VERBOSE

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "log.h"

#ifdef linux
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

/* all formats index */
typedef enum {
    LOG_FMT_LVL    = 1 << 0, /**< level */
    LOG_FMT_TAG    = 1 << 1, /**< tag */
    LOG_FMT_TIME   = 1 << 2, /**< current time */
    LOG_FMT_P_INFO = 1 << 3, /**< process info */
    LOG_FMT_T_INFO = 1 << 4, /**< thread info */
    LOG_FMT_DIR    = 1 << 5, /**< file directory and name */
    LOG_FMT_FUNC   = 1 << 6, /**< function name */
    LOG_FMT_LINE   = 1 << 7, /**< line number */
} LOG_FMT;

/* macro definition for all formats */
#define LOG_FMT_ALL                                                                             \
    (LOG_FMT_LVL | LOG_FMT_TAG | LOG_FMT_TIME | LOG_FMT_P_INFO | LOG_FMT_T_INFO | LOG_FMT_DIR | \
     LOG_FMT_FUNC | LOG_FMT_LINE)

/* buffer size for every line's log */
#define LOG_LINE_BUF_SIZE 1024
/* output line number max length */
#define LOG_LINE_NUM_MAX_LEN 5
/* output filter's tag max length */
#define LOG_FILTER_TAG_MAX_LEN 16
/* output filter's keyword max length */
#define LOG_FILTER_KW_MAX_LEN 16
/* output filter's tag level max num */
#define LOG_FILTER_TAG_LVL_MAX_NUM 5
/* EasyLogger file log plugin's using max rotate file count */
#define LOG_FILE_MAX_ROTATE 3
/* EasyLogger file log plugin's using file max size */
#define LOG_FILE_MAX_SIZE (10 * 1024)

/* output newline sign */
#define LOG_NEWLINE_SIGN "\n"

#define CSI_START "\033["
#define CSI_END "\033[0m"

/* output log front color */
#define F_BLACK "30;"
#define F_RED "31;"
#define F_GREEN "32;"
#define F_YELLOW "33;"
#define F_BLUE "34;"
#define F_MAGENTA "35;"
#define F_CYAN "36;"
#define F_WHITE "37;"
/* output log background color */
#define B_NULL
#define B_BLACK "40;"
#define B_RED "41;"
#define B_GREEN "42;"
#define B_YELLOW "43;"
#define B_BLUE "44;"
#define B_MAGENTA "45;"
#define B_CYAN "46;"
#define B_WHITE "47;"
/* output log fonts style */
#define S_BOLD "1m"
#define S_UNDERLINE "4m"
#define S_BLINK "5m"
#define S_NORMAL "22m"

/* output log default color definition: [front color] + [background color] + [show style] */
#define LOG_COLOR_ASSERT (F_MAGENTA B_NULL S_NORMAL)
#define LOG_COLOR_ERROR (F_RED B_NULL S_NORMAL)
#define LOG_COLOR_WARN (F_YELLOW B_NULL S_NORMAL)
#define LOG_COLOR_INFO (F_CYAN B_NULL S_NORMAL)
#define LOG_COLOR_DEBUG (F_GREEN B_NULL S_NORMAL)
#define LOG_COLOR_VERBOSE (F_BLUE B_NULL S_NORMAL)

/* output log's tag filter */
typedef struct {
    uint8_t level;
    char tag[LOG_FILTER_TAG_MAX_LEN + 1];
    bool tag_use_flag; /**< false : tag is no used   true: tag is used */
} log_tag_lvl_filter_t;

/* output log's filter */
typedef struct {
    uint8_t level;
    char tag[LOG_FILTER_TAG_MAX_LEN + 1];
    char keyword[LOG_FILTER_KW_MAX_LEN + 1];
    log_tag_lvl_filter_t tag_lvl[LOG_FILTER_TAG_LVL_MAX_NUM];
} log_filter_t;

/* easy logger */
typedef struct {
    log_filter_t filter;
    size_t enabled_fmt_set[LOG_LVL_MAX];
    bool init_ok;
    bool output_enabled;
    bool text_color_enabled;
    /* file */
    char* name;      /* file name */
    FILE* fp;        /* file descriptor */
    size_t max_size; /* file max size */
    int max_rotate;  /* max rotate file count */
} log_t;

/* log */
static log_t g_log = {
    .filter =
        {
            .level = LOG_LVL_VERBOSE,
        },
    .enabled_fmt_set =
        {
            LOG_FMT_ALL & ~LOG_FMT_P_INFO & ~LOG_FMT_T_INFO,        /* LOG_LVL_ASSERT */
            LOG_FMT_LVL | LOG_FMT_TAG | LOG_FMT_TIME | LOG_FMT_DIR, /* LOG_LVL_ERROR */
            LOG_FMT_LVL | LOG_FMT_TAG | LOG_FMT_TIME | LOG_FMT_DIR, /* LOG_LVL_WARN */
            LOG_FMT_LVL | LOG_FMT_TAG | LOG_FMT_TIME,               /* LOG_LVL_INFO */
            LOG_FMT_ALL & ~LOG_FMT_P_INFO & ~LOG_FMT_T_INFO,        /* LOG_LVL_DEBUG */
            LOG_FMT_ALL,                                            /* LOG_LVL_VERBOSE */
        },
    .output_enabled     = true,
    .text_color_enabled = true,
    .max_size           = LOG_FILE_MAX_SIZE,
    .max_rotate         = LOG_FILE_MAX_ROTATE,
};
/* every line log's buffer */
static char log_buf[LOG_LINE_BUF_SIZE] = {0};
/* level output info */
static const char* level_output_info[] = {
    [LOG_LVL_ASSERT] = "A/", [LOG_LVL_ERROR] = "E/", [LOG_LVL_WARN] = "W/",
    [LOG_LVL_INFO] = "I/",   [LOG_LVL_DEBUG] = "D/", [LOG_LVL_VERBOSE] = "V/",
};

/* color output info */
static const char* color_output_info[] = {
    [LOG_LVL_ASSERT] = LOG_COLOR_ASSERT, [LOG_LVL_ERROR] = LOG_COLOR_ERROR,
    [LOG_LVL_WARN] = LOG_COLOR_WARN,     [LOG_LVL_INFO] = LOG_COLOR_INFO,
    [LOG_LVL_DEBUG] = LOG_COLOR_DEBUG,   [LOG_LVL_VERBOSE] = LOG_COLOR_VERBOSE,
};

void (*log_assert_hook)(const char* expr, const char* func, size_t line);

/* log */
static bool get_fmt_enabled(uint8_t level, size_t set);

/* port */
static int log_init(void);
static int log_port_init(void);
static void log_port_output(const char* log, size_t size);
static void log_port_output_lock(void);
static void log_port_output_unlock(void);
static const char* log_port_get_time(void);
static const char* log_port_get_p_info(void);
static const char* log_port_get_t_info(void);

static int log_file_port_init(void);
static void inline log_file_port_lock(void);
static void inline log_file_port_unlock(void);
static void log_file_port_deinit(void);

/**
 * another copy string function
 *
 * @param cur_len current copied log length, max size is LOG_LINE_BUF_SIZE
 * @param dst destination
 * @param src source
 *
 * @return copied length
 */
static size_t log_strcpy(size_t cur_len, char* dst, const char* src) {
    const char* src_old = src;

    if (!dst || !src) return 0;

    while (*src != 0) {
        /* make sure destination has enough space */
        if (cur_len++ < LOG_LINE_BUF_SIZE) {
            *dst++ = *src++;
        } else {
            break;
        }
    }
    return src - src_old;
}

static int log_file_init(void) {
    log_file_port_init();

    log_file_port_lock();

    g_log.fp = fopen(g_log.name, "a+");

    log_file_port_unlock();
    return 0;
}

/*
 * rotate the log file xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0
 */
static bool log_file_rotate(void) {
#define SUFFIX_LEN 10
    /* mv xxx.log.n-1 => xxx.log.n, and xxx.log => xxx.log.0 */
    int n, err = 0;
    char oldpath[256], newpath[256];
    size_t base = strlen(g_log.name);
    bool result = true;
    FILE* tmp_fp;

    memcpy(oldpath, g_log.name, base);
    memcpy(newpath, g_log.name, base);

    fclose(g_log.fp);

    for (n = g_log.max_rotate - 1; n >= 0; --n) {
        snprintf(oldpath + base, SUFFIX_LEN, n ? ".%d" : "", n - 1);
        snprintf(newpath + base, SUFFIX_LEN, ".%d", n);
        /* remove the old file */
        if ((tmp_fp = fopen(newpath, "r")) != NULL) {
            fclose(tmp_fp);
            remove(newpath);
        }
        /* change the new log file to old file name */
        if ((tmp_fp = fopen(oldpath, "r")) != NULL) {
            fclose(tmp_fp);
            err = rename(oldpath, newpath);
        }

        if (err < 0) {
            result = false;
            goto __exit;
        }
    }

__exit:
    /* reopen the file */
    g_log.fp = fopen(g_log.name, "a+");

    return result;
}

static void log_file_write(const char* log, size_t size) {
    size_t file_size = 0;

    LOG_CHECK(log == NULL, return;);

    log_file_port_lock();

    if (g_log.fp == NULL) goto __exit;

    fseek(g_log.fp, 0L, SEEK_END);
    file_size = ftell(g_log.fp);

    if (unlikely(file_size > g_log.max_size)) {
#if LOG_FILE_MAX_ROTATE > 0
        if (!log_file_rotate()) {
            goto __exit;
        }
#else
        goto __exit;
#endif
    }

    fwrite(log, size, 1, g_log.fp);

    fflush(g_log.fp);

__exit:
    log_file_port_unlock();
}

static void log_file_deinit(void) {
    log_file_port_lock();

    if (g_log.fp) {
        fclose(g_log.fp);
        g_log.fp = NULL;
    }

    log_file_port_unlock();

    log_file_port_deinit();
}

/**
 * EasyLogger initialize.
 *
 * @return result
 */
int log_init(void) {
    int ret = 0;

    /* port initialize */
    if ((ret = log_port_init()) != 0) return ret;

    /* close printf buffer */
    setbuf(stdout, NULL);

    g_log.init_ok = true;

    return ret;
}

/**
 * set output enable or disable
 *
 * @param enabled TRUE: enable FALSE: disable
 */
void log_set_output_enabled(bool enabled) { g_log.output_enabled = enabled; }

/**
 * set log file output enable or disable
 *
 * @param enabled TRUE: enable FALSE: disable
 */
void log_set_file_output_enabled(bool enabled)
{
    LOG_CHECK(g_log.name == NULL, return );

    if (enabled && g_log.fp == NULL)
        log_file_init();
    else if (g_log.fp != NULL)
        log_file_deinit();
}

/**
 * set log file name
 *
 * @param name log file name
 */
void log_set_file_name(const char* name) {
    LOG_CHECK(name == NULL || strlen(name) == 0, return );
    LOG_CHECK(g_log.fp != NULL, return );

    g_log.name = (char*)name;
}

/**
 * set log filter all parameter
 *
 * @param level level
 * @param tag tag
 * @param keyword keyword
 */
void log_set_filter(uint8_t level, const char* tag, const char* keyword) {
    LOG_CHECK(level > LOG_LVL_VERBOSE, return;);

    log_set_filter_lvl(level);
    log_set_filter_tag(tag);
    log_set_filter_kw(keyword);
}

/**
 * set log filter's level
 *
 * @param level level
 */
void log_set_filter_lvl(uint8_t level) {
    LOG_CHECK(level > LOG_LVL_VERBOSE, return;);

    g_log.filter.level = level;
}

/**
 * set log filter's tag
 *
 * @param tag tag
 */
void log_set_filter_tag(const char* tag) { strncpy(g_log.filter.tag, tag, LOG_FILTER_TAG_MAX_LEN); }

/**
 * set log filter's keyword
 *
 * @param keyword keyword
 */
void log_set_filter_kw(const char* keyword) {
    strncpy(g_log.filter.keyword, keyword, LOG_FILTER_KW_MAX_LEN);
}

/**
 * Set the filter's level by different tag.
 * The log on this tag which level is less than it will stop output.
 *
 * example:
 *     // the example tag log enter silent mode
 *     log_set_filter_tag_lvl("example", LOG_FILTER_LVL_SILENT);
 *     // the example tag log which level is less than INFO level will stop output
 *     log_set_filter_tag_lvl("example", LOG_LVL_INFO);
 *     // remove example tag's level filter, all level log will resume output
 *     log_set_filter_tag_lvl("example", LOG_FILTER_LVL_ALL);
 *
 * @param tag log tag
 * @param level The filter level. When the level is LOG_FILTER_LVL_SILENT, the log enter silent
 * mode. When the level is LOG_FILTER_LVL_ALL, it will remove this tag's level filer. Then all level
 * log will resume output.
 *
 */
void log_set_filter_tag_lvl(const char* tag, uint8_t level) {
    LOG_CHECK(level > LOG_LVL_VERBOSE, return;);
    LOG_CHECK(tag == NULL, return;);
    uint8_t i = 0;

    if (!g_log.init_ok) {
        log_init();
    }

    log_port_output_lock();
    /* find the tag in arr */
    for (i = 0; i < LOG_FILTER_TAG_LVL_MAX_NUM; i++) {
        if (g_log.filter.tag_lvl[i].tag_use_flag == true &&
            !strncmp(tag, g_log.filter.tag_lvl[i].tag, LOG_FILTER_TAG_MAX_LEN)) {
            break;
        }
    }

    if (i < LOG_FILTER_TAG_LVL_MAX_NUM) {
        /* find OK */
        if (level == LOG_FILTER_LVL_ALL) {
            /* remove current tag's level filter when input level is the lowest level */
            g_log.filter.tag_lvl[i].tag_use_flag = false;
            memset(g_log.filter.tag_lvl[i].tag, '\0', LOG_FILTER_TAG_MAX_LEN + 1);
            g_log.filter.tag_lvl[i].level = LOG_FILTER_LVL_SILENT;
        } else {
            g_log.filter.tag_lvl[i].level = level;
        }
    } else {
        /* only add the new tag's level filer when level is not LOG_FILTER_LVL_ALL */
        if (level != LOG_FILTER_LVL_ALL) {
            for (i = 0; i < LOG_FILTER_TAG_LVL_MAX_NUM; i++) {
                if (g_log.filter.tag_lvl[i].tag_use_flag == false) {
                    strncpy(g_log.filter.tag_lvl[i].tag, tag, LOG_FILTER_TAG_MAX_LEN);
                    g_log.filter.tag_lvl[i].level        = level;
                    g_log.filter.tag_lvl[i].tag_use_flag = true;
                    break;
                }
            }
        }
    }
    log_port_output_unlock();
}

/**
 * get the level on tag's level filer
 *
 * @param tag tag
 *
 * @return It will return the lowest level when tag was not found.
 *         Other level will return when tag was found.
 */
int log_get_filter_tag_lvl(const char* tag) {
    LOG_CHECK(tag == NULL, return -1;);
    uint8_t i     = 0;
    uint8_t level = LOG_FILTER_LVL_ALL;

    if (!g_log.init_ok) {
        log_init();
    }

    log_port_output_lock();
    /* find the tag in arr */
    for (i = 0; i < LOG_FILTER_TAG_LVL_MAX_NUM; i++) {
        if (g_log.filter.tag_lvl[i].tag_use_flag == true &&
            !strncmp(tag, g_log.filter.tag_lvl[i].tag, LOG_FILTER_TAG_MAX_LEN)) {
            level = g_log.filter.tag_lvl[i].level;
            break;
        }
    }
    log_port_output_unlock();

    return level;
}

/**
 * output RAW format log
 *
 * @param format output format
 * @param ... args
 */
void log_raw(const char* format, ...) {
    va_list args;
    size_t log_len = 0;
    int fmt_result;

    if (!g_log.init_ok) {
        log_init();
    }

    /* check output enabled */
    if (!g_log.output_enabled) {
        return;
    }

    /* args point to the first variable parameter */
    va_start(args, format);

    /* lock output */
    log_port_output_lock();

    /* package log data to buffer */
    fmt_result = vsnprintf(log_buf, LOG_LINE_BUF_SIZE, format, args);

    /* output converted log */
    if ((fmt_result > -1) && (fmt_result <= LOG_LINE_BUF_SIZE)) {
        log_len = fmt_result;
    } else {
        log_len = LOG_LINE_BUF_SIZE;
    }
    /* output log */
    log_port_output(log_buf, log_len);

    /* write the file */
    log_file_write(log_buf, log_len);

    /* unlock output */
    log_port_output_unlock();

    va_end(args);
}

/**
 * output the log
 *
 * @param level level
 * @param tag tag
 * @param file file name
 * @param func function name
 * @param line line number
 * @param format output format
 * @param ... args
 *
 */
void log_output(uint8_t level, const char* tag, const char* file, const char* func, const long line,
                const char* format, ...) {
    size_t tag_len = strlen(tag), newline_len = strlen(LOG_NEWLINE_SIGN);
    int log_len                                    = 0;
    char line_num[LOG_LINE_NUM_MAX_LEN + 1]        = {0};
    char tag_sapce[LOG_FILTER_TAG_MAX_LEN / 2 + 1] = {0};
    va_list args;
    int fmt_result;

    LOG_CHECK(level > LOG_LVL_VERBOSE, return;);

    if (!g_log.init_ok) {
        log_init();
    }

    /* check output enabled */
    if (!g_log.output_enabled) {
        return;
    }
    /* level filter */
    if (level > g_log.filter.level || level > log_get_filter_tag_lvl(tag)) {
        return;
    } else if (!strstr(tag, g_log.filter.tag)) { /* tag filter */
        return;
    }
    /* args point to the first variable parameter */
    va_start(args, format);
    /* lock output */
    log_port_output_lock();

    /* add CSI start sign and color info */
    if (g_log.text_color_enabled) {
        log_len += log_strcpy(log_len, log_buf + log_len, CSI_START);
        log_len += log_strcpy(log_len, log_buf + log_len, color_output_info[level]);
    }

    /* package level info */
    if (get_fmt_enabled(level, LOG_FMT_LVL)) {
        log_len += log_strcpy(log_len, log_buf + log_len, level_output_info[level]);
    }
    /* package tag info */
    if (get_fmt_enabled(level, LOG_FMT_TAG)) {
        log_len += log_strcpy(log_len, log_buf + log_len, tag);
        /* if the tag length is less than 50% LOG_FILTER_TAG_MAX_LEN, then fill space */
        if (tag_len <= LOG_FILTER_TAG_MAX_LEN / 2) {
            memset(tag_sapce, ' ', LOG_FILTER_TAG_MAX_LEN / 2 - tag_len);
            log_len += log_strcpy(log_len, log_buf + log_len, tag_sapce);
        }
        log_len += log_strcpy(log_len, log_buf + log_len, " ");
    }
    /* package time, process and thread info */
    if (get_fmt_enabled(level, LOG_FMT_TIME | LOG_FMT_P_INFO | LOG_FMT_T_INFO)) {
        log_len += log_strcpy(log_len, log_buf + log_len, "[");
        /* package time info */
        if (get_fmt_enabled(level, LOG_FMT_TIME)) {
            log_len += log_strcpy(log_len, log_buf + log_len, log_port_get_time());
            if (get_fmt_enabled(level, LOG_FMT_P_INFO | LOG_FMT_T_INFO)) {
                log_len += log_strcpy(log_len, log_buf + log_len, " ");
            }
        }
        /* package process info */
        if (get_fmt_enabled(level, LOG_FMT_P_INFO)) {
            log_len += log_strcpy(log_len, log_buf + log_len, log_port_get_p_info());
            if (get_fmt_enabled(level, LOG_FMT_T_INFO)) {
                log_len += log_strcpy(log_len, log_buf + log_len, " ");
            }
        }
        /* package thread info */
        if (get_fmt_enabled(level, LOG_FMT_T_INFO)) {
            log_len += log_strcpy(log_len, log_buf + log_len, log_port_get_t_info());
        }
        log_len += log_strcpy(log_len, log_buf + log_len, "] ");
    }
    /* package file directory and name, function name and line number info */
    if (get_fmt_enabled(level, LOG_FMT_DIR | LOG_FMT_FUNC | LOG_FMT_LINE)) {
        log_len += log_strcpy(log_len, log_buf + log_len, "(");
        /* package file info */
        if (get_fmt_enabled(level, LOG_FMT_DIR)) {
            log_len += log_strcpy(log_len, log_buf + log_len, file);
            if (get_fmt_enabled(level, LOG_FMT_FUNC)) {
                log_len += log_strcpy(log_len, log_buf + log_len, ":");
            } else if (get_fmt_enabled(level, LOG_FMT_LINE)) {
                log_len += log_strcpy(log_len, log_buf + log_len, " ");
            }
        }
        /* package line info */
        if (get_fmt_enabled(level, LOG_FMT_LINE)) {
            snprintf(line_num, LOG_LINE_NUM_MAX_LEN, "%ld", line);
            log_len += log_strcpy(log_len, log_buf + log_len, line_num);
            if (get_fmt_enabled(level, LOG_FMT_FUNC)) {
                log_len += log_strcpy(log_len, log_buf + log_len, " ");
            }
        }
        /* package func info */
        if (get_fmt_enabled(level, LOG_FMT_FUNC)) {
            log_len += log_strcpy(log_len, log_buf + log_len, func);
        }
        log_len += log_strcpy(log_len, log_buf + log_len, ")");
    }
    /* package other log data to buffer. '\0' must be added in the end by vsnprintf. */
    fmt_result = vsnprintf(log_buf + log_len, LOG_LINE_BUF_SIZE - log_len, format, args);

    va_end(args);
    /* calculate log length */
    if ((log_len + fmt_result <= LOG_LINE_BUF_SIZE) && (fmt_result > -1)) {
        log_len += fmt_result;
    } else {
        /* using max length */
        log_len = LOG_LINE_BUF_SIZE;
    }
    /* overflow check and reserve some space for CSI end sign and newline sign */
    if (log_len + (sizeof(CSI_END) - 1) + newline_len > LOG_LINE_BUF_SIZE) {
        /* using max length */
        log_len = LOG_LINE_BUF_SIZE;
        /* reserve some space for CSI end sign */
        log_len -= (sizeof(CSI_END) - 1);
        /* reserve some space for newline sign */
        log_len -= newline_len;
    }
    /* keyword filter */
    if (g_log.filter.keyword[0] != '\0') {
        /* add string end sign */
        log_buf[log_len] = '\0';
        /* find the keyword */
        if (!strstr(log_buf, g_log.filter.keyword)) {
            /* unlock output */
            log_port_output_unlock();
            return;
        }
    }

    /* add CSI end sign */
    if (g_log.text_color_enabled) {
        log_len += log_strcpy(log_len, log_buf + log_len, CSI_END);
    }

    /* package newline sign */
    log_len += log_strcpy(log_len, log_buf + log_len, LOG_NEWLINE_SIGN);
    /* output log */
    log_port_output(log_buf, log_len);

    /* write the file */
    log_file_write(log_buf, log_len);

    /* unlock output */
    log_port_output_unlock();
}

/**
 * get format enabled
 *
 * @param level level
 * @param set format set
 *
 * @return enable or disable
 */
static bool get_fmt_enabled(uint8_t level, size_t set) {
    LOG_CHECK(level > LOG_LVL_VERBOSE, return false);

    if (g_log.enabled_fmt_set[level] & set) {
        return true;
    } else {
        return false;
    }
}

/**
 * Set a hook function to EasyLogger assert. It will run when the expression is false.
 *
 * @param hook the hook function
 */
void log_assert_set_hook(void (*hook)(const char* expr, const char* func, size_t line)) {
    log_assert_hook = hook;
}

/**
 * find the log level
 * @note make sure the log level is output on each format
 *
 * @param log log buffer
 *
 * @return log level, found failed will return -1
 */
int log_find_lvl(const char* log) {
    LOG_CHECK(log == NULL, return -1;);
    /* make sure the log level is output on each format */
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_ASSERT] & LOG_FMT_LVL) == 0, return -1;);
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_ERROR] & LOG_FMT_LVL) == 0, return -1;);
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_WARN] & LOG_FMT_LVL) == 0, return -1;);
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_INFO] & LOG_FMT_LVL) == 0, return -1;);
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_DEBUG] & LOG_FMT_LVL) == 0, return -1;);
    LOG_CHECK((g_log.enabled_fmt_set[LOG_LVL_VERBOSE] & LOG_FMT_LVL) == 0, return -1;);

    uint8_t i;
    size_t csi_start_len = strlen(CSI_START);
    for (i = 0; i < LOG_LVL_MAX; i++) {
        if (!strncmp(color_output_info[i], log + csi_start_len, strlen(color_output_info[i]))) {
            return i;
        }
    }
    /* found failed */
    return -1;
}

/**
 * find the log tag
 * @note make sure the log tag is output on each format
 * @note the tag don't have space in it
 *
 * @param log log buffer
 * @param lvl log level, you can get it by @see log_find_lvl
 * @param tag_len found tag length
 *
 * @return log tag, found failed will return NULL
 */
const char* log_find_tag(const char* log, uint8_t lvl, size_t* tag_len) {
    const char *tag = NULL, *tag_end = NULL;

    LOG_CHECK(log == NULL, return NULL;);
    LOG_CHECK(tag_len == NULL, return NULL;);
    LOG_CHECK(lvl >= LOG_LVL_MAX, return NULL;);
    /* make sure the log tag is output on each format */
    LOG_CHECK((g_log.enabled_fmt_set[lvl] & LOG_FMT_TAG) == 0, return NULL;);

    tag = log + strlen(CSI_START) + strlen(color_output_info[lvl]) + strlen(level_output_info[lvl]);
    /* find the first space after tag */
    if ((tag_end = memchr(tag, ' ', LOG_FILTER_TAG_MAX_LEN)) != NULL) {
        *tag_len = tag_end - tag;
    } else {
        tag = NULL;
    }

    return tag;
}

/**
 * dump the hex format data to log
 *
 * @param name name for hex object, it will show on log header
 * @param width hex number for every line, such as: 16, 32
 * @param buf hex buffer
 * @param size buffer size
 */
void log_hexdump(const char* name, uint8_t width, uint8_t* buf, uint16_t size) {
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')

    int i, j;
    int log_len         = 0;
    char dump_string[8] = {0};
    int fmt_result;

    if (!g_log.init_ok) {
        log_init();
    }

    if (!g_log.output_enabled) {
        return;
    }

    /* level filter */
    if (LOG_LVL_DEBUG > g_log.filter.level) {
        return;
    } else if (!strstr(name, g_log.filter.tag)) { /* tag filter */
        return;
    }

    /* lock output */
    log_port_output_lock();

    for (i = 0; i < size; i += width) {
        /* package header */
        fmt_result =
            snprintf(log_buf, LOG_LINE_BUF_SIZE, "D/HEX %s: %04X-%04X: ", name, i, i + width - 1);
        /* calculate log length */
        if ((fmt_result > -1) && (fmt_result <= LOG_LINE_BUF_SIZE)) {
            log_len = fmt_result;
        } else {
            log_len = LOG_LINE_BUF_SIZE;
        }
        /* dump hex */
        for (j = 0; j < width; j++) {
            if (i + j < size) {
                snprintf(dump_string, sizeof(dump_string), "%02X ", buf[i + j]);
            } else {
                strncpy(dump_string, "   ", sizeof(dump_string));
            }
            log_len += log_strcpy(log_len, log_buf + log_len, dump_string);
            if ((j + 1) % 8 == 0) {
                log_len += log_strcpy(log_len, log_buf + log_len, " ");
            }
        }
        log_len += log_strcpy(log_len, log_buf + log_len, "  ");
        /* dump char for hex */
        for (j = 0; j < width; j++) {
            if (i + j < size) {
                snprintf(dump_string, sizeof(dump_string), "%c",
                         __is_print(buf[i + j]) ? buf[i + j] : '.');
                log_len += log_strcpy(log_len, log_buf + log_len, dump_string);
            }
        }
        /* overflow check and reserve some space for newline sign */
        if (log_len + strlen(LOG_NEWLINE_SIGN) > LOG_LINE_BUF_SIZE) {
            log_len = LOG_LINE_BUF_SIZE - strlen(LOG_NEWLINE_SIGN);
        }
        /* package newline sign */
        log_len += log_strcpy(log_len, log_buf + log_len, LOG_NEWLINE_SIGN);
        /* do log output */
        log_port_output(log_buf, log_len);

        /* write the file */
        log_file_write(log_buf, log_len);
    }
    /* unlock output */
    log_port_output_unlock();
}

/* log port */
static pthread_mutex_t output_lock = PTHREAD_MUTEX_INITIALIZER;

/* log port initialize */
static int log_port_init(void) { return 0; }

/* output log */
static void log_port_output(const char* log, size_t size) { printf("%.*s", (int)size, log); }

/* output lock */
static void log_port_output_lock(void) { pthread_mutex_lock(&output_lock); }

/* output unlock */
static void log_port_output_unlock(void) { pthread_mutex_unlock(&output_lock); }

/* current time */
static const char* log_port_get_time(void) {
    static char cur_system_time[32] = {0};

    time_t cur_t;
    struct tm cur_tm;
    struct timeval tv;
    char system_time[24] = {0};

    gettimeofday(&tv, NULL);
    cur_t = tv.tv_sec;
    localtime_r(&cur_t, &cur_tm);

    strftime(system_time, sizeof(system_time), "%Y-%m-%d %T", &cur_tm);
    snprintf(cur_system_time, sizeof(cur_system_time), "%s-%03ld", system_time, tv.tv_usec / 1000);

    return cur_system_time;
}

/* current process name */
static const char* log_port_get_p_info(void) {
    static char cur_process_info[10] = {0};

    snprintf(cur_process_info, 10, "pid:%04d", getpid());

    return cur_process_info;
}

/* current thread name */
static const char* log_port_get_t_info(void) {
    static char cur_thread_info[10] = {0};

    snprintf(cur_thread_info, 10, "tid:%04ld", pthread_self());

    return cur_thread_info;
}

/* file port */
static pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;

/*  file port initialize */
static int log_file_port_init(void) { return 0; }

/* file log lock */
static void inline log_file_port_lock(void) { pthread_mutex_lock(&file_lock); }

/* file log unlock */
static void inline log_file_port_unlock(void) { pthread_mutex_unlock(&file_lock); }

/* file log deinit */
static void log_file_port_deinit(void) {}
