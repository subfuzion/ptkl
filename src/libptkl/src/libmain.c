#include <log.h>

#include "ptkl.h"

void ptkl_init ()
{
	register_signal_panic_handlers ();
}
