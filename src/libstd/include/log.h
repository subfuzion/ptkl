/*
 * ptkl - Partikle Runtime
 *
 * MIT License
 *
 * Copyright (c) 2025 Tony Pujals
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/**
 * Use the log MACROS defined at the end instead of the log
 * functions directly for extra detail AND conditional
 * logging support based on LOG_LEVEL. The extra detail
 * provides: {filename}:{line}: {function}.
 *
 * For the macros to work, define a log level before using
 * them. For example:
 *
 *     #define LOG_LEVEL LOG_LEVEL_TRACE
 *
 * The log levels, in order of increasing verbosity, are:
 *
 *     LOG_LEVEL_NONE
 *     LOG_LEVEL_ERROR
 *     LOG_LEVEL_INFO
 *     LOG_LEVEL_DEBUG
 *     LOG_LEVEL_TRACE
 *
 * The log macros are:
 *
 *     ERROR(fmt, ...)
 *     INFO(fmt, ...)
 *     DEBUG(fmt, ...)
 *     TRACE(fmt, ...)
 *
 * There is an additional special log macro that depends on the environment
 * variable LOG_LEVEL being set to one of "none"|"error"|"info"|"debug"|"trace"
 * (case-insensitive). If not set, the default is LOG_LEVEL_NONE.
 *
 *     LOG(fmt, ...)
 *
 * Special log macros that always work regardless of
 * LOG_LEVEL:
 *
 *     PANIC(fmt, ...)
 *     FATAL(fmt, ...)
 *
 * NOTES:
 *
 * 1. The ERROR, INFO, DEBUG, and TRACE macros are based on the defined value
 *    for LOG_LEVEL at compile time.
 *
 *    The LOG macro dynamically checks the value of the environment variable
 *    `LOG_LEVEL` at runtime (the value is case-insensitive and must be one of
 *    "none"|"error"|"info"|"debug"|"trace"; the default is none. The trade-off
 *    for using LOG is convenience over performance and binary size.
 *
 * 1. EXIT_FAILURE regardless of LOG_LEVEL. The only difference
 *    is that PANIC prints a stack trace. Using the macros
 *    instead of panic() and fatal() functions directly provides
 *    extra detail: {filename}:{line}: {function}.
 *
 * 2. All logs (even INFO) are printed to stderr to facilitate
 *    redirecting log output to a different destination than
 *    stdout.
 *
 * 3. Log functions are variadic and support printf format
 *    specifiers.
 *
 * 4. Log functions automatically append a newline to output.
 *
 * 5. To augment the log stack trace for panic and various
 *    abort signals, make sure to call
 *    register_signal_panic_handlers() early (e.g., at the
 *    top of main(). This is recommended even if you don't
 *    explicitly invoke panic() in any code.
 *
 */

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5
#define LOG_LEVEL_TODO -1

/**
 * Register handlers to augment log stack trace.
 * Recommended: register at the top of main().
 *
 *  SIGABRT - failed tests, etc.
 *  SIGFPE  - arithmetic (divide by zero, etc) errors
 *  SIGILL  - illegal instruction
 *  SIGSEGV - segmentation violation
 *  SIGBUS  - invalid bus address access
 */
void register_signal_panic_handlers ();

/**
 * Print stack trace to stderr.
 */
void log_stack_trace ();

/**
 * Print to stderr along with stack trace and exit with EXIT_FAILURE.
 */
void panic (const char *fmt, ...);

/**
 * Print to stderr and exit with EXIT_FAILURE.
 */
void fatal (const char *fmt, ...);

/**
 *  Print to stderr.
 */
void log_error (const char *fmt, ...);

/**
 * Print to stderr (includes time).
 */
void log_time (const char *fmt, ...);

/**
 * Print to stdout.
 */
void log_info (const char *fmt, ...);


/**
 * Print formated trace statements to stderr.
 */
static inline void trace (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	vfprintf (stderr, fmt, args);
	fprintf (stderr, "\n");
	va_end (args);
}

/**
 * MACROS
 */

/**
 * PANIC: recommended to call register_signal_panic_handlers() before using.
 */
#define PANIC(format, ...)                                                     \
	fprintf (stderr, "PANIC: %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	panic (format __VA_OPT__ (, ) __VA_ARGS__)

/**
 * FATAL
 */
#define FATAL(format, ...)                                                     \
	fprintf (stderr, "FATAL: %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	fatal (format __VA_OPT__ (, ) __VA_ARGS__)

/**
 * ERROR requires:  #define LOG_LEVEL LOG_LEVEL_ERROR
 */
#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_LEVEL_ERROR
#define ERROR(format, ...)                                                     \
	fprintf (stderr, "ERROR: %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	trace (format __VA_OPT__ (, ) __VA_ARGS__)
#else
#define ERROR(format, ...)
#endif /* LOG_LEVEL >= LOG_LEVEL_ERROR */

/**
 * WARN requires:  #define LOG_LEVEL LOG_LEVEL_WARN
 */
#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_LEVEL_WARN
#define WARN(format, ...)                                                      \
	fprintf (stderr, "WARN : %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	trace (format __VA_OPT__ (, ) __VA_ARGS__)
#else
#define WARN(format, ...)
#endif /* LOG_LEVEL >= LOG_LEVEL_WARN */

/**
 * INFO requires:  #define LOG_LEVEL LOG_LEVEL_INFO
 */
#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_LEVEL_INFO
#define INFO(format, ...)                                                      \
	fprintf (stderr, "INFO : %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	trace (format __VA_OPT__ (, ) __VA_ARGS__)
#else
#define INFO(format, ...)
#endif /* LOG_LEVEL >= LOG_LEVEL_INFO */

/**
 * DEBUG requires:  #define LOG_LEVEL LOG_LEVEL_DEBUG
 */
#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_LEVEL_DEBUG
#define DEBUG(format, ...)                                                     \
	fprintf (stderr, "DEBUG: %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	trace (format __VA_OPT__ (, ) __VA_ARGS__)
#else
#define DEBUG(format, ...)
#endif /* LOG_LEVEL >= LOG_LEVEL_DEBUG */

/**
 * TRACE requires:  #define LOG_LEVEL LOG_LEVEL_TRACE
 */
#if defined(LOG_LEVEL) && LOG_LEVEL >= LOG_LEVEL_TRACE
#define TRACE(format, ...)                                                     \
	fprintf (stderr, "TRACE: %s:%d: %s(): ", __FILE_NAME__, __LINE__,      \
		 __func__);                                                    \
	trace (format __VA_OPT__ (, ) __VA_ARGS__)
#else
#define TRACE(format, ...)
#endif /* LOG_LEVEL >= LOG_LEVEL_TRACE */

/**
 * LOG depends on environment variable LOG_LEVEL.
 */
#define LOG(log_level, format, ...)                                            \
	do {                                                                   \
		int ll = (int)log_level;                                       \
		char *env_level_str = getenv ("LOG_LEVEL");                    \
		int env_level = LOG_LEVEL_NONE;                                \
		if (env_level_str == nullptr) {                                \
			/* do nothing */                                       \
		} else if (strncasecmp (env_level_str, "error", 5) == 0) {     \
			env_level = LOG_LEVEL_ERROR;                           \
		} else if (strncasecmp (env_level_str, "warn", 4) == 0) {      \
			env_level = LOG_LEVEL_WARN;                            \
		} else if (strncasecmp (env_level_str, "info", 4) == 0) {      \
			env_level = LOG_LEVEL_INFO;                            \
		} else if (strncasecmp (env_level_str, "debug", 5) == 0) {     \
			env_level = LOG_LEVEL_DEBUG;                           \
		} else if (strncasecmp (env_level_str, "trace", 5) == 0) {     \
			env_level = LOG_LEVEL_TRACE;                           \
		} else if (strncasecmp (env_level_str, "todo", 4) == 0) {      \
			env_level = LOG_LEVEL_TODO;                            \
		} else {                                                       \
			/* do nothing */                                       \
		}                                                              \
		if (ll == LOG_LEVEL_TODO) {                                    \
			fprintf (stderr, "%s: %s:%d: %s(): ", "TODO ",         \
				 __FILE_NAME__, __LINE__, __func__);           \
			trace (format __VA_OPT__ (, ) __VA_ARGS__);            \
		} else if (env_level >= ll) {                                  \
			char *level = nullptr;                                 \
			switch (ll) {                                          \
			case LOG_LEVEL_ERROR: level = "ERROR"; break;          \
			case LOG_LEVEL_WARN: level = "WARN "; break;           \
			case LOG_LEVEL_INFO: level = "INFO "; break;           \
			case LOG_LEVEL_DEBUG: level = "DEBUG"; break;          \
			case LOG_LEVEL_TRACE: level = "TRACE"; break;          \
			default: /* do nothing */                              \
			}                                                      \
			if (level != nullptr) {                                \
				fprintf (stderr, "%s: %s:%d: %s(): ", level,   \
					 __FILE_NAME__, __LINE__, __func__);   \
				trace (format __VA_OPT__ (, ) __VA_ARGS__);    \
			}                                                      \
		}                                                              \
	} while (0)

#define LOG_ERROR(format, ...)                                                 \
	LOG (LOG_LEVEL_ERROR, format __VA_OPT__ (, ) __VA_ARGS__)

#define LOG_WARN(format, ...)                                                  \
	LOG (LOG_LEVEL_WARN, format __VA_OPT__ (, ) __VA_ARGS__)

#define LOG_INFO(format, ...)                                                  \
	LOG (LOG_LEVEL_INFO, format __VA_OPT__ (, ) __VA_ARGS__)

#define LOG_DEBUG(format, ...)                                                 \
	LOG (LOG_LEVEL_DEBUG, format __VA_OPT__ (, ) __VA_ARGS__)

#define LOG_TRACE(format, ...)                                                 \
	LOG (LOG_LEVEL_TRACE, format __VA_OPT__ (, ) __VA_ARGS__)

#define TODO(format, ...)                                                      \
	LOG (LOG_LEVEL_TODO, format __VA_OPT__ (, ) __VA_ARGS__)

#endif /* LOG_H */
