#include "std.h"
#include "view.h"
#include "font16.h"
#include "cpu_manager.h"
#include "util.h"
#include "debugger/consts.h"

char str[80];

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

DebugView::DebugView(HWND wnd) : wnd_(wnd)
{
	gdibuf_ = static_cast<u8*>(malloc(DBG_GDIBUFSZ));
	gdibmp_ = { { { sizeof(BITMAPINFOHEADER), DEBUG_WND_WIDTH, -DEBUG_WND_HEIGHT, 1, 8, BI_RGB, 0 } } };
	txtscr_ = static_cast<u8*>(malloc(debug_text_width * debug_text_height * 2));
}

auto DebugView::on_paint(HWND hwnd) const -> void
{
	PAINTSTRUCT ps;

	const auto bptr = gdibuf_;
	const auto hdc = BeginPaint(hwnd, &ps);
	SetDIBitsToDevice(hdc, 0, 0, DEBUG_WND_WIDTH, DEBUG_WND_HEIGHT, 0, 0, 0, DEBUG_WND_HEIGHT, bptr, &gdibmp_.header, DIB_RGB_COLORS);
	EndPaint(hwnd, &ps);
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
	for (auto& frame : frames_)
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
	frames_.push_back(TFrame{ x, y, dx, dy, attr });
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

auto DebugView::clear_canvas() -> void
{
	memset(txtscr_, backgr_ch, sizeof txtscr_ / 2);
	memset(txtscr_ + sizeof txtscr_ / 2, backgr, sizeof txtscr_ / 2);

	frames_.clear();
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

auto DebugView::wtline(const char* name, unsigned ptr, unsigned y) const -> void
{
	char line[40];
	if (name)
		sprintf(line, "%3s: ", name);
	else
		sprintf(line, "%04X ", ptr);

	auto& cpu = TCpuMgr::get_cpu();
	for (unsigned dx = 0; dx < 8; dx++)
	{
		const auto c = cpu.DirectRm(ptr++);
		sprintf(line + 5 + 3 * dx, "%02X", c);
		line[7 + 3 * dx] = ' ';
		line[29 + dx] = c ? c : '.';
	}

	line[37] = 0;
	tprint(wat_x, wat_y + y, line, w_other);
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