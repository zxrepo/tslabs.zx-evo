#include "std.h"
#include "emul.h"
#include "vars.h"
#include "draw.h"
#include "gui.h"

void out(unsigned port, u8 val);


void mon_scr(char alt)
{

}

void mon_scr0() { mon_scr(0); }
void mon_scr1() { mon_scr(1); }
void mon_scray() { mon_scr(2); }


void mon_setup_dlg()
{
#ifdef MOD_SETTINGS
	setup_dlg();
	temp.rflags = RF_MONITOR;
	set_video();
#endif
}

void mon_scrshot() { show_scrshot++; if (show_scrshot == 3) show_scrshot = 0; }
