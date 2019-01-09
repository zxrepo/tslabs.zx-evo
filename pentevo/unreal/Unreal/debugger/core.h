#pragma once
#include "view.h"
#include "defs.h"
#include "dialogs.h"
#include "views/mem.h"
#include "views/reg.h"
#include "views/devs.h"
#include "views/trace.h"

namespace z80dbg
{
	void __cdecl SetLastT();
}

enum FILEDLG_MODE { FDM_LOAD = 0, FDM_SAVE, FDM_DISASM };

class DebugCore final
{
	static DebugCore * instance_;

	DebugCore& ref_ = *this;

	HWND wnd_{};
	HMENU menu_{};

	DebugView* view_{};
	MemView* mem_{};
	RegView* regs_{};
	WatchView* watch_{};
	StackView* stack_{};
	AyView* ay_{};
	BanksView* banks_{};
	PortsView* ports_{};
	DosView* dos_{};
	TimeView* time_{};

	Dialogs* dialogs_{};

	unsigned addr = 0;
	unsigned end = 0xFFFF;
	unsigned ripper{}; // ripper mode (none/read/write)

	unsigned rw_drive{};
	unsigned rw_trk{};
	unsigned rw_cyl{};
	unsigned rw_tsec{};
	unsigned rw_rsec{}; 
	unsigned rw_side{};

	char fname[20] = "", trdname[9] = "12345678", trdext[2] = "C";

	DebugCore();

	static auto APIENTRY wnd_proc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)->LRESULT;
	static auto rw_err(const char *msg) -> void;

	auto create_window() ->void;

	auto mon_emul() const -> void;
	auto mon_exitsub() const -> void;
	auto mon_step() -> void;
	auto mon_stepover() -> void;
	auto mon_switch_cpu() -> void;
	auto mon_nxt() -> void;
	auto mon_aux() -> void;
	auto mon_prv() -> void;
	auto mon_switch_dump() const -> void;
	auto mon_dump() const -> void;
	auto mon_tool() -> void;
	auto mon_fill() -> void;
	auto mon_load() -> void;
	auto mon_save() -> void;

	auto chere() const -> void;

	auto rw_trdos_sectors(FILEDLG_MODE mode, u8* memdata) -> char;
	auto wr_trdos_file(u8* memdata) -> char;
	auto query_file_addr(const FILEDLG_MODE mode) -> char;
	auto write_mem(u8* memdata) const -> void;
	auto read_mem(u8* memdata) const -> void;
	auto rw_select_drive() -> char;

public:
	dbgwnd activedbg = dbgwnd::trace;

	static auto debug_cond_check(Z80 *cpu) -> void;
	static auto get_instance()->DebugCore*;
	static auto get_view()->DebugView*;
	static auto get_dialogs()->Dialogs*;
	static auto isbrk(const Z80 &cpu)->u8;

	auto debugscr() -> void;
	auto debug(Z80* cpu) -> void;
	auto debug_events(Z80 *cpu) -> void;
	auto handle_mouse() -> void;
};