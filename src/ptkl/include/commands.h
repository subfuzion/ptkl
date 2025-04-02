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
 * The above copyright notice and this permissio  n notice shall be included in
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

#ifndef COMMANDS_H
#define COMMANDS_H

#include "command.h"

/* Command groups */
#define GROUP_BUILTIN "Built-in Commands"
#define GROUP_DEVELOPMENT "Development Commands"
#define GROUP_SERVICES "Service Commands"
#define GROUP_INTERACTIVE "Interactive Commands"

/* Command constructors */
command compile_new (command parent, const char *group);
command console_command_new (command parent, const char *group);
command data_new (command parent, const char *group);
command help_new (command parent, const char *group);
command logs_new (command parent, const char *group);
command repl_new (command parent, const char *group);
command run_new (command parent, const char *group);
command serve_new (command parent, const char *group);
command service_new (command parent, const char *group);
command storage_new (command parent, const char *group);
command version_new (command parent, const char *group);

#endif /* COMMANDS_H */
