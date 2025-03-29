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

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "log.h"

#include <stdarg.h>


void log_info (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);

	FILE *f = stdout;
	vfprintf (f, fmt, args);
	fprintf (f, "\n");

	va_end (args);
}


void log_time (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);

	FILE *f = stderr;
	time_t now = time (nullptr);
	struct tm *time = gmtime (&now);
	char time_str[9];
	strftime (time_str, sizeof (time_str), "%T", time);
	fprintf (f, "[%s] ", time_str);
	vfprintf (f, fmt, args);
	va_end (args);
	fprintf (f, "\n");
}


void log_error (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);

	FILE *f = stderr;
	fprintf (f, "error: ");

#if LOG_UTC_TIME
	time_t now = time (nullptr);
	struct tm *time = gmtime (&now);
	char time_str[9];
	strftime (time_str, sizeof (time_str), "%T", time);
	fprintf (stderr, "[%s] ", time_str);
	vfprintf (stderr, fmt, args);
#else
	vfprintf (f, fmt, args);
#endif
	va_end (args);
	fprintf (f, "\n");
}


void log_stack_trace ()
{
	void *buffer[1024];
	int count = backtrace (buffer, sizeof (buffer) / sizeof (buffer[0]));
	char **traces = backtrace_symbols (buffer, count);
	if (traces != NULL) {
		for (int i = 0; i < count; i++) {
			fprintf (stderr, "%s\n", traces[i]);
		}
		free (traces);
	} else {
		perror ("unable to print stack trace");
		exit (EXIT_FAILURE);
	}
}


static void panic_signal_handler (int sig)
{
	log_error ("Caught signal: %d", sig);
	// log_stack_trace (SKIP_SIGNAL_HANDLER_FRAMES);
	log_stack_trace ();
	exit (EXIT_FAILURE);
}


void register_signal_panic_handlers ()
{
	/* don't intercept SIGINT, SIGTERM */
	signal (SIGABRT, panic_signal_handler); /* for testing */
	signal (SIGFPE,
		panic_signal_handler); /* arithmetic (divide by zero, etc) */
	signal (SIGILL, panic_signal_handler); /* illegal instruction */
	signal (SIGSEGV, panic_signal_handler); /* segmentation violation */
	signal (SIGBUS,
		panic_signal_handler); /* bus error accessing invalid address */
}


void panic (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);

	vfprintf (stderr, fmt, args);
	fprintf (stderr, "\n");

	log_stack_trace ();

	va_end (args);
	exit (EXIT_FAILURE);
}

void fatal (const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);

	vfprintf (stderr, fmt, args);
	fprintf (stderr, "\n");

	va_end (args);
	exit (EXIT_FAILURE);
}
