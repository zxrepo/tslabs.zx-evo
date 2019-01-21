#pragma once
#include "ui_action.h"
#include "debugger/libs/view.h"

class Actions final
{
public:
	Actions()
	{
		printf("1");
	}

	UIAction<> mon_emul = UIAction<>(ActionType::monitor, "emul");
	UIAction<> mon_save_block = UIAction<>(ActionType::monitor, "saveblock");
	UIAction<> mon_load_block = UIAction<>(ActionType::monitor, "loadblock");
	UIAction<> mon_fill_block = UIAction<>(ActionType::monitor, "fillblock");
	UIAction<> mon_step = UIAction<>(ActionType::monitor, "step");
	UIAction<> mon_step_over = UIAction<>(ActionType::monitor, "stepover");
	UIAction<> mon_exit_sub = UIAction<>(ActionType::monitor, "exitsub");
	UIAction<> mon_dump = UIAction<>(ActionType::monitor, "dump");

	UIAction<> mon_switch_dump = UIAction<>(ActionType::monitor, "switchdump");
	UIAction<> mon_next = UIAction<>(ActionType::monitor, "next");
	UIAction<> mon_prev = UIAction<>(ActionType::monitor, "prev");
	UIAction<> mon_ripper = UIAction<>(ActionType::monitor, "rip");
	UIAction<> mon_help = UIAction<>(ActionType::monitor, "help");
	UIAction<> mon_cpu = UIAction<>(ActionType::monitor, "cpu");
	UIAction<> mon_exit = UIAction<>(ActionType::monitor, "exit");

	UIAction<> mon_poke_dialog = UIAction<>(ActionType::monitor, "pokedialog");
	UIAction<> mon_tape_browser = UIAction<>(ActionType::monitor, "tapebrowser");
	UIAction<> mon_reset = UIAction<>(ActionType::monitor, "reset");
	UIAction<> mon_reset128 = UIAction<>(ActionType::monitor, "reset128");
	UIAction<> mon_reset_sys = UIAction<>(ActionType::monitor, "resetsys");
	UIAction<> mon_reset48 = UIAction<>(ActionType::monitor, "reset48");
	UIAction<> mon_reset_basic = UIAction<>(ActionType::monitor, "resetbasic");
	UIAction<> mon_reset_dos = UIAction<>(ActionType::monitor, "resetdos");
	UIAction<> mon_reset_cache = UIAction<>(ActionType::monitor, "resetcache");
	UIAction<> mon_set_watch = UIAction<>(ActionType::monitor, "setwatch");
	UIAction<> mon_screenshot = UIAction<>(ActionType::monitor, "scrshot");
	UIAction<> mon_switch_ay = UIAction<>(ActionType::monitor, "switchay");

	UIAction<> mon_nmi = UIAction<>(ActionType::monitor, "nmi");
	UIAction<> mon_nmi_dos = UIAction<>(ActionType::monitor, "nmidos");
	UIAction<> mon_nmi_cache = UIAction<>(ActionType::monitor, "nmicache");
	UIAction<> mon_save_snap = UIAction<>(ActionType::monitor, "save");
	UIAction<> mon_load_snap = UIAction<>(ActionType::monitor, "load");
	UIAction<> mon_save_sound = UIAction<>(ActionType::monitor, "savesound");
	UIAction<> mon_qsave1 = UIAction<>(ActionType::monitor, "qsave1");
	UIAction<> mon_qsave2 = UIAction<>(ActionType::monitor, "qsave2");
	UIAction<> mon_qsave3 = UIAction<>(ActionType::monitor, "qsave3");
	UIAction<> mon_qload1 = UIAction<>(ActionType::monitor, "qload1");
	UIAction<> mon_qload2 = UIAction<>(ActionType::monitor, "qload2");
	UIAction<> mon_qload3 = UIAction<>(ActionType::monitor, "qload3");
	UIAction<> mon_labels = UIAction<>(ActionType::monitor, "labels");
	UIAction<> mon_set_bank = UIAction<>(ActionType::monitor, "setbank");
	UIAction<> mon_mem_search = UIAction<>(ActionType::monitor, "memsearch");
	UIAction<> mon_set_himem = UIAction<>(ActionType::monitor, "sethimem");

	UIAction<> main_exit = UIAction<>(ActionType::main, "exit");
	UIAction<> main_monitor = UIAction<>(ActionType::main, "monitor");
	UIAction<> main_fullsceen = UIAction<>(ActionType::main, "fullscreen");
	UIAction<> main_pause = UIAction<>(ActionType::main, "pause");
	UIAction<> main_select_fix = UIAction<>(ActionType::main, "selectfix");
	UIAction<> main_select_snd = UIAction<>(ActionType::main, "selectsnd");
	UIAction<> main_inc_fix = UIAction<>(ActionType::main, "incfix");
	UIAction<> main_dec_fix = UIAction<>(ActionType::main, "decfix");
	UIAction<> main_inc_fix10 = UIAction<>(ActionType::main, "incfix10");
	UIAction<> main_dec_fix10 = UIAction<>(ActionType::main, "decfix10");

	UIAction<> main_leds = UIAction<>(ActionType::main, "leds");
	UIAction<> main_status = UIAction<>(ActionType::main, "status");
	UIAction<> main_max_speed = UIAction<>(ActionType::main, "maxspeed");
	UIAction<> main_select_filter = UIAction<>(ActionType::main, "selectfilter");
	UIAction<> main_select_driver = UIAction<>(ActionType::main, "selectdriver");
	UIAction<> main_poke_dialog = UIAction<>(ActionType::main, "pokedialog");
	UIAction<> main_start_tape = UIAction<>(ActionType::main, "starttape");
	UIAction<> main_screenshoot = UIAction<>(ActionType::main, "screenshot");
	UIAction<> main_save_video = UIAction<>(ActionType::main, "savevideo");

	UIAction<> main_reset = UIAction<>(ActionType::main, "reset");
	UIAction<> main_reset128 = UIAction<>(ActionType::main, "reset128");
	UIAction<> main_reset_sys = UIAction<>(ActionType::main, "resetsys");
	UIAction<> main_reset48 = UIAction<>(ActionType::main, "reset48");
	UIAction<> main_reset_basic = UIAction<>(ActionType::main, "resetbasic");
	UIAction<> main_reset_dos = UIAction<>(ActionType::main, "resetdos");
	UIAction<> main_reset_cache = UIAction<>(ActionType::main, "resetcache");

	UIAction<> main_nmi = UIAction<>(ActionType::main, "nmi");
	UIAction<> main_nmi_dos = UIAction<>(ActionType::main, "nmidos");
	UIAction<> main_nmi_cache = UIAction<>(ActionType::main, "nmicache");

	UIAction<> main_tape_browser = UIAction<>(ActionType::main, "tapebrowser");
	UIAction<> main_settings = UIAction<>(ActionType::main, "settings");
	UIAction<> main_save_snap = UIAction<>(ActionType::main, "save");
	UIAction<> main_load_snap = UIAction<>(ActionType::main, "load");
	UIAction<> main_save_sound = UIAction<>(ActionType::main, "savesound");
	UIAction<> main_qsave1 = UIAction<>(ActionType::main, "qsave1");
	UIAction<> main_qsave2 = UIAction<>(ActionType::main, "qsave2");
	UIAction<> main_qsave3 = UIAction<>(ActionType::main, "qsave3");
	UIAction<> main_qload1 = UIAction<>(ActionType::main, "qload1");
	UIAction<> main_qload2 = UIAction<>(ActionType::main, "qload2");
	UIAction<> main_qload3 = UIAction<>(ActionType::main, "qload3");

	UIAction<> main_key_stick = UIAction<>(ActionType::main, "keystick");
	UIAction<> main_auto_fire = UIAction<>(ActionType::main, "autofire");
	UIAction<> main_save_ram = UIAction<>(ActionType::main, "saveram");
	UIAction<> main_save_all = UIAction<>(ActionType::main, "saveall");
	UIAction<> main_lock_mouse = UIAction<>(ActionType::main, "lockmouse");
	UIAction<> main_atm_keyboard = UIAction<>(ActionType::main, "xtkbd");
	UIAction<> main_paste_text = UIAction<>(ActionType::main, "pastetext");
	UIAction<> main_size1 = UIAction<>(ActionType::main, "size1");
	UIAction<> main_size2 = UIAction<>(ActionType::main, "size2");
	UIAction<> main_size_max = UIAction<>(ActionType::main, "sizem");
	UIAction<> main_mem_search = UIAction<>(ActionType::main, "memsearch");
	UIAction<> main_help = UIAction<>(ActionType::main, "help");
	UIAction<> main_tsu_toggle = UIAction<>(ActionType::main, "tsutoggle");
	UIAction<> main_flic_toggle = UIAction<>(ActionType::main, "flictoggle");

	UIAction<> atm_keyboard = UIAction<>(ActionType::xt, "xtkbd");

	UIAction<> banks_up = UIAction<>(ActionType::banks, "up");
	UIAction<> banks_down = UIAction<>(ActionType::banks, "down ");
	UIAction<> banks_edit = UIAction<>(ActionType::banks, "edit");

	UIAction<> mem_left = UIAction<>(ActionType::memory, "left");
	UIAction<> mem_right = UIAction<>(ActionType::memory, "right");
	UIAction<> mem_up = UIAction<>(ActionType::memory, "up");
	UIAction<> mem_down = UIAction<>(ActionType::memory, "down");
	UIAction<> mem_pg_up = UIAction<>(ActionType::memory, "pgup");
	UIAction<> mem_pg_down = UIAction<>(ActionType::memory, "pgdn");

	UIAction<> mem_switch = UIAction<>(ActionType::memory, "switch");
	UIAction<> mem_start_line = UIAction<>(ActionType::memory, "stline");
	UIAction<> mem_end_line = UIAction<>(ActionType::memory, "endline");
	UIAction<> mem_find_text = UIAction<>(ActionType::memory, "findtext");
	UIAction<> mem_find_code = UIAction<>(ActionType::memory, "findcode");

	UIAction<> mem_goto = UIAction<>(ActionType::memory, "goto");
	UIAction<> mem_view = UIAction<>(ActionType::memory, "mem");
	UIAction<> mem_disk_phys = UIAction<>(ActionType::memory, "diskphys");
	UIAction<> mem_disk_log = UIAction<>(ActionType::memory, "disklog");
	UIAction<> mem_disk_go = UIAction<>(ActionType::memory, "diskgo");

	UIAction<> mem_view_pc = UIAction<>(ActionType::memory, "pc");
	UIAction<> mem_view_sp = UIAction<>(ActionType::memory, "sp");
	UIAction<> mem_view_bc = UIAction<>(ActionType::memory, "bc");
	UIAction<> mem_view_de = UIAction<>(ActionType::memory, "de");
	UIAction<> mem_view_hl = UIAction<>(ActionType::memory, "hl");
	UIAction<> mem_view_ix = UIAction<>(ActionType::memory, "ix");
	UIAction<> mem_view_iy = UIAction<>(ActionType::memory, "iy");

	UIAction<> reg_left = UIAction<>(ActionType::reg, "left");
	UIAction<> reg_right = UIAction<>(ActionType::reg, "right");
	UIAction<> reg_up = UIAction<>(ActionType::reg, "up");
	UIAction<> reg_down = UIAction<>(ActionType::reg, "down");
	UIAction<> reg_edit = UIAction<>(ActionType::reg, "edit");

	UIAction<> reg_a = UIAction<>(ActionType::reg, "a");
	UIAction<> reg_f = UIAction<>(ActionType::reg, "f");
	UIAction<> reg_bc = UIAction<>(ActionType::reg, "bc");
	UIAction<> reg_de = UIAction<>(ActionType::reg, "de");
	UIAction<> reg_hl = UIAction<>(ActionType::reg, "hl");
	UIAction<> reg_pc = UIAction<>(ActionType::reg, "pc");
	UIAction<> reg_sp = UIAction<>(ActionType::reg, "sp");
	UIAction<> reg_ix = UIAction<>(ActionType::reg, "ix");
	UIAction<> reg_iy = UIAction<>(ActionType::reg, "iy");
	UIAction<> reg_i = UIAction<>(ActionType::reg, "i");
	UIAction<> reg_r = UIAction<>(ActionType::reg, "r");

	UIAction<> reg_im = UIAction<>(ActionType::reg, "im");
	UIAction<> reg_iff1 = UIAction<>(ActionType::reg, "iff1");
	UIAction<> reg_iff2 = UIAction<>(ActionType::reg, "iff2");

	UIAction<> reg_sf = UIAction<>(ActionType::reg, "SF");
	UIAction<> reg_zf = UIAction<>(ActionType::reg, "ZF");
	UIAction<> reg_f5 = UIAction<>(ActionType::reg, "F5");
	UIAction<> reg_hf = UIAction<>(ActionType::reg, "HF");
	UIAction<> reg_f3 = UIAction<>(ActionType::reg, "F3");
	UIAction<> reg_pf = UIAction<>(ActionType::reg, "PF");
	UIAction<> reg_nf = UIAction<>(ActionType::reg, "NF");
	UIAction<> reg_cf = UIAction<>(ActionType::reg, "CF");

	UIAction<> reg_code_jump = UIAction<>(ActionType::reg, "codejump");
	UIAction<> reg_data_jump = UIAction<>(ActionType::reg, "datajump");

	UIAction<> trace_find_pc = UIAction<>(ActionType::trace, "findpc");
	UIAction<> trace_here = UIAction<>(ActionType::trace, "here");
	UIAction<> trace_find_text = UIAction<>(ActionType::trace, "findtext");
	UIAction<> trace_find_code = UIAction<>(ActionType::trace, "findcode");
	UIAction<> trace_goto = UIAction<>(ActionType::trace, "goto");
	UIAction<> trace_bpx = UIAction<>(ActionType::trace, "bpx");
	UIAction<> trace_asm = UIAction<>(ActionType::trace, "asm");
	UIAction<> trace_set_pc = UIAction<>(ActionType::trace, "setpc");

	UIAction<> trace_up = UIAction<>(ActionType::trace, "up");
	UIAction<> trace_down = UIAction<>(ActionType::trace, "down");
	UIAction<> trace_left = UIAction<>(ActionType::trace, "left");
	UIAction<> trace_right = UIAction<>(ActionType::trace, "right");
	UIAction<> trace_pg_up = UIAction<>(ActionType::trace, "pgdn");
	UIAction<> trace_pg_down = UIAction<>(ActionType::trace, "pgup");

	UIAction<> trace_save1 = UIAction<>(ActionType::trace, "save1");
	UIAction<> trace_save2 = UIAction<>(ActionType::trace, "save2");
	UIAction<> trace_save3 = UIAction<>(ActionType::trace, "save3");
	UIAction<> trace_save4 = UIAction<>(ActionType::trace, "save4");
	UIAction<> trace_save5 = UIAction<>(ActionType::trace, "save5");
	UIAction<> trace_save6 = UIAction<>(ActionType::trace, "save6");
	UIAction<> trace_save7 = UIAction<>(ActionType::trace, "save7");
	UIAction<> trace_save8 = UIAction<>(ActionType::trace, "save8");

	UIAction<> trace_restore1 = UIAction<>(ActionType::trace, "crest1");
	UIAction<> trace_restore2 = UIAction<>(ActionType::trace, "crest2");
	UIAction<> trace_restore3 = UIAction<>(ActionType::trace, "crest3");
	UIAction<> trace_restore4 = UIAction<>(ActionType::trace, "crest4");
	UIAction<> trace_restore5 = UIAction<>(ActionType::trace, "crest5");
	UIAction<> trace_restore6 = UIAction<>(ActionType::trace, "crest6");
	UIAction<> trace_restore7 = UIAction<>(ActionType::trace, "crest7");
	UIAction<> trace_restore8 = UIAction<>(ActionType::trace, "crest8");

	UIAction<> trace_back = UIAction<>(ActionType::trace, "back");
	UIAction<> trace_context = UIAction<>(ActionType::trace, "context");
	UIAction<> trace_data_jump = UIAction<>(ActionType::trace, "datajump");
	UIAction<> trace_labels = UIAction<>(ActionType::trace, "labels");
	UIAction<> trace_import_labels = UIAction<>(ActionType::trace, "importl");

	UIAction<> mon_gs = UIAction<>(ActionType::monitor, "gs");
	UIAction<> mon_osw = UIAction<>(ActionType::monitor, "osw");
	UIAction<> mon_settings = UIAction<>(ActionType::monitor, "settings");
	UIAction<> mon_bp_dialog = UIAction<>(ActionType::monitor, "bpdialog");

	UIAction<unsigned, unsigned> dialog_find1 = UIAction<unsigned, unsigned>(ActionType::sys, "");
	UIAction<unsigned, unsigned> dialog_find2 = UIAction<unsigned, unsigned>(ActionType::sys, "");

	UIAction<MenuDef&, char> handle_menu = UIAction<MenuDef&, char>(ActionType::sys, "");
	UIAction<HWND> on_paint = UIAction<HWND>(ActionType::sys, "");
	UIAction<dbgwnd> set_active_dbg = UIAction<dbgwnd>(ActionType::sys, "");

	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
};

auto get_actions(const std::vector<ActionType>& types)->std::vector<UIAction<>*>;
extern Actions actions;
