#pragma once
#include <vector>
#include "sysdefs.h"
#include "vars.h"
#include "debugger/consts.h"

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

enum class dbgwnd
{
	no = 0,
	mem,
	trace,
	regs,
	banks
};

class DebugView final
{
	u8 *gdibuf_{};
	u8 *txtscr_{};
	
	std::vector<TFrame> frames_{};
	char str_[0x80]{};

	static auto format_item(char *dst, unsigned width, const char *text, MenuItem::flags_t flags) -> void;
	static auto menu_move(MenuDef *menu, int dir) -> void;

	auto paint_items(MenuDef *menu) -> void;

public:
	HWND wnd_{};
	GDIBMP gdibmp_{};
	dbgwnd activedbg = dbgwnd::trace;;

	explicit DebugView(HWND wnd);

	auto on_paint(HWND hwnd) const ->void;
	auto show_dialog(__in LPCSTR lpTemplateName, __in_opt DLGPROC lpDialogFunc) const -> void;

	auto set_scr(u32 addr, u8 val) const -> void;

	auto flip() -> void;
	auto add_frame(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 attr) -> void;
	auto handle_menu(MenuDef *menu) -> char;

	auto clear_canvas() -> void;

	auto tprint(unsigned x, unsigned y, const char *str, u8 attr) const -> void;
	auto tprint_fg(unsigned x, unsigned y, const char *str, u8 attr) const -> void;
	auto filledframe(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color = fframe_inside) -> void;
	auto fillattr(unsigned x, unsigned y, unsigned dx, u8 color = fframe_inside) const -> void;
	auto fillrect(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color = fframe_inside) const -> void;
	auto wtline(const char *name, unsigned ptr, unsigned y) const -> void;

	auto inputhex(unsigned x, unsigned y, unsigned sz, bool hex) -> unsigned;
	auto input4(unsigned x, unsigned y, unsigned val) -> unsigned;
	auto input2(unsigned x, unsigned y, unsigned val) -> unsigned;
};
