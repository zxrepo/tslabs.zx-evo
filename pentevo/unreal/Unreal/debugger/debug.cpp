#include "std.h"
#include "emul.h"
#include "vars.h"
#include "draw.h"
#include "dx.h"
#include "debug.h"
#include "dbgtrace.h"
#include "dbgoth.h"
#include "dbglabls.h"
#include "dbgbpx.h"
#include "dbgcmd.h"
#include "util.h"
#include "resource.h"
#include "emulkeys.h"
#include "dbgrwdlg.h"
#include "font16.h"
#include "consts.h"
#include "op_system.h"

#ifdef MOD_MONITOR

u8 trace_labels;

unsigned show_scrshot;
unsigned user_watches[3] = { 0x4000, 0x8000, 0xC000 };


unsigned dbg_extport;
u8 dgb_extval; // extended memory port like 1FFD or DFFD

void show_tsconf();
void init_tsconf();

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

