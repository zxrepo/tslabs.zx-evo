#include "std.h"
#include "emul.h"
#include "emulkeys.h"
#include "snapshot.h"
#include "gui.h"
#include "savesnd.h"
#include "cheat.h"
#include "debugger/core.h"

action ac_main[] =
{
   { "main.exit", correct_exit },
   { "main.monitor", main_debug },
   { "main.fullscreen", main_fullscr },
   { "main.pause", main_pause },
   { "main.selectfix", main_selectfix },
   { "main.selectsnd", main_selectsnd },
   { "main.incfix", main_incfix },
   { "main.decfix", main_decfix },
   { "main.incfix10", main_incfix10 },
   { "main.decfix10", main_decfix10 },
   { "main.leds", main_leds },
   { "main.status", main_status },
   { "main.maxspeed", main_maxspeed },
   { "main.selectfilter", main_selectfilter },
   { "main.selectdriver", main_selectdriver },
   { "main.pokedialog", main_poke },
   { "main.starttape", main_starttape },
   { "main.screenshot", main_scrshot },
   { "main.savevideo", main_savevideo },
   { "main.reset", main_reset },
   { "main.reset128", main_reset128 },
   { "main.resetsys", main_resetsys },
   { "main.reset48", main_reset48 },
   { "main.resetbasic", main_resetbas },
   { "main.resetdos", main_resetdos },
   { "main.resetcache", main_resetcache },
   { "main.nmi", main_nmi },
   { "main.nmidos", main_nmidos },
   { "main.nmicache", main_nmicache },
   { "main.tapebrowser", main_tapebrowser },
   { "main.settings", setup_dlg },
   { "main.save", savesnap },
   { "main.load", opensnap },
   { "main.savesound", savesnddialog },
   { "main.qsave1", qsave1 },
   { "main.qsave2", qsave2 },
   { "main.qsave3", qsave3 },
   { "main.qload1", qload1 },
   { "main.qload2", qload2 },
   { "main.qload3", qload3 },
   { "main.keystick", main_keystick },
   { "main.autofire", main_autofire },
   { "main.saveram", main_save_ram },
   { "main.saveall", main_save },
   { "main.lockmouse", main_mouse },
   { "main.xtkbd", main_atmkbd },
   { "main.pastetext", main_pastetext },
   { "main.size1", main_size1 },
   { "main.size2", main_size2 },
   { "main.sizem", main_sizem },
   { "main.memsearch", main_cheat },
   { "main.help", main_help },
   { "main.tsutoggle", main_tsutoggle },
   { "main.flictoggle", main_flictoggle },

   { 0, 0 },
};

action ac_main_xt[] =
{
   { "main.xtkbd", main_atmkbd },

   { 0, 0 },
};
