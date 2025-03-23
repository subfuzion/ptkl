# Contributing

## High level notes

### Fail fast for non-interactive code

In general, functions are expected to succeed and, therefore, shouldn't return
errors. Use `fatal` (logs error and exits) and `panic` (logs error plus stack
trace and then exits) instead of complicating code with error handling.

Partikle will include an interactive SHELL/REPL component that shouldn't exit 
until the user quits. For interactive use, a flag will be set to reload the 
runtime and restore session state without exiting the interactive process. 
