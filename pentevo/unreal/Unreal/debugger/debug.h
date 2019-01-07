#pragma once
#include <vector>

enum class dbgwnd
{
	no = 0,
	mem,
	trace,
	regs,
	banks
};

enum { ed_mem, ed_phys, ed_log, ed_cmos, ed_nvram, ed_max };

class TCpuMgr final
{
	static const unsigned count;
	static Z80* cpus_[];
	static TZ80State prev_cpus_[];
	static unsigned current_cpu_;
public:
	static Z80 &get_cpu() { return *cpus_[current_cpu_]; }
	static Z80 &get_cpu(const u32 idx) { return *cpus_[idx]; }
	static TZ80State &prev_cpu(const u32 idx) { return prev_cpus_[idx]; }
	static TZ80State &prev_cpu() { return prev_cpus_[current_cpu_]; }
	static void switch_cpu();
	static unsigned get_current_cpu() { return current_cpu_; }
	static void set_current_cpu(const u32 idx) { current_cpu_ = idx; }
	static void copy_to_prev();
	static unsigned get_count() { return count; }
};

// debug breakpoints format descriptor
typedef struct
{
	unsigned reg;       // token string (e.g. 'DOS','OUT')
	const void *ptr;    // pointer to variable, containing queried value
	u8 size;            // size of variable (1, 2, 4) or 0 for address of function: bool func()
} BPXR;

struct MenuItem final
{
	const char *text;
	enum flags_t { disabled = 1, left = 2, right = 4, center = 8 } flags;
};

struct MenuDef final
{
	MenuItem *items;
	unsigned n_items;
	const char *title;
	unsigned pos;
};

struct TFrame
{
	unsigned x;
	unsigned y;
	unsigned dx;
	unsigned dy;
	
	u8 c;
};

class DebugView
{
	const HWND wnd_;

	GDIBMP gdibmp_{};
	u8 *gdibuf_{};
	u8 *txtscr_{};

	std::vector<TFrame> frames_{};
	char str_[0x80]{};

	static auto format_item(char *dst, unsigned width, const char *text, MenuItem::flags_t flags) -> void;
	static auto menu_move(MenuDef *menu, int dir) -> void;

	auto paint_items(MenuDef *menu) -> void;

public:
	explicit DebugView(HWND wnd);

	auto show_dialog(__in LPCSTR lpTemplateName, __in_opt DLGPROC lpDialogFunc) const -> void;
	auto flip() -> void;
	auto add_frame(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 attr) -> void;
	auto handle_menu(MenuDef *menu) -> char;

	auto tprint(unsigned x, unsigned y, const char *str, u8 attr) const -> void;
	auto tprint_fg(unsigned x, unsigned y, const char *str, u8 attr) const -> void;
	auto filledframe(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color) -> void;
	auto fillattr(unsigned x, unsigned y, unsigned dx, u8 color) const -> void;
	auto fillrect(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color) const -> void;

	auto inputhex(unsigned x, unsigned y, unsigned sz, bool hex) -> unsigned;
	auto input4(unsigned x, unsigned y, unsigned val) -> unsigned;
	auto input2(unsigned x, unsigned y, unsigned val) -> unsigned;
};

class DebugCore
{
	static DebugCore * instance_;

	HWND wnd_{};
	HMENU menu_{};
	DebugView* view_{};

	DebugCore();

	auto create_window() ->void;
public:

	auto debug(Z80* cpu) const -> void;
	auto debug_events(Z80 *cpu) const -> void;
	static auto debug_cond_check(Z80 *cpu) -> void;

	static auto get_instance()->DebugCore*;
	static auto get_view()->DebugView*;
};

extern TCpuMgr cpu_mgr;
extern dbgwnd activedbg;
extern unsigned dbg_extport;
extern u8 dgb_extval; // extended memory port like 1FFD or DFFD

extern unsigned mem_sz;
extern unsigned mem_disk;
extern unsigned mem_track;
extern unsigned mem_max;

extern u8 mem_dump;
extern unsigned show_scrshot;
extern u8 editor;
extern u8 mem_ascii;


extern unsigned ripper; // ripper mode (none/read/write)

extern unsigned user_watches[3];
extern unsigned regs_curs;

extern u8 trace_labels;

u8 isbrk(const Z80 &cpu); // is there breakpoints active or any other reason to use debug z80 loop?
void debugscr();

