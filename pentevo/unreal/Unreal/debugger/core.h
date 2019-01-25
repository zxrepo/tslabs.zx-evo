#pragma once
#include "defs.h"

class DebugCore;
class DebugView;
class MemView;
class RegView;
class WatchView;
class StackView;
class AyView;
class BanksView;
class PortsView;
class DosView;
class TimeView;
class TraceView;
class TsconfView;
class Dialogs;

#include "views/mem.h"
#include "views/reg.h"
#include "views/devs.h"
#include "views/trace.h"
#include "libs/dialogs.h"
#include "views/dbgtsconf.h"

namespace z80dbg
{
	void __cdecl SetLastT();
}

// debug breakpoints format descriptor
struct BPXR
{
	unsigned reg;       // token string (e.g. 'DOS','OUT')
	const void *ptr;    // pointer to variable, containing queried value
	u8 size;            // size of variable (1, 2, 4) or 0 for address of function: bool func()
};

enum FILEDLG_MODE { FDM_LOAD = 0, FDM_SAVE, FDM_DISASM };

class DebugCore final
{
	static DebugCore * instance_;

	DebugCore& ref_;
	HMENU menu_{};

	DebugView* view_ = nullptr;
	MemView* mem_ = nullptr;
	RegView* regs_ = nullptr;
	WatchView* watch_ = nullptr;
	StackView* stack_ = nullptr;
	AyView* ay_ = nullptr;
	BanksView* banks_ = nullptr;
	PortsView* ports_ = nullptr;
	DosView* dos_ = nullptr;
	TimeView* time_ = nullptr;
	TraceView* trace_ = nullptr;
	TsconfView* tsconf_ = nullptr;

	Dialogs* dialogs_ = nullptr;

	unsigned addr = 0;
	unsigned end = 0xFFFF;

	unsigned rw_drive{};
	unsigned rw_trk{};
	unsigned rw_tsec{};

	char fname[20] = "", trdname[9] = "12345678", trdext[2] = "C";

	DebugCore();

	static auto APIENTRY wnd_proc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)->LRESULT;
	static auto rw_err(const char *msg) -> void;

	auto subscrible() -> void;
	auto create_window() -> HWND;

	auto mon_emul() const -> void;
	auto mon_exitsub() const -> void;
	auto mon_step() const -> void;
	auto mon_stepover() const -> void;
	auto mon_switch_cpu() const -> void;
	auto mon_nxt() const -> void;
	auto mon_aux() const -> void;
	auto mon_prv() const -> void;
	auto mon_switch_dump() const -> void;
	auto mon_tool() -> void;
	auto mon_fill() -> void;
	auto mon_load() -> void;
	auto mon_save() -> void;

	

	auto rw_trdos_sectors(FILEDLG_MODE mode, u8* memdata) -> char;
	auto wr_trdos_file(u8* memdata) -> char;
	auto query_file_addr(FILEDLG_MODE mode) -> char;
	auto write_mem(u8* memdata) const -> void;
	auto read_mem(u8* memdata) const -> void;
	auto rw_select_drive() -> char;

	static auto correct_exit() -> void;

public:
	unsigned ripper{}; // ripper mode (none/read/write)
	
	static auto debug_cond_check(Z80 *cpu) -> void;
	static auto debug_events(Z80 *cpu) -> void;
	static auto get_instance()->DebugCore*;
	static auto init_bpx(char* file) -> void;
	static auto done_bpx() -> void;

	auto debugscr() const -> void;
	auto debug(Z80* cpu) const -> void;
	
	auto handle_mouse() const -> void;
};