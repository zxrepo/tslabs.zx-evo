#include "std.h"
#include "emul.h"
#include "debug.h"
#include "dbgtrace.h"
#include "consts.h"

#ifdef MOD_MONITOR

u8 trace_labels;

void debugscr()
{
	memset(txtscr, backgr_ch, sizeof txtscr / 2);
	memset(txtscr + sizeof txtscr / 2, backgr, sizeof txtscr / 2);
	nfr = 0;

	show_regs();
	show_trace();
	show_mem();
	show_watch();
	show_stack();
	show_ay();
	show_banks();
	show_ports();
	show_dos();
	show_tsconf();

	show_time();
}


#endif // MOD_MONITOR

