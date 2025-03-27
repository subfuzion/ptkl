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
#include <string.h>
#include <sys/time.h>

#include "errors.h"


static void log_error (const char *message)
{
#if LOG_UTC_TIME
	time_t now = time (nullptr);
	struct tm *time = gmtime (&now);
	char time_str[9];
	strftime (time_str, sizeof (time_str), "%T", time);
	fprintf (stderr, "[%s] %s\n", time_str, message);
#else
	fprintf (stderr, "%s\n", message);
#endif
}


static void print_stack_trace (const char *msg)
{
	if (msg != nullptr && strlen (msg) > 0) log_error (msg);
	void *buffer[1024];
	int count = backtrace (buffer, sizeof (buffer) / sizeof (buffer[0]));
	char **traces = backtrace_symbols (buffer, count);
	if (traces != NULL) {
		log_error ("Stack trace:");
		for (int i = 3; i < count; i++) {
			log_error (traces[i]);
		}
		free (traces);
	} else {
		perror ("unable to print stack trace");
		exit (EXIT_FAILURE);
	}
	exit (EXIT_FAILURE);
}


static void panic_signal_handler (int sig)
{
	char msg[80];
	snprintf (msg, 80, "Caught signal: %d", sig);
	print_stack_trace (msg);
}


void register_signal_panic_handlers ()
{
	/* don't SIGINT, SIGTERM */
	signal (SIGABRT, panic_signal_handler); /* for tesing */
	signal (SIGFPE,
		panic_signal_handler); /* arithmetic (divide by zero, etc) */
	signal (SIGILL, panic_signal_handler); /* illegal instruction */
	signal (SIGSEGV, panic_signal_handler); /* segmentation violation */
	signal (SIGBUS,
		panic_signal_handler); /* bus error accessing invalid address */
}


void panic (const char *msg)
{
	char err[1024];
	snprintf (err, 1024, "panic: %s", msg ? msg : "unknown error");
	print_stack_trace (err);
}
