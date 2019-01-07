#include "std.h"
#include "emul.h"
#include "vars.h"
#include "draw.h"
#include "dx.h"
#include "debug.h"
#include "dbgreg.h"
#include "dbgtrace.h"
#include "dbgmem.h"
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

#ifdef MOD_MONITOR

u8 trace_labels;

unsigned show_scrshot;
unsigned user_watches[3] = { 0x4000, 0x8000, 0xC000 };

unsigned mem_sz = 8;
unsigned mem_disk, mem_track, mem_max;
u8 mem_ascii;
u8 mem_dump;
u8 editor = ed_mem;

unsigned regs_curs;
unsigned dbg_extport;
u8 dgb_extval; // extended memory port like 1FFD or DFFD

unsigned ripper; // ripper mode (none/read/write)

dbgwnd activedbg = dbgwnd::trace;

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

void handle_mouse()
{
	auto& cpu = TCpuMgr::get_cpu();

	const unsigned mx = ((mousepos & 0xFFFF) - temp.gx) / 8;
	const unsigned my = (((mousepos >> 16) & 0x7FFF) - temp.gy) / 16;

	if (my >= trace_y && my < trace_y + trace_size && mx >= trace_x && mx < trace_x + 32)
	{
		needclr++; activedbg = dbgwnd::trace;
		cpu.trace_curs = cpu.trpc[my - trace_y];
		if (mx - trace_x < cs[1][0]) cpu.trace_mode = 0;
		else if (mx - trace_x < cs[2][0]) cpu.trace_mode = 1;
		else cpu.trace_mode = 2;
	}
	if (my >= mem_y && my < mem_y + mem_size && mx >= mem_x && mx < mem_x + 37)
	{
		needclr++; activedbg = dbgwnd::mem;
		const auto dx = mx - mem_x;
		if (mem_dump)
		{
			if (dx >= 5)
				cpu.mem_curs = cpu.mem_top + (dx - 5) + (my - mem_y) * 32;
		}
		else
		{
			const auto mem_se = (dx - 5) % 3;
			if (dx >= 29) cpu.mem_curs = cpu.mem_top + (dx - 29) + (my - mem_y) * 8, mem_ascii = 1;
			if (dx >= 5 && mem_se != 2 && dx < 29)
				cpu.mem_curs = cpu.mem_top + (dx - 5) / 3 + (my - mem_y) * 8,
				cpu.mem_second = mem_se, mem_ascii = 0;
		}
	}
	if (mx >= regs_x && my >= regs_y && mx < regs_x + 32 && my < regs_y + 4) {
		needclr++; activedbg = dbgwnd::regs;
		for (unsigned i = 0; i < regs_layout_count; i++) {
			unsigned delta = 1;
			if (regs_layout[i].width == 16) delta = 4;
			if (regs_layout[i].width == 8) delta = 2;
			if (my - regs_y == regs_layout[i].y && mx - regs_x - regs_layout[i].x < delta) regs_curs = i;
		}
	}
	if (mx >= banks_x && my >= banks_y + 1 && mx < banks_x + 7 && my < banks_y + 5) {
		needclr++; activedbg = dbgwnd::banks;
		selbank = my - (banks_y + 1); showbank = true;
	}
	else showbank = false;

	if (mousepos & 0x80000000) { // right-click
		enum { idm_bpx = 1, idm_some_other };
		const auto menu = CreatePopupMenu();
		if (activedbg == dbgwnd::trace) {
			AppendMenu(menu, MF_STRING, idm_bpx, "breakpoint");
		}
		else {
			AppendMenu(menu, MF_STRING, 0, "I don't know");
			AppendMenu(menu, MF_STRING, 0, "what to place");
			AppendMenu(menu, MF_STRING, 0, "to menu, so");
			AppendMenu(menu, MF_STRING, 0, "No Stuff Here");
		}
		POINT globalpos; GetCursorPos(&globalpos);
		const auto cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_LEFTALIGN | TPM_TOPALIGN,
			globalpos.x,
			globalpos.y, 0, wnd, nullptr);
		DestroyMenu(menu);
		if (cmd == idm_bpx) cbpx();
		//if (cmd == IDM_SOME_OTHER) some_other();
		//needclr++;
	}
	mousepos = 0;
}

void TCpuMgr::copy_to_prev()
{
	for (unsigned i = 0; i < count; i++)
		prev_cpus_[i] = TZ80State(*cpus_[i]);
}

DebugCore* DebugCore::instance_ = nullptr;

auto DebugView::format_item(char* dst, unsigned width, const char* text, MenuItem::flags_t flags) -> void
{
	memset(dst, ' ', width + 2); dst[width + 2] = 0;
	unsigned sz = strlen(text), left = 0;
	if (sz > width) sz = width;
	if (flags & MenuItem::right) left = width - sz;
	else if (flags & MenuItem::center) left = (width - sz) / 2;
	memcpy(dst + left + 1, text, sz);
}

auto DebugView::menu_move(MenuDef* menu, int dir) -> void
{
	const unsigned start = menu->pos;
	for (;;) {
		menu->pos += dir;
		if (int(menu->pos) == -1) menu->pos = menu->n_items - 1;
		if (menu->pos >= menu->n_items) menu->pos = 0;
		if (!(menu->items[menu->pos].flags & MenuItem::disabled)) return;
		if (menu->pos == start) return;
	}
}

auto DebugView::paint_items(MenuDef* menu) -> void
{
	char ln[debug_text_width]; unsigned item;

	unsigned maxlen = strlen(menu->title);
	for (item = 0; item < menu->n_items; item++) {
		unsigned sz = strlen(menu->items[item].text);
		maxlen = max(maxlen, sz);
	}
	const unsigned menu_dx = maxlen + 2;
	const unsigned menu_dy = menu->n_items + 3;
	const unsigned menu_x = (debug_text_width - menu_dx) / 2;
	const unsigned menu_y = (debug_text_height - menu_dy) / 2;
	filledframe(menu_x, menu_y, menu_dx, menu_dy, menu_inside);
	format_item(ln, maxlen, menu->title, MenuItem::center);
	tprint(menu_x, menu_y, ln, menu_header);

	for (/*unsigned*/ item = 0; item < menu->n_items; item++) {
		u8 color = menu_item;
		if (menu->items[item].flags & MenuItem::disabled) color = menu_item_dis;
		else if (item == menu->pos) color = menu_cursor;
		format_item(ln, maxlen, menu->items[item].text, menu->items[item].flags);
		tprint(menu_x, menu_y + item + 2, ln, color);
	}
}

DebugView::DebugView(HWND wnd): wnd_(wnd)
{
	gdibuf_ = static_cast<u8*>(malloc(DBG_GDIBUFSZ));
	gdibmp_ = { { { sizeof(BITMAPINFOHEADER), DEBUG_WND_WIDTH, -DEBUG_WND_HEIGHT, 1, 8, BI_RGB, 0 } } };
	txtscr_ = static_cast<u8*>(malloc(debug_text_width * debug_text_height * 2));
}

auto DebugView::show_dialog(LPCSTR lpTemplateName, DLGPROC lpDialogFunc) const -> void
{
	DialogBox(hIn, lpTemplateName, wnd_, lpDialogFunc);
}

auto DebugView::flip() -> void
{
	for (auto y = 0; y < debug_text_height; y++)
		for (auto x = 0; x < debug_text_width; x++)
		{
			const auto atr = txtscr_[(y * debug_text_width) + x + debug_text_size];
			if (atr == 0xFF) continue;   // transparent color
			const auto chr = txtscr_[(y * debug_text_width) + x];
			auto bp = (y * DEBUG_WND_WIDTH * 16) + (x * 8);

			for (auto by = 0; by < 16; by++)
			{
				auto f = font16[(chr * 16) + by];

				for (auto bx = 0; bx < 8; bx++)
				{
					gdibuf_[bp + bx] = (f & 0x80) ? (atr & 0xF) : (atr >> 4);
					f <<= 1;
				}

				bp += DEBUG_WND_WIDTH;
			}
		}

	// show frames
	for(auto& frame : frames_)
	{
		const u8 a1 = (frame.c | 0x08) * 0x11;
		auto y = frame.y * 16 - 1;

		for (auto x = 8 * frame.x - 1; x < (frame.x + frame.dx) * 8; x++)
			gdibuf_[y * DEBUG_WND_WIDTH + x] = a1;

		y = (frame.y + frame.dy) * 16;
		for (auto x = 8 * frame.x - 1; x < (frame.x + frame.dx) * 8; x++)
			gdibuf_[y * DEBUG_WND_WIDTH + x] = a1;

		auto x = frame.x * 8 - 1;
		for (y = 16 * frame.y; y < (frame.y + frame.dy) * 16; y++)
			gdibuf_[y * DEBUG_WND_WIDTH + x] = a1;

		x = (frame.x + frame.dx) * 8;
		for (y = 16 * frame.y; y < (frame.y + frame.dy) * 16; y++)
			gdibuf_[y * DEBUG_WND_WIDTH + x] = a1;
	}

	InvalidateRect(wnd_, nullptr, FALSE);
}

auto DebugView::add_frame(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 attr) -> void
{
	frames_.push_back(TFrame{x, y, dx, dy, attr});
}

auto DebugView::handle_menu(MenuDef* menu) -> char
{
	if (menu->items[menu->pos].flags & MenuItem::disabled)
		menu_move(menu, 1);
	for (;;)
	{
		paint_items(menu);
		flip();

		unsigned key;
		for (;; Sleep(20))
		{
			key = process_msgs();
			needclr = 0;
			flip();

			if (mousepos)
				return 0;
			if (key)
				break;
		}
		if (key == VK_ESCAPE)
			return 0;
		if (key == VK_RETURN || key == VK_SPACE)
			return 1;
		if (key == VK_UP || key == VK_LEFT)
			menu_move(menu, -1);
		if (key == VK_DOWN || key == VK_RIGHT)
			menu_move(menu, 1);
		if (key == VK_HOME || key == VK_PRIOR)
		{
			menu->pos = -1;
			menu_move(menu, 1);
		}
		if (key == VK_END || key == VK_NEXT)
		{
			menu->pos = menu->n_items;
			menu_move(menu, -1);
		}
	}
}

auto DebugView::tprint(unsigned x, unsigned y, const char* str, u8 attr) const -> void
{
	for (unsigned ptr = y * debug_text_width + x; *str; str++, ptr++) {
		txtscr_[ptr] = *str;
		txtscr_[ptr + debug_text_width * debug_text_height] = attr;
	}
}

auto DebugView::tprint_fg(unsigned x, unsigned y, const char* str, u8 attr) const -> void
{
	for (auto ptr = y * debug_text_width + x; *str; str++, ptr++) {
		txtscr_[ptr] = *str;
		txtscr_[ptr + debug_text_width * debug_text_height] = (txtscr_[ptr + debug_text_width * debug_text_height] & 0xF0) + attr;
	}
}

auto DebugView::filledframe(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color) -> void
{
	for (auto yy = y; yy < (y + dy); yy++)
		for (auto xx = x; xx < (x + dx); xx++)
			txtscr_[yy * debug_text_width + xx] = ' ',
			txtscr_[yy * debug_text_width + xx + debug_text_height * debug_text_width] = color;

	frames_.clear();
	add_frame(x, y, dx, dy, fframe_frame);
}

auto DebugView::fillattr(unsigned x, unsigned y, unsigned dx, u8 color) const -> void
{
	for (auto xx = x; xx < (x + dx); xx++)
		txtscr_[y* debug_text_width + xx + debug_text_height * debug_text_width] = color;
}

auto DebugView::fillrect(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color) const -> void
{
	for (auto yy = y; yy < (y + dy); yy++)
		for (auto xx = x; xx < (x + dx); xx++)
			txtscr_[yy * debug_text_width + xx] = ' ',
			txtscr_[yy * debug_text_width + xx + debug_text_height * debug_text_width] = color;
}

auto DebugView::inputhex(unsigned x, unsigned y, unsigned sz, bool hex) -> unsigned
{
	unsigned cr = 0;
	mousepos = 0;

	for (;;)
	{
		str_[sz] = 0;

		unsigned i;
		for (i = strlen(str_); i < sz; i++)
			str_[i] = ' ';
		for (i = 0; i < sz; i++)
		{
			unsigned vl = u8(str_[i]);
			tprint(x + i, y, reinterpret_cast<char*>(&vl), (i == cr) ? w_inputcur : w_inputbg);
		}

		flip();

		unsigned key;
		for (;; Sleep(20))
		{
			key = process_msgs();
			needclr = 0;
			flip();

			if (mousepos)
				return 0;
			if (key)
				break;
		}

		switch (key)
		{
		case VK_ESCAPE: return 0;
		case VK_RETURN:
			for (auto ptr = str_ + sz - 1; *ptr == ' ' && ptr >= str_; *ptr-- = 0) {}
			return 1;
		case VK_LEFT:
			if (cr)
				cr--;
			continue;
		case VK_BACK:
			if (cr)
			{
				for (i = cr; i < sz; i++)
					str_[i - 1] = str_[i];
				str_[sz - 1] = ' ';
				--cr;
			}
			continue;
		case VK_RIGHT:
			if (cr != sz - 1)
				cr++;
			continue;
		case VK_HOME:
			cr = 0;
			continue;
		case VK_END:
			for (cr = sz - 1; cr && str_[cr] == ' ' && str_[cr - 1] == ' '; cr--) {}
			continue;
		case VK_DELETE:
			for (i = cr; i < sz - 1; i++)
				str_[i] = str_[i + 1];
			str_[sz - 1] = ' ';
			continue;
		case VK_INSERT:
			for (i = sz - 1; i > cr; i--)
				str_[i] = str_[i - 1];
			str_[cr] = ' ';
			continue;
		default:;
		}

		if (hex)
		{
			if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'F'))
				str_[cr++] = u8(key);
		}
		else
		{
			u8 Kbd[256];
			GetKeyboardState(Kbd);
			u16 k;
			if (ToAscii(key, 0, Kbd, &k, 0) == 1)
			{
				char m;
				if (CharToOemBuff(reinterpret_cast<char *>(&k), &m, 1))
					str_[cr++] = m;
			}
		}
		if (cr == sz)
			cr--;
	}
}

auto DebugView::input4(unsigned x, unsigned y, unsigned val) -> unsigned
{
	sprintf(str_, "%04X", val);
	if (inputhex(x, y, 4, true))
	{
		sscanf(str_, "%x", &val);
		return val;
	}
	return -1;
}

auto DebugView::input2(unsigned x, unsigned y, unsigned val) -> unsigned
{
	sprintf(str_, "%02X", val);
	if (inputhex(x, y, 2, true))
	{
		sscanf(str_, "%x", &val);
		return val;
	}

	return -1;
}

DebugCore::DebugCore()
{
	create_window();
	view_ = new DebugView(wnd_);
}

auto DebugCore::create_window() -> void
{
	WNDCLASS  wc{};
	RECT cl_rect;
	const DWORD dw_style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	wc.lpfnWndProc = WNDPROC(DebugWndProc);
	wc.hInstance = hIn;
	wc.lpszClassName = "DEBUG_WND";
	wc.hIcon = LoadIcon(hIn, MAKEINTRESOURCE(IDI_MAIN));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc);

	menu_ = LoadMenu(hIn, MAKEINTRESOURCE(IDR_DEBUGMENU));

	wnd_ = CreateWindow("DEBUG_WND", "UnrealSpeccy debugger", dw_style, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, nullptr, menu_, hIn, NULL);

	cl_rect.left = 0;
	cl_rect.top = 0;
	cl_rect.right = DEBUG_WND_WIDTH - 1;
	cl_rect.bottom = DEBUG_WND_HEIGHT - 1;
	AdjustWindowRect(&cl_rect, dw_style, GetMenu(wnd_) != nullptr);
	SetWindowPos(wnd_, nullptr, 0, 0, cl_rect.right - cl_rect.left + 1, cl_rect.bottom - cl_rect.top + 1, SWP_NOMOVE);

	for (unsigned i = 0; i < 0x100; i++)
	{
		const unsigned y = (i & 8) ? 0xFF : 0xC0;
		const unsigned r = (i & 2) ? y : 0;
		const unsigned g = (i & 4) ? y : 0;
		const unsigned b = (i & 1) ? y : 0;

		gdibmp.header.bmiColors[i].rgbRed = r;
		gdibmp.header.bmiColors[i].rgbGreen = g;
		gdibmp.header.bmiColors[i].rgbBlue = b;
	}

	init_tsconf();
}

auto DebugCore::debug(Z80* cpu) const -> void
{
	sound_stop();
	temp.mon_scale = temp.scale;
	temp.scale = 1;
	needclr = 1;
	dbgbreak = 1;
	flip();
	const auto oldrflags = temp.rflags;
	temp.rflags = RF_MONITOR;
	//set_video();
	ShowWindow(wnd_, SW_SHOW);

	TCpuMgr::set_current_cpu(cpu->GetIdx());
	auto prevcpu = &TCpuMgr::prev_cpu(cpu->GetIdx());
	cpu->trace_curs = cpu->pc;
	cpu->dbg_stopsp = cpu->dbg_stophere = -1;
	cpu->dbg_loop_r1 = 0, cpu->dbg_loop_r2 = 0xFFFF;
	mousepos = 0;

	while (dbgbreak) // debugger event loop
	{
		if (trace_labels)
			mon_labels.notify_user_labels();

		cpu = &TCpuMgr::get_cpu();
		prevcpu = &TCpuMgr::prev_cpu(cpu->GetIdx());
	repaint_dbg:
		cpu->trace_top &= 0xFFFF;
		cpu->trace_curs &= 0xFFFF;

		debugscr();
		if (cpu->trace_curs < cpu->trace_top || cpu->trace_curs >= cpu->trpc[trace_size] || asmii == UINT_MAX)
		{
			cpu->trace_top = cpu->trace_curs;
			debugscr();
		}

		view_->flip();

	sleep:
		while (!dispatch(nullptr))
		{
			if (mousepos)
				handle_mouse();
			if (needclr)
			{
				needclr--;
				goto repaint_dbg;
			}
			if (!dbgbreak)
				goto leave_dbg;	/* ugh... too much gotos... */
			Sleep(20);
		}

		if (activedbg == dbgwnd::regs && dispatch_more(ac_regs) > 0) continue;
		if (activedbg == dbgwnd::trace && dispatch_more(ac_trace) > 0) continue;
		if (activedbg == dbgwnd::mem && dispatch_more(ac_mem) > 0) continue;
		if (activedbg == dbgwnd::banks && dispatch_more(ac_banks) > 0) continue;
		if (activedbg == dbgwnd::regs && dispatch_regs()) continue;
		if (activedbg == dbgwnd::trace && dispatch_trace()) continue;
		if (activedbg == dbgwnd::mem && dispatch_mem()) continue;
		if (activedbg == dbgwnd::banks && dispatch_banks()) continue;
		if (needclr)
		{
			needclr--;
			continue;
		}
		goto sleep;
	}

leave_dbg:
	*prevcpu = TZ80State(*cpu);
	//   CpuMgr.CopyToPrev();
	cpu->SetLastT();
	temp.scale = temp.mon_scale;
	//temp.rflags = RF_GDI; // facepalm.jpg
	temp.rflags = oldrflags;
	//apply_video();
	ShowWindow(wnd_, SW_HIDE);
	sound_play();
	input.nokb = 20;
}

auto DebugCore::debug_events(Z80* cpu) const -> void
{
	const auto pc = cpu->pc & 0xFFFF;
	const auto membit = cpu->membits + pc;
	*membit |= MEMBITS_X;
	cpu->dbgbreak |= (*membit & MEMBITS_BPX);
	dbgbreak |= (*membit & MEMBITS_BPX);

	if (pc == cpu->dbg_stophere)
	{
		cpu->dbgbreak = 1;
		dbgbreak = 1;
	}

	if ((cpu->sp & 0xFFFF) == cpu->dbg_stopsp)
	{
		if (pc > cpu->dbg_stophere && pc < cpu->dbg_stophere + 0x100)
		{
			cpu->dbgbreak = 1;
			dbgbreak = 1;
		}
		if (pc < cpu->dbg_loop_r1 || pc > cpu->dbg_loop_r2)
		{
			cpu->dbgbreak = 1;
			dbgbreak = 1;
		}
	}

	debug_cond_check(cpu);
	brk_port_in = brk_port_out = -1; // reset only when breakpoints active
	brk_mem_rd = brk_mem_wr = -1;	// reset only when breakpoints active

	if (cpu->dbgbreak)
		debug(cpu);
}

auto DebugCore::debug_cond_check(Z80* cpu) -> void
{
	if (cpu->cbpn)
	{
		cpu->r_low = (cpu->r_low & 0x7F) + cpu->r_hi;
		for (unsigned i = 0; i < cpu->cbpn; i++)
		{
			if (calc(cpu, cpu->cbp[i]))
			{
				cpu->dbgbreak |= 1;
				dbgbreak |= 1;
			}
		}
	}
}

auto DebugCore::get_instance() -> DebugCore*
{
	if (instance_ == nullptr)
	{
		instance_ = new DebugCore();
		return instance_;
	}

	return instance_;
}

auto DebugCore::get_view() -> DebugView*
{
	return get_instance()->view_;
}

#endif // MOD_MONITOR

u8 isbrk(const Z80 &cpu) // is there breakpoints active or any other reason to use debug z80 loop?
{
#ifndef MOD_DEBUGCORE
	return 0;
#else

#ifdef MOD_MEMBAND_LED
	if (conf.led.memband & 0x80000000)
		return 1;
#endif

	if (conf.mem_model == MM_PROFSCORP)
		return 1; // breakpoint on read ROM switches ROM bank

#ifdef MOD_MONITOR
	if (cpu.cbpn)
		return 1;
	u8 res = 0;
	for (auto i = 0; i < 0x10000; i++)
		res |= cpu.membits[i];
	return (res & (MEMBITS_BPR | MEMBITS_BPW | MEMBITS_BPX));
#endif

#endif
}


/* ===================== */

static LRESULT APIENTRY DebugWndProc(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;

	if (uMessage == WM_CLOSE)
	{
		mon_emul();
		return 0;
	}

	if (uMessage == WM_PAINT)
	{
		const auto bptr = debug_gdibuf;
		const auto hdc = BeginPaint(hwnd, &ps);
		SetDIBitsToDevice(hdc, 0, 0, DEBUG_WND_WIDTH, DEBUG_WND_HEIGHT, 0, 0, 0, DEBUG_WND_HEIGHT, bptr, &debug_gdibmp.header, DIB_RGB_COLORS);
		EndPaint(hwnd, &ps);
		return 0L;
	}

	if (uMessage == WM_COMMAND)
	{
		switch (wparam) {
		case IDM_DEBUG_RUN: mon_emul(); break;

		case IDM_DEBUG_STEP: mon_step(); break;
		case IDM_DEBUG_STEPOVER: mon_stepover(); break;
		case IDM_DEBUG_TILLRETURN: mon_exitsub(); break;
		case IDM_DEBUG_RUNTOCURSOR: chere(); break;

		case IDM_BREAKPOINT_TOGGLE: cbpx(); break;
		case IDM_BREAKPOINT_MANAGER: mon_bpdialog(); break;

		case IDM_MON_LOADBLOCK: mon_load(); break;
		case IDM_MON_SAVEBLOCK: mon_save(); break;
		case IDM_MON_FILLBLOCK: mon_fill(); break;

		case IDM_MON_RIPPER: mon_tool(); break;
		default: ;
		}
		needclr = 1;
	}

	return DefWindowProc(hwnd, uMessage, wparam, lparam);
}
