#pragma once
#include <utility>
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

template<class OutType = void, class ...InType>
class TFunc
{
	using action_method = std::function<OutType(InType...)>;
	std::vector<action_method> actions_{};

public:
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
};

template<class ...Args>
using TAction = TFunc<void, Args...>;

struct UIKey;
extern std::vector<UIKey*> actions_registry;

struct UIKey final : public TAction<>
{
	const std::string name;
	const ActionType type;
	unsigned k1{}, k2{}, k3{}, k4{};

	UIKey(ActionType type, std::string name);

	auto is_empty() const -> bool;
	auto get_full_name() const -> std::string;
};

class Actions final
{
public:
	UIKey mon_emul{ ActionType::monitor, "emul" };
	UIKey mon_save_block{ ActionType::monitor, "saveblock" };
	UIKey mon_load_block{ ActionType::monitor, "loadblock" };
	UIKey mon_fill_block{ ActionType::monitor, "fillblock" };
	UIKey mon_step{ ActionType::monitor, "step" };
	UIKey mon_step_over{ ActionType::monitor, "stepover" };
	UIKey mon_exit_sub{ ActionType::monitor, "exitsub" };
	UIKey mon_dump{ ActionType::monitor, "dump" };

	UIKey mon_switch_dump{ ActionType::monitor, "switchdump" };
	UIKey mon_next{ ActionType::monitor, "next" };
	UIKey mon_prev{ ActionType::monitor, "prev" };
	UIKey mon_ripper{ ActionType::monitor, "rip" };
	UIKey mon_help{ ActionType::monitor, "help" };
	UIKey mon_cpu{ ActionType::monitor, "cpu" };
	UIKey mon_exit{ ActionType::monitor, "exit" };

	UIKey mon_poke_dialog{ ActionType::monitor, "pokedialog" };
	UIKey mon_tape_browser{ ActionType::monitor, "tapebrowser" };
	UIKey mon_reset{ ActionType::monitor, "reset" };
	UIKey mon_reset128{ ActionType::monitor, "reset128" };
	UIKey mon_reset_sys{ ActionType::monitor, "resetsys" };
	UIKey mon_reset48{ ActionType::monitor, "reset48" };
	UIKey mon_reset_basic{ ActionType::monitor, "resetbasic" };
	UIKey mon_reset_dos{ ActionType::monitor, "resetdos" };
	UIKey mon_reset_cache{ ActionType::monitor, "resetcache" };
	UIKey mon_set_watch{ ActionType::monitor, "setwatch" };
	UIKey mon_screenshot{ ActionType::monitor, "scrshot" };
	UIKey mon_switch_ay{ ActionType::monitor, "switchay" };

	UIKey mon_nmi{ ActionType::monitor, "nmi" };
	UIKey mon_nmi_dos{ ActionType::monitor, "nmidos" };
	UIKey mon_nmi_cache{ ActionType::monitor, "nmicache" };
	UIKey mon_save_snap{ ActionType::monitor, "save" };
	UIKey mon_load_snap{ ActionType::monitor, "load" };
	UIKey mon_save_sound{ ActionType::monitor, "savesound" };
	UIKey mon_qsave1{ ActionType::monitor, "qsave1" };
	UIKey mon_qsave2{ ActionType::monitor, "qsave2" };
	UIKey mon_qsave3{ ActionType::monitor, "qsave3" };
	UIKey mon_qload1{ ActionType::monitor, "qload1" };
	UIKey mon_qload2{ ActionType::monitor, "qload2" };
	UIKey mon_qload3{ ActionType::monitor, "qload3" };
	UIKey mon_labels{ ActionType::monitor, "labels" };
	UIKey mon_set_bank{ ActionType::monitor, "setbank" };
	UIKey mon_mem_search{ ActionType::monitor, "memsearch" };
	UIKey mon_set_himem{ ActionType::monitor, "sethimem" };

	UIKey mon_gs{ ActionType::monitor, "gs" };
	UIKey mon_osw{ ActionType::monitor, "osw" };
	UIKey mon_settings{ ActionType::monitor, "settings" };
	UIKey mon_bp_dialog{ ActionType::monitor, "bpdialog" };


	UIKey main_exit{ ActionType::main, "exit" };
	UIKey main_monitor{ ActionType::main, "monitor" };
	UIKey main_fullsceen{ ActionType::main, "fullscreen" };
	UIKey main_pause{ ActionType::main, "pause" };
	UIKey main_select_fix{ ActionType::main, "selectfix" };
	UIKey main_select_snd{ ActionType::main, "selectsnd" };
	UIKey main_inc_fix{ ActionType::main, "incfix" };
	UIKey main_dec_fix{ ActionType::main, "decfix" };
	UIKey main_inc_fix10{ ActionType::main, "incfix10" };
	UIKey main_dec_fix10{ ActionType::main, "decfix10" };

	UIKey main_leds{ ActionType::main, "leds" };
	UIKey main_status{ ActionType::main, "status" };
	UIKey main_max_speed{ ActionType::main, "maxspeed" };
	UIKey main_select_filter{ ActionType::main, "selectfilter" };
	UIKey main_select_driver{ ActionType::main, "selectdriver" };
	UIKey main_poke_dialog{ ActionType::main, "pokedialog" };
	UIKey main_start_tape{ ActionType::main, "starttape" };
	UIKey main_screenshoot{ ActionType::main, "screenshot" };
	UIKey main_save_video{ ActionType::main, "savevideo" };

	UIKey main_reset{ ActionType::main, "reset" };
	UIKey main_reset128{ ActionType::main, "reset128" };
	UIKey main_reset_sys{ ActionType::main, "resetsys" };
	UIKey main_reset48{ ActionType::main, "reset48" };
	UIKey main_reset_basic{ ActionType::main, "resetbasic" };
	UIKey main_reset_dos{ ActionType::main, "resetdos" };
	UIKey main_reset_cache{ ActionType::main, "resetcache" };

	UIKey main_nmi{ ActionType::main, "nmi" };
	UIKey main_nmi_dos{ ActionType::main, "nmidos" };
	UIKey main_nmi_cache{ ActionType::main, "nmicache" };

	UIKey main_tape_browser{ ActionType::main, "tapebrowser" };
	UIKey main_settings{ ActionType::main, "settings" };
	UIKey main_save_snap{ ActionType::main, "save" };
	UIKey main_load_snap{ ActionType::main, "load" };
	UIKey main_save_sound{ ActionType::main, "savesound" };
	UIKey main_qsave1{ ActionType::main, "qsave1" };
	UIKey main_qsave2{ ActionType::main, "qsave2" };
	UIKey main_qsave3{ ActionType::main, "qsave3" };
	UIKey main_qload1{ ActionType::main, "qload1" };
	UIKey main_qload2{ ActionType::main, "qload2" };
	UIKey main_qload3{ ActionType::main, "qload3" };

	UIKey main_key_stick{ ActionType::main, "keystick" };
	UIKey main_auto_fire{ ActionType::main, "autofire" };
	UIKey main_save_ram{ ActionType::main, "saveram" };
	UIKey main_save_all{ ActionType::main, "saveall" };
	UIKey main_lock_mouse{ ActionType::main, "lockmouse" };
	UIKey main_atm_keyboard{ ActionType::main, "xtkbd" };
	UIKey main_paste_text{ ActionType::main, "pastetext" };
	UIKey main_size1{ ActionType::main, "size1" };
	UIKey main_size2{ ActionType::main, "size2" };
	UIKey main_size_max{ ActionType::main, "sizem" };
	UIKey main_mem_search{ ActionType::main, "memsearch" };
	UIKey main_help{ ActionType::main, "help" };
	UIKey main_tsu_toggle{ ActionType::main, "tsutoggle" };
	UIKey main_flic_toggle{ ActionType::main, "flictoggle" };

	
	UIKey atm_keyboard{ ActionType::xt, "xtkbd" };

	
	UIKey banks_up{ ActionType::banks, "up" };
	UIKey banks_down{ ActionType::banks, "down " };
	UIKey banks_edit{ ActionType::banks, "edit" };


	UIKey mem_left{ ActionType::memory, "left" };
	UIKey mem_right{ ActionType::memory, "right" };
	UIKey mem_up{ ActionType::memory, "up" };
	UIKey mem_down{ ActionType::memory, "down" };
	UIKey mem_pg_up{ ActionType::memory, "pgup" };
	UIKey mem_pg_down{ ActionType::memory, "pgdn" };

	UIKey mem_switch{ ActionType::memory, "switch" };
	UIKey mem_start_line{ ActionType::memory, "stline" };
	UIKey mem_end_line{ ActionType::memory, "endline" };
	UIKey mem_find_text{ ActionType::memory, "findtext" };
	UIKey mem_find_code{ ActionType::memory, "findcode" };

	UIKey mem_goto{ ActionType::memory, "goto" };
	UIKey mem_view{ ActionType::memory, "mem" };
	UIKey mem_disk_phys{ ActionType::memory, "diskphys" };
	UIKey mem_disk_log{ ActionType::memory, "disklog" };
	UIKey mem_disk_go{ ActionType::memory, "diskgo" };

	UIKey mem_view_pc{ ActionType::memory, "pc" };
	UIKey mem_view_sp{ ActionType::memory, "sp" };
	UIKey mem_view_bc{ ActionType::memory, "bc" };
	UIKey mem_view_de{ ActionType::memory, "de" };
	UIKey mem_view_hl{ ActionType::memory, "hl" };
	UIKey mem_view_ix{ ActionType::memory, "ix" };
	UIKey mem_view_iy{ ActionType::memory, "iy" };


	UIKey reg_left{ ActionType::reg, "left" };
	UIKey reg_right{ ActionType::reg, "right" };
	UIKey reg_up{ ActionType::reg, "up" };
	UIKey reg_down{ ActionType::reg, "down" };
	UIKey reg_edit{ ActionType::reg, "edit" };

	UIKey reg_a{ ActionType::reg, "a" };
	UIKey reg_f{ ActionType::reg, "f" };
	UIKey reg_bc{ ActionType::reg, "bc" };
	UIKey reg_de{ ActionType::reg, "de" };
	UIKey reg_hl{ ActionType::reg, "hl" };
	UIKey reg_pc{ ActionType::reg, "pc" };
	UIKey reg_sp{ ActionType::reg, "sp" };
	UIKey reg_ix{ ActionType::reg, "ix" };
	UIKey reg_iy{ ActionType::reg, "iy" };
	UIKey reg_i{ ActionType::reg, "i" };
	UIKey reg_r{ ActionType::reg, "r" };

	UIKey reg_im{ ActionType::reg, "im" };
	UIKey reg_iff1{ ActionType::reg, "iff1" };
	UIKey reg_iff2{ ActionType::reg, "iff2" };

	UIKey reg_sf{ ActionType::reg, "SF" };
	UIKey reg_zf{ ActionType::reg, "ZF" };
	UIKey reg_f5{ ActionType::reg, "F5" };
	UIKey reg_hf{ ActionType::reg, "HF" };
	UIKey reg_f3{ ActionType::reg, "F3" };
	UIKey reg_pf{ ActionType::reg, "PF" };
	UIKey reg_nf{ ActionType::reg, "NF" };
	UIKey reg_cf{ ActionType::reg, "CF" };

	UIKey reg_code_jump{ ActionType::reg, "codejump" };
	UIKey reg_data_jump{ ActionType::reg, "datajump" };


	UIKey trace_find_pc{ ActionType::trace, "findpc" };
	UIKey trace_here{ ActionType::trace, "here" };
	UIKey trace_find_text{ ActionType::trace, "findtext" };
	UIKey trace_find_code{ ActionType::trace, "findcode" };
	UIKey trace_goto{ ActionType::trace, "goto" };
	UIKey trace_bpx{ ActionType::trace, "bpx" };
	UIKey trace_asm{ ActionType::trace, "asm" };
	UIKey trace_set_pc{ ActionType::trace, "setpc" };

	UIKey trace_up{ ActionType::trace, "up" };
	UIKey trace_down{ ActionType::trace, "down" };
	UIKey trace_left{ ActionType::trace, "left" };
	UIKey trace_right{ ActionType::trace, "right" };
	UIKey trace_pg_up{ ActionType::trace, "pgdn" };
	UIKey trace_pg_down{ ActionType::trace, "pgup" };

	UIKey trace_save1{ ActionType::trace, "save1" };
	UIKey trace_save2{ ActionType::trace, "save2" };
	UIKey trace_save3{ ActionType::trace, "save3" };
	UIKey trace_save4{ ActionType::trace, "save4" };
	UIKey trace_save5{ ActionType::trace, "save5" };
	UIKey trace_save6{ ActionType::trace, "save6" };
	UIKey trace_save7{ ActionType::trace, "save7" };
	UIKey trace_save8{ ActionType::trace, "save8" };

	UIKey trace_restore1{ ActionType::trace, "crest1" };
	UIKey trace_restore2{ ActionType::trace, "crest2" };
	UIKey trace_restore3{ ActionType::trace, "crest3" };
	UIKey trace_restore4{ ActionType::trace, "crest4" };
	UIKey trace_restore5{ ActionType::trace, "crest5" };
	UIKey trace_restore6{ ActionType::trace, "crest6" };
	UIKey trace_restore7{ ActionType::trace, "crest7" };
	UIKey trace_restore8{ ActionType::trace, "crest8" };

	UIKey trace_back{ ActionType::trace, "back" };
	UIKey trace_context{ ActionType::trace, "context" };
	UIKey trace_data_jump{ ActionType::trace, "datajump" };
	UIKey trace_labels{ ActionType::trace, "labels" };
	UIKey trace_import_labels{ ActionType::trace, "importl" };


	TFunc<unsigned, unsigned> dialog_find1 = TFunc<unsigned, unsigned>();
	TFunc<unsigned, unsigned> dialog_find2 = TFunc<unsigned, unsigned>();

	TFunc<char, MenuDef&> handle_menu{};
	TAction<HWND> on_paint{};
	TAction<bool> show_debug_window{};

	TAction<dbgwnd> set_active_dbg{};
	TFunc<bool, dbgwnd> is_active_dbg{};

	TAction<int> set_banks{};
	TAction<bool> show_banks{};
	
	TAction<> debug_screen{};
};

auto get_actions(const std::vector<ActionType>& types)->std::vector<UIKey*>;
extern Actions actions;
