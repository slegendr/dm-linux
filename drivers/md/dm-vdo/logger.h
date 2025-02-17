/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2023 Red Hat
 */

#ifndef UDS_LOGGER_H
#define UDS_LOGGER_H

#include <linux/kern_levels.h>
#include <linux/module.h>
#include <linux/ratelimit.h>
#include <linux/device-mapper.h>

/* Custom logging utilities for UDS */

enum {
	UDS_LOG_EMERG = LOGLEVEL_EMERG,
	UDS_LOG_ALERT = LOGLEVEL_ALERT,
	UDS_LOG_CRIT = LOGLEVEL_CRIT,
	UDS_LOG_ERR = LOGLEVEL_ERR,
	UDS_LOG_WARNING = LOGLEVEL_WARNING,
	UDS_LOG_NOTICE = LOGLEVEL_NOTICE,
	UDS_LOG_INFO = LOGLEVEL_INFO,
	UDS_LOG_DEBUG = LOGLEVEL_DEBUG,

	UDS_LOG_MAX = UDS_LOG_DEBUG,
	UDS_LOG_DEFAULT = UDS_LOG_INFO,
};

extern int vdo_log_level;

#define DM_MSG_PREFIX "vdo"
#define UDS_LOGGING_MODULE_NAME DM_NAME ": " DM_MSG_PREFIX

/* Apply a rate limiter to a log method call. */
#define uds_log_ratelimit(log_fn, ...)                                    \
	do {                                                              \
		static DEFINE_RATELIMIT_STATE(_rs,                        \
					      DEFAULT_RATELIMIT_INTERVAL, \
					      DEFAULT_RATELIMIT_BURST);   \
		if (__ratelimit(&_rs)) {                                  \
			log_fn(__VA_ARGS__);                              \
		}                                                         \
	} while (0)

int uds_get_log_level(void);

void uds_log_embedded_message(int priority, const char *module, const char *prefix,
			      const char *fmt1, va_list args1, const char *fmt2, ...)
	__printf(4, 0) __printf(6, 7);

void uds_log_backtrace(int priority);

/* All log functions will preserve the caller's value of errno. */

#define uds_log_strerror(priority, errnum, ...) \
	__uds_log_strerror(priority, errnum, UDS_LOGGING_MODULE_NAME, __VA_ARGS__)

int __uds_log_strerror(int priority, int errnum, const char *module,
		       const char *format, ...)
	__printf(4, 5);

int uds_vlog_strerror(int priority, int errnum, const char *module, const char *format,
		      va_list args)
	__printf(4, 0);

/* Log an error prefixed with the string associated with the errnum. */
#define uds_log_error_strerror(errnum, ...) \
	uds_log_strerror(UDS_LOG_ERR, errnum, __VA_ARGS__)

#define uds_log_debug_strerror(errnum, ...) \
	uds_log_strerror(UDS_LOG_DEBUG, errnum, __VA_ARGS__)

#define uds_log_info_strerror(errnum, ...) \
	uds_log_strerror(UDS_LOG_INFO, errnum, __VA_ARGS__)

#define uds_log_warning_strerror(errnum, ...) \
	uds_log_strerror(UDS_LOG_WARNING, errnum, __VA_ARGS__)

#define uds_log_fatal_strerror(errnum, ...) \
	uds_log_strerror(UDS_LOG_CRIT, errnum, __VA_ARGS__)

#define uds_log_message(priority, ...) \
	__uds_log_message(priority, UDS_LOGGING_MODULE_NAME, __VA_ARGS__)

void __uds_log_message(int priority, const char *module, const char *format, ...)
	__printf(3, 4);

#define uds_log_debug(...) uds_log_message(UDS_LOG_DEBUG, __VA_ARGS__)

#define uds_log_info(...) uds_log_message(UDS_LOG_INFO, __VA_ARGS__)

#define uds_log_warning(...) uds_log_message(UDS_LOG_WARNING, __VA_ARGS__)

#define uds_log_error(...) uds_log_message(UDS_LOG_ERR, __VA_ARGS__)

#define uds_log_fatal(...) uds_log_message(UDS_LOG_CRIT, __VA_ARGS__)

void uds_pause_for_logger(void);
#endif /* UDS_LOGGER_H */
