#pragma once
#include <vector>
#include <functional>
#include "debugger/libs/view.h"

enum class ActionType
{
	main,
	monitor,
	reg,
	trace,
	memory,
	banks,
	xt,
	sys,

	empty,
};

class UIKeyBind {
public:
	unsigned k1{}, k2{}, k3{}, k4{};
};

template<class OutType = void, class ...InType>
class TFunc final : public std::conditional <sizeof...(InType) == 0, UIKeyBind, std::_Any_tag>::type
{
	enum { arg_count = sizeof...(InType) };
	using action_method = std::function<OutType(InType...)>;
	std::vector<action_method> actions_{};

public:
	const ActionType type;
	const std::string name;

	//template<const ActionType Type, typename Flag = std::enable_if<true, void>::type>
	TFunc(const ActionType type, std::string name) : type(type), name(std::move(name)) { }

	auto operator +=(action_method action) -> void
	{
		actions_.push_back(std::move(action));
	}

	template<class OT = OutType>
	typename std::enable_if<std::is_void<OT>::value, void>::type operator() (InType... args)
	{
		for (auto& item : actions_)
			item(args...);
	}

	template<class OT = OutType>
	typename std::enable_if<!std::is_void<OT>::value, OT>::type operator() (InType... args)
	{
		static OutType result{};
		for (auto& item : actions_)
			result = item(args...);

		return result;
	}

	auto is_empty() const -> bool
	{
		return type == ActionType::empty;
	}

	auto get_full_name() const -> std::string
	{
		std::string prefix;
		switch (type)
		{
		case ActionType::memory:
		case ActionType::banks:
			prefix = "mem.";
			break;
		case ActionType::xt:
		case ActionType::main:
			prefix = "main.";
			break;
		case ActionType::monitor:
			prefix = "mon.";
			break;
		case ActionType::reg:
			prefix = "reg.";
			break;
		case ActionType::trace:
			prefix = "cpu.";
			break;
		default:
			prefix = "`error#";
		}

		return prefix + name;
	}
};

template<class ...Args>
using TAction = TFunc<void, Args...>;

extern std::vector<TAction<>*> actions_registry;

class Actions final
{
public:
	TAction<> mon_emul = TAction<>(ActionType::monitor, "emul");
	TAction<> mon_save_block = TAction<>(ActionType::monitor, "saveblock");
	TAction<> mon_load_block = TAction<>(ActionType::monitor, "loadblock");
	TAction<> mon_fill_block = TAction<>(ActionType::monitor, "fillblock");
	TAction<> mon_step = TAction<>(ActionType::monitor, "step");
	TAction<> mon_step_over = TAction<>(ActionType::monitor, "stepover");
	TAction<> mon_exit_sub = TAction<>(ActionType::monitor, "exitsub");
	TAction<> mon_dump = TAction<>(ActionType::monitor, "dump");

	TAction<> mon_switch_dump = TAction<>(ActionType::monitor, "switchdump");
	TAction<> mon_next = TAction<>(ActionType::monitor, "next");
	TAction<> mon_prev = TAction<>(ActionType::monitor, "prev");
	TAction<> mon_ripper = TAction<>(ActionType::monitor, "rip");
	TAction<> mon_help = TAction<>(ActionType::monitor, "help");
	TAction<> mon_cpu = TAction<>(ActionType::monitor, "cpu");
	TAction<> mon_exit = TAction<>(ActionType::monitor, "exit");

	TAction<> mon_poke_dialog = TAction<>(ActionType::monitor, "pokedialog");
	TAction<> mon_tape_browser = TAction<>(ActionType::monitor, "tapebrowser");
	TAction<> mon_reset = TAction<>(ActionType::monitor, "reset");
	TAction<> mon_reset128 = TAction<>(ActionType::monitor, "reset128");
	TAction<> mon_reset_sys = TAction<>(ActionType::monitor, "resetsys");
	TAction<> mon_reset48 = TAction<>(ActionType::monitor, "reset48");
	TAction<> mon_reset_basic = TAction<>(ActionType::monitor, "resetbasic");
	TAction<> mon_reset_dos = TAction<>(ActionType::monitor, "resetdos");
	TAction<> mon_reset_cache = TAction<>(ActionType::monitor, "resetcache");
	TAction<> mon_set_watch = TAction<>(ActionType::monitor, "setwatch");
	TAction<> mon_screenshot = TAction<>(ActionType::monitor, "scrshot");
	TAction<> mon_switch_ay = TAction<>(ActionType::monitor, "switchay");

	TAction<> mon_nmi = TAction<>(ActionType::monitor, "nmi");
	TAction<> mon_nmi_dos = TAction<>(ActionType::monitor, "nmidos");
	TAction<> mon_nmi_cache = TAction<>(ActionType::monitor, "nmicache");
	TAction<> mon_save_snap = TAction<>(ActionType::monitor, "save");
	TAction<> mon_load_snap = TAction<>(ActionType::monitor, "load");
	TAction<> mon_save_sound = TAction<>(ActionType::monitor, "savesound");
	TAction<> mon_qsave1 = TAction<>(ActionType::monitor, "qsave1");
	TAction<> mon_qsave2 = TAction<>(ActionType::monitor, "qsave2");
	TAction<> mon_qsave3 = TAction<>(ActionType::monitor, "qsave3");
	TAction<> mon_qload1 = TAction<>(ActionType::monitor, "qload1");
	TAction<> mon_qload2 = TAction<>(ActionType::monitor, "qload2");
	TAction<> mon_qload3 = TAction<>(ActionType::monitor, "qload3");
	TAction<> mon_labels = TAction<>(ActionType::monitor, "labels");
	TAction<> mon_set_bank = TAction<>(ActionType::monitor, "setbank");
	TAction<> mon_mem_search = TAction<>(ActionType::monitor, "memsearch");
	TAction<> mon_set_himem = TAction<>(ActionType::monitor, "sethimem");

	TAction<> main_exit = TAction<>(ActionType::main, "exit");
	TAction<> main_monitor = TAction<>(ActionType::main, "monitor");
	TAction<> main_fullsceen = TAction<>(ActionType::main, "fullscreen");
	TAction<> main_pause = TAction<>(ActionType::main, "pause");
	TAction<> main_select_fix = TAction<>(ActionType::main, "selectfix");
	TAction<> main_select_snd = TAction<>(ActionType::main, "selectsnd");
	TAction<> main_inc_fix = TAction<>(ActionType::main, "incfix");
	TAction<> main_dec_fix = TAction<>(ActionType::main, "decfix");
	TAction<> main_inc_fix10 = TAction<>(ActionType::main, "incfix10");
	TAction<> main_dec_fix10 = TAction<>(ActionType::main, "decfix10");

	TAction<> main_leds = TAction<>(ActionType::main, "leds");
	TAction<> main_status = TAction<>(ActionType::main, "status");
	TAction<> main_max_speed = TAction<>(ActionType::main, "maxspeed");
	TAction<> main_select_filter = TAction<>(ActionType::main, "selectfilter");
	TAction<> main_select_driver = TAction<>(ActionType::main, "selectdriver");
	TAction<> main_poke_dialog = TAction<>(ActionType::main, "pokedialog");
	TAction<> main_start_tape = TAction<>(ActionType::main, "starttape");
	TAction<> main_screenshoot = TAction<>(ActionType::main, "screenshot");
	TAction<> main_save_video = TAction<>(ActionType::main, "savevideo");

	TAction<> main_reset = TAction<>(ActionType::main, "reset");
	TAction<> main_reset128 = TAction<>(ActionType::main, "reset128");
	TAction<> main_reset_sys = TAction<>(ActionType::main, "resetsys");
	TAction<> main_reset48 = TAction<>(ActionType::main, "reset48");
	TAction<> main_reset_basic = TAction<>(ActionType::main, "resetbasic");
	TAction<> main_reset_dos = TAction<>(ActionType::main, "resetdos");
	TAction<> main_reset_cache = TAction<>(ActionType::main, "resetcache");

	TAction<> main_nmi = TAction<>(ActionType::main, "nmi");
	TAction<> main_nmi_dos = TAction<>(ActionType::main, "nmidos");
	TAction<> main_nmi_cache = TAction<>(ActionType::main, "nmicache");

	TAction<> main_tape_browser = TAction<>(ActionType::main, "tapebrowser");
	TAction<> main_settings = TAction<>(ActionType::main, "settings");
	TAction<> main_save_snap = TAction<>(ActionType::main, "save");
	TAction<> main_load_snap = TAction<>(ActionType::main, "load");
	TAction<> main_save_sound = TAction<>(ActionType::main, "savesound");
	TAction<> main_qsave1 = TAction<>(ActionType::main, "qsave1");
	TAction<> main_qsave2 = TAction<>(ActionType::main, "qsave2");
	TAction<> main_qsave3 = TAction<>(ActionType::main, "qsave3");
	TAction<> main_qload1 = TAction<>(ActionType::main, "qload1");
	TAction<> main_qload2 = TAction<>(ActionType::main, "qload2");
	TAction<> main_qload3 = TAction<>(ActionType::main, "qload3");

	TAction<> main_key_stick = TAction<>(ActionType::main, "keystick");
	TAction<> main_auto_fire = TAction<>(ActionType::main, "autofire");
	TAction<> main_save_ram = TAction<>(ActionType::main, "saveram");
	TAction<> main_save_all = TAction<>(ActionType::main, "saveall");
	TAction<> main_lock_mouse = TAction<>(ActionType::main, "lockmouse");
	TAction<> main_atm_keyboard = TAction<>(ActionType::main, "xtkbd");
	TAction<> main_paste_text = TAction<>(ActionType::main, "pastetext");
	TAction<> main_size1 = TAction<>(ActionType::main, "size1");
	TAction<> main_size2 = TAction<>(ActionType::main, "size2");
	TAction<> main_size_max = TAction<>(ActionType::main, "sizem");
	TAction<> main_mem_search = TAction<>(ActionType::main, "memsearch");
	TAction<> main_help = TAction<>(ActionType::main, "help");
	TAction<> main_tsu_toggle = TAction<>(ActionType::main, "tsutoggle");
	TAction<> main_flic_toggle = TAction<>(ActionType::main, "flictoggle");

	TAction<> atm_keyboard = TAction<>(ActionType::xt, "xtkbd");

	TAction<> banks_up = TAction<>(ActionType::banks, "up");
	TAction<> banks_down = TAction<>(ActionType::banks, "down ");
	TAction<> banks_edit = TAction<>(ActionType::banks, "edit");

	TAction<> mem_left = TAction<>(ActionType::memory, "left");
	TAction<> mem_right = TAction<>(ActionType::memory, "right");
	TAction<> mem_up = TAction<>(ActionType::memory, "up");
	TAction<> mem_down = TAction<>(ActionType::memory, "down");
	TAction<> mem_pg_up = TAction<>(ActionType::memory, "pgup");
	TAction<> mem_pg_down = TAction<>(ActionType::memory, "pgdn");

	TAction<> mem_switch = TAction<>(ActionType::memory, "switch");
	TAction<> mem_start_line = TAction<>(ActionType::memory, "stline");
	TAction<> mem_end_line = TAction<>(ActionType::memory, "endline");
	TAction<> mem_find_text = TAction<>(ActionType::memory, "findtext");
	TAction<> mem_find_code = TAction<>(ActionType::memory, "findcode");

	TAction<> mem_goto = TAction<>(ActionType::memory, "goto");
	TAction<> mem_view = TAction<>(ActionType::memory, "mem");
	TAction<> mem_disk_phys = TAction<>(ActionType::memory, "diskphys");
	TAction<> mem_disk_log = TAction<>(ActionType::memory, "disklog");
	TAction<> mem_disk_go = TAction<>(ActionType::memory, "diskgo");

	TAction<> mem_view_pc = TAction<>(ActionType::memory, "pc");
	TAction<> mem_view_sp = TAction<>(ActionType::memory, "sp");
	TAction<> mem_view_bc = TAction<>(ActionType::memory, "bc");
	TAction<> mem_view_de = TAction<>(ActionType::memory, "de");
	TAction<> mem_view_hl = TAction<>(ActionType::memory, "hl");
	TAction<> mem_view_ix = TAction<>(ActionType::memory, "ix");
	TAction<> mem_view_iy = TAction<>(ActionType::memory, "iy");

	TAction<> reg_left = TAction<>(ActionType::reg, "left");
	TAction<> reg_right = TAction<>(ActionType::reg, "right");
	TAction<> reg_up = TAction<>(ActionType::reg, "up");
	TAction<> reg_down = TAction<>(ActionType::reg, "down");
	TAction<> reg_edit = TAction<>(ActionType::reg, "edit");

	TAction<> reg_a = TAction<>(ActionType::reg, "a");
	TAction<> reg_f = TAction<>(ActionType::reg, "f");
	TAction<> reg_bc = TAction<>(ActionType::reg, "bc");
	TAction<> reg_de = TAction<>(ActionType::reg, "de");
	TAction<> reg_hl = TAction<>(ActionType::reg, "hl");
	TAction<> reg_pc = TAction<>(ActionType::reg, "pc");
	TAction<> reg_sp = TAction<>(ActionType::reg, "sp");
	TAction<> reg_ix = TAction<>(ActionType::reg, "ix");
	TAction<> reg_iy = TAction<>(ActionType::reg, "iy");
	TAction<> reg_i = TAction<>(ActionType::reg, "i");
	TAction<> reg_r = TAction<>(ActionType::reg, "r");

	TAction<> reg_im = TAction<>(ActionType::reg, "im");
	TAction<> reg_iff1 = TAction<>(ActionType::reg, "iff1");
	TAction<> reg_iff2 = TAction<>(ActionType::reg, "iff2");

	TAction<> reg_sf = TAction<>(ActionType::reg, "SF");
	TAction<> reg_zf = TAction<>(ActionType::reg, "ZF");
	TAction<> reg_f5 = TAction<>(ActionType::reg, "F5");
	TAction<> reg_hf = TAction<>(ActionType::reg, "HF");
	TAction<> reg_f3 = TAction<>(ActionType::reg, "F3");
	TAction<> reg_pf = TAction<>(ActionType::reg, "PF");
	TAction<> reg_nf = TAction<>(ActionType::reg, "NF");
	TAction<> reg_cf = TAction<>(ActionType::reg, "CF");

	TAction<> reg_code_jump = TAction<>(ActionType::reg, "codejump");
	TAction<> reg_data_jump = TAction<>(ActionType::reg, "datajump");

	TAction<> trace_find_pc = TAction<>(ActionType::trace, "findpc");
	TAction<> trace_here = TAction<>(ActionType::trace, "here");
	TAction<> trace_find_text = TAction<>(ActionType::trace, "findtext");
	TAction<> trace_find_code = TAction<>(ActionType::trace, "findcode");
	TAction<> trace_goto = TAction<>(ActionType::trace, "goto");
	TAction<> trace_bpx = TAction<>(ActionType::trace, "bpx");
	TAction<> trace_asm = TAction<>(ActionType::trace, "asm");
	TAction<> trace_set_pc = TAction<>(ActionType::trace, "setpc");

	TAction<> trace_up = TAction<>(ActionType::trace, "up");
	TAction<> trace_down = TAction<>(ActionType::trace, "down");
	TAction<> trace_left = TAction<>(ActionType::trace, "left");
	TAction<> trace_right = TAction<>(ActionType::trace, "right");
	TAction<> trace_pg_up = TAction<>(ActionType::trace, "pgdn");
	TAction<> trace_pg_down = TAction<>(ActionType::trace, "pgup");

	TAction<> trace_save1 = TAction<>(ActionType::trace, "save1");
	TAction<> trace_save2 = TAction<>(ActionType::trace, "save2");
	TAction<> trace_save3 = TAction<>(ActionType::trace, "save3");
	TAction<> trace_save4 = TAction<>(ActionType::trace, "save4");
	TAction<> trace_save5 = TAction<>(ActionType::trace, "save5");
	TAction<> trace_save6 = TAction<>(ActionType::trace, "save6");
	TAction<> trace_save7 = TAction<>(ActionType::trace, "save7");
	TAction<> trace_save8 = TAction<>(ActionType::trace, "save8");

	TAction<> trace_restore1 = TAction<>(ActionType::trace, "crest1");
	TAction<> trace_restore2 = TAction<>(ActionType::trace, "crest2");
	TAction<> trace_restore3 = TAction<>(ActionType::trace, "crest3");
	TAction<> trace_restore4 = TAction<>(ActionType::trace, "crest4");
	TAction<> trace_restore5 = TAction<>(ActionType::trace, "crest5");
	TAction<> trace_restore6 = TAction<>(ActionType::trace, "crest6");
	TAction<> trace_restore7 = TAction<>(ActionType::trace, "crest7");
	TAction<> trace_restore8 = TAction<>(ActionType::trace, "crest8");

	TAction<> trace_back = TAction<>(ActionType::trace, "back");
	TAction<> trace_context = TAction<>(ActionType::trace, "context");
	TAction<> trace_data_jump = TAction<>(ActionType::trace, "datajump");
	TAction<> trace_labels = TAction<>(ActionType::trace, "labels");
	TAction<> trace_import_labels = TAction<>(ActionType::trace, "importl");

	TAction<> mon_gs = TAction<>(ActionType::monitor, "gs");
	TAction<> mon_osw = TAction<>(ActionType::monitor, "osw");
	TAction<> mon_settings = TAction<>(ActionType::monitor, "settings");
	TAction<> mon_bp_dialog = TAction<>(ActionType::monitor, "bpdialog");

	TFunc<unsigned, unsigned> dialog_find1 = TFunc<unsigned, unsigned>(ActionType::sys, "");
	TFunc<unsigned, unsigned> dialog_find2 = TFunc<unsigned, unsigned>(ActionType::sys, "");

	TFunc<char, MenuDef&> handle_menu = TFunc<char, MenuDef&>(ActionType::sys, "");
	TAction<HWND> on_paint = TAction<HWND>(ActionType::sys, "");
	TAction<dbgwnd> set_active_dbg = TAction<dbgwnd>(ActionType::sys, "");

};

auto get_actions(const std::vector<ActionType>& types)->std::vector<TAction<>*>;
extern Actions actions;