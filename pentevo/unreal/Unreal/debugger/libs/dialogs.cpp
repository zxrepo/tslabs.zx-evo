#include "std.h"
#include "sysdefs.h"
#include "dialogs.h"
#include "vars.h"
#include "util.h"
#include "resource.h"
#include "debugger/consts.h"
#include "debugger/views/mem.h"
#include "cpu_manager.h"
#include "funcs.h"
#include "leds.h"
#include "core/actions/actions.h"

static bool __cdecl get_dos_flag()
{
	return (comp.flags & CF_DOSPORTS) != 0;
}

enum
{
	DB_STOP = 0,
	DB_CHAR,
	DB_SHORT,
	DB_PCHAR,
	DB_PSHORT,
	DB_PINT,
	DB_PFUNC,
};

#define DECL_REGS(var, cpu)                       \
  static BPXR var[] =                             \
  {                                               \
    { WORD4('D','O','S',0), (const void *)get_dos_flag, 0 },  \
    { WORD4('O','U','T',0), &brk_port_out, 4 },   \
    { WORD2('I','N'), &brk_port_in, 4 },          \
    { WORD4('V','A','L',0), &brk_port_val, 1 },   \
    { WORD2('R','D'), &brk_mem_rd, 4 },           \
    { WORD2('W','R'), &brk_mem_wr, 4 },           \
    { WORD4('M','D','T',0), &brk_mem_val, 1 },    \
    { WORD2('F','D'), &comp.p7FFD, 1 },           \
    { WORD4('P','G','0',0), &comp.ts.page[0], 1 },\
    { WORD4('P','G','1',0), &comp.ts.page[1], 1 },\
    { WORD4('P','G','2',0), &comp.ts.page[2], 1 },\
    { WORD4('P','G','3',0), &comp.ts.page[3], 1 },\
                                                  \
    { WORD4('A','F','\'',0), &cpu.alt.af, 2 },    \
    { WORD4('B','C','\'',0), &cpu.alt.bc, 2 },    \
    { WORD4('D','E','\'',0), &cpu.alt.de, 2 },    \
    { WORD4('H','L','\'',0), &cpu.alt.hl, 2 },    \
    { WORD2('A','\''), &cpu.alt.a, 1 },           \
    { WORD2('F','\''), &cpu.alt.f, 1 },           \
    { WORD2('B','\''), &cpu.alt.b, 1 },           \
    { WORD2('C','\''), &cpu.alt.c, 1 },           \
    { WORD2('D','\''), &cpu.alt.d, 1 },           \
    { WORD2('E','\''), &cpu.alt.e, 1 },           \
    { WORD2('H','\''), &cpu.alt.h, 1 },           \
    { WORD2('L','\''), &cpu.alt.l, 1 },           \
                                                  \
    { WORD2('A','F'), &cpu.af, 2 },               \
    { WORD2('B','C'), &cpu.bc, 2 },               \
    { WORD2('D','E'), &cpu.de, 2 },               \
    { WORD2('H','L'), &cpu.hl, 2 },               \
    { 'A', &cpu.a, 1 },                           \
    { 'F', &cpu.f, 1 },                           \
    { 'B', &cpu.b, 1 },                           \
    { 'C', &cpu.c, 1 },                           \
    { 'D', &cpu.d, 1 },                           \
    { 'E', &cpu.e, 1 },                           \
    { 'H', &cpu.h, 1 },                           \
    { 'L', &cpu.l, 1 },                           \
                                                  \
    { WORD2('P','C'), &cpu.pc, 2 },               \
    { WORD2('S','P'), &cpu.sp, 2 },               \
    { WORD2('I','X'), &cpu.ix, 2 },               \
    { WORD2('I','Y'), &cpu.iy, 2 },               \
                                                  \
    { 'I', &cpu.i, 1 },                           \
    { 'R', &cpu.r_low, 1 },                       \
  }

static char str[80];

Dialogs::Dialogs(DebugView& view, MemView& mem): view_(view), mem_(mem)
{
	subscrible();
}

auto Dialogs::find1dlg(unsigned start) const -> unsigned
{
	static char ftext[12] = "";
	strcpy(str, ftext);
	view_.filledframe(10, 10, 16, 4);
	view_.tprint(10, 10, "  find string   ", frm_header);
	view_.tprint(11, 12, "text:", fframe_inside);
	if (!view_.inputhex(17, 12, 8, false)) return -1;
	strcpy(ftext, str);
	const auto len = strlen(ftext);
	unsigned i; //Alone Coder 0.36.7
	for (auto ptr = mem_.memadr(start + 1); ptr != start; ptr = mem_.memadr(ptr + 1)) {
		for (/*unsigned*/ i = 0; i < len; i++)
			if (mem_.editrm(mem_.memadr(ptr + i)) != ftext[i]) break;
		if (i == len) return ptr;
	}
	view_.tprint(11, 12, "  not found   ", fframe_error);
	view_.flip();
	while (!process_msgs()) {}
	return -1;
}

auto Dialogs::find2dlg(unsigned start) const -> unsigned
{
	static unsigned code = 0xF3, mask = 0xFF; char ln[64];
	view_.filledframe(10, 10, 16, 5);
	view_.tprint(10, 10, "   find data    ", frm_header);
	sprintf(ln, "code: %08lX", _byteswap_ulong(code));
	view_.tprint(11, 12, ln, fframe_inside);
	sprintf(ln, "mask: %08lX", _byteswap_ulong(mask));
	view_.tprint(11, 13, ln, fframe_inside);
	sprintf(str, "%08lX", _byteswap_ulong(code));
	if (!view_.inputhex(17, 12, 8, true)) return -1;
	sscanf(str, "%x", &code); code = _byteswap_ulong(code);
	view_.tprint(17, 12, str, fframe_inside);
	sprintf(str, "%08lX", _byteswap_ulong(mask));
	if (!view_.inputhex(17, 13, 8, true)) return -1;
	sscanf(str, "%x", &mask); mask = _byteswap_ulong(mask);
	unsigned i; //Alone Coder 0.36.7
	for (auto ptr = mem_.memadr(start + 1); ptr != start; ptr = mem_.memadr(ptr + 1)) {
		const auto cd = reinterpret_cast<u8*>(&code);
		const auto ms = reinterpret_cast<u8*>(&mask);
		for (/*unsigned*/ i = 0; i < 4; i++)
			if ((mem_.editrm(mem_.memadr(ptr + i)) & ms[i]) != (cd[i] & ms[i])) break;
		if (i == 4) return ptr;
	}
	view_.tprint(11, 12, "  not found   ", fframe_error);
	view_.tprint(11, 13, "              ", fframe_error);
	view_.flip();
	while (!process_msgs());
	return -1;
}

INT_PTR CALLBACK gsdlg(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
	char tmp[0x200];
	if (msg == WM_INITDIALOG) {
	repaint:
		while (SendDlgItemMessage(dlg, IDC_GSLIST, LB_GETCOUNT, 0, 0))
			SendDlgItemMessage(dlg, IDC_GSLIST, LB_DELETESTRING, 0, 0);
		if (gs.modsize) {
			sprintf(tmp, "%-.20s (%s)", gs.mod, gs.mod_playing ? "playing" : "stopped");
			SendDlgItemMessage(dlg, IDC_GSLIST, LB_ADDSTRING, 0, LPARAM(tmp));
		}
		for (unsigned i = 1; i < gs.total_fx; i++) {
			sprintf(tmp, "%csmp %d: v=%d, n=%d, %d%s",
				gs.cur_fx == i ? '*' : ' ', i,
				gs.sample[i].volume, gs.sample[i].note, gs.sample[i].end,
				gs.sample[i].loop < gs.sample[i].end ? " (L)" : nil);
			SendDlgItemMessage(dlg, IDC_GSLIST, LB_ADDSTRING, 0, LPARAM(tmp));
		}
		*tmp = 0;
		for (auto i = 0; i < 0x100; i++) {
			if (gs.badgs[i]) sprintf(tmp + strlen(tmp), "%02X ", i);
		}
		SendDlgItemMessage(dlg, IDE_GS, WM_SETTEXT, 0, LPARAM(tmp));
		return 1;
	}
	if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_CLOSE) EndDialog(dlg, 0);
	if (msg != WM_COMMAND) return 0;
	const unsigned id = LOWORD(wp);
	if (id == IDCANCEL || id == IDOK) EndDialog(dlg, 0);
	if (id == IDB_GS_CLEAR) { memset(gs.badgs, 0, sizeof gs.badgs); SendDlgItemMessage(dlg, IDE_GS, WM_SETTEXT, 0, 0); }
	if (id == IDB_GS_RESET) { gs.reset(); goto repaint; }
	if (id == IDB_GS_PLAY || (id == IDC_GSLIST && HIWORD(wp) == LBN_DBLCLK)) {
		unsigned i = SendDlgItemMessage(dlg, IDC_GSLIST, LB_GETCURSEL, 0, 0);
		if (i > 0x100) return 1;
		if (!i && gs.modsize) {
			gs.mod_playing ^= 1;
			if (gs.mod_playing) gs.restart_mod(0, 0); else gs.stop_mod();
			goto repaint;
		}
		if (!gs.modsize) i++;
		gs.debug_note(i);
	}
	return 0;
}

auto Dialogs::mon_gsdialog() const -> void
{
	if (conf.gs_type == 2)
		view_.show_dialog(MAKEINTRESOURCE(IDD_GS), gsdlg);
	else
		MessageBox(wnd, "high-level GS emulation\nis not initialized", nullptr, MB_OK | MB_ICONERROR);
}

void clear_list_box(HWND box)
{
	while (SendMessage(box, LB_GETCOUNT, 0, 0))
		SendMessage(box, LB_DELETESTRING, 0, 0);
}

void fill_mem_box(HWND dlg, unsigned address)
{
	const auto box = GetDlgItem(dlg, IDC_MEM);
	clear_list_box(box);
	unsigned selection = 0;

	auto& cpu = TCpuMgr::get_cpu();
	unsigned end; //Alone Coder 0.36.7
	for (unsigned start = 0; start < 0x10000; ) {
		const u8 mask = MEMBITS_BPR | MEMBITS_BPW;
		if (!(cpu.membits[start] & mask)) { start++; continue; }
		const unsigned active = cpu.membits[start];
		for (/*unsigned*/ end = start; end < 0xFFFF && !((active ^ cpu.membits[end + 1]) & mask); end++) {}
		char tmp[16];
		if (start == end) sprintf(tmp, "%04X ", start);
		else sprintf(tmp, "%04X-%04X ", start, end);
		if (active & MEMBITS_BPR) strcat(tmp, "R");
		if (active & MEMBITS_BPW) strcat(tmp, "W");
		SendMessage(box, LB_ADDSTRING, 0, LPARAM(tmp));
		if (start <= address && address <= end)
			selection = SendMessage(box, LB_GETCOUNT, 0, 0);
		start = end + 1;
	}
	if (selection) SendMessage(box, LB_SETCURSEL, selection - 1, 0);
}

void fill_bpx_box(HWND dlg, unsigned address)
{
	const auto box = GetDlgItem(dlg, IDC_BPX);
	clear_list_box(box);
	unsigned selection = 0;

	auto& cpu = TCpuMgr::get_cpu();
	unsigned end; //Alone Coder 0.36.7
	for (unsigned start = 0; start < 0x10000; )
	{
		if (!(cpu.membits[start] & MEMBITS_BPX))
		{
			start++; continue;
		}
		for (/*unsigned*/ end = start; end < 0xFFFF && (cpu.membits[end + 1] & MEMBITS_BPX); end++) {}
		char tmp[16];
		if (start == end) sprintf(tmp, "%04X", start);
		else sprintf(tmp, "%04X-%04X", start, end);
		SendMessage(box, LB_ADDSTRING, 0, LPARAM(tmp));
		if (start <= address && address <= end)
			selection = SendMessage(box, LB_GETCOUNT, 0, 0);
		start = end + 1;
	}
	if (selection) SendMessage(box, LB_SETCURSEL, selection - 1, 0);
}

void script2text(char *dst, unsigned *src)
{
	char stack[64][0x200], tmp[0x200];
	unsigned sp = 0, r;

	const auto& cpu = TCpuMgr::get_cpu();

	DECL_REGS(regs, cpu);

	while ((r = *src++))
	{
		if (r == DB_CHAR)
		{
			sprintf(stack[sp++], "'%c'", *src++);
			continue;
		}
		if (r == DB_SHORT)
		{
			sprintf(stack[sp], "0%X", *src++);
			if (isdigit(stack[sp][1])) strcpy(stack[sp], stack[sp] + 1);
			sp++;
			continue;
		}
		if (r >= DB_PCHAR && r <= DB_PFUNC)
		{
			unsigned i; //Alone Coder 0.36.7
			for (/*int*/ i = 0; i < _countof(regs); i++)
			{
				if (*src == unsigned(regs[i].ptr))
					break;
			}
			*reinterpret_cast<unsigned*>(&stack[sp++]) = regs[i].reg;
			src++;
			continue;
		}
		if (r == 'M' || r == '~' || r == '!')
		{ // unary operators
			sprintf(tmp, "%c(%s)", r, stack[sp - 1]);
			strcpy(stack[sp - 1], tmp);
			continue;
		}
		// else binary operators
		sprintf(tmp, "(%s%s%s)", stack[sp - 2], reinterpret_cast<char*>(&r), stack[sp - 1]);
		sp--; strcpy(stack[sp - 1], tmp);
	}
	if (!sp)
		*dst = 0;
	else
		strcpy(dst, stack[sp - 1]);
}

void fill_cond_box(HWND dlg, unsigned cursor)
{
	const auto box = GetDlgItem(dlg, IDC_CBP);
	clear_list_box(box);

	auto& cpu = TCpuMgr::get_cpu();
	for (unsigned i = 0; i < cpu.cbpn; i++)
	{
		char tmp[0x200]; script2text(tmp, cpu.cbp[i]);
		SendMessage(box, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(tmp));
	}
	SendMessage(box, LB_SETCURSEL, cursor, 0);
}


void set_bpx_buttons(HWND dlg)
{
	auto focus = -1, text = 0, box = 0;
	const auto focused_wnd = GetFocus();
	if (focused_wnd == GetDlgItem(dlg, IDE_CBP) || focused_wnd == GetDlgItem(dlg, IDC_CBP))
		focus = 0, text = IDE_CBP, box = IDC_CBP;
	if (focused_wnd == GetDlgItem(dlg, IDE_BPX) || focused_wnd == GetDlgItem(dlg, IDC_BPX))
		focus = 1, text = IDE_BPX, box = IDC_BPX;
	if (focused_wnd == GetDlgItem(dlg, IDE_MEM) || focused_wnd == GetDlgItem(dlg, IDC_MEM) ||
		focused_wnd == GetDlgItem(dlg, IDC_MEM_R) || focused_wnd == GetDlgItem(dlg, IDC_MEM_W))
		focus = 2, text = IDE_MEM, box = IDC_MEM;

	SendDlgItemMessage(dlg, IDE_CBP, EM_SETREADONLY, BOOL(focus != 0), 0);
	SendDlgItemMessage(dlg, IDE_BPX, EM_SETREADONLY, BOOL(focus != 1), 0);
	SendDlgItemMessage(dlg, IDE_MEM, EM_SETREADONLY, BOOL(focus != 2), 0);

	auto del0 = 0, add0 = 0, del1 = 0, add1 = 0, del2 = 0, add2 = 0;
	const unsigned max = SendDlgItemMessage(dlg, box, LB_GETCOUNT, 0, 0);
	unsigned
		cur = SendDlgItemMessage(dlg, box, LB_GETCURSEL, 0, 0),
		len = SendDlgItemMessage(dlg, text, WM_GETTEXTLENGTH, 0, 0);

	if (max && cur >= max) SendDlgItemMessage(dlg, box, LB_SETCURSEL, cur = 0, 0);

	if (focus == 0) { if (len && max < MAX_CBP) add0 = 1; if (cur < max) del0 = 1; }
	if (focus == 1) { if (len) add1 = 1; if (cur < max) del1 = 1; }
	if (focus == 2) {
		if (IsDlgButtonChecked(dlg, IDC_MEM_R) == BST_UNCHECKED && IsDlgButtonChecked(dlg, IDC_MEM_W) == BST_UNCHECKED) len = 0;
		if (len) add2 = 1; if (cur < max) del2 = 1;
	}

	EnableWindow(GetDlgItem(dlg, IDB_CBP_ADD), add0);
	EnableWindow(GetDlgItem(dlg, IDB_CBP_DEL), del0);
	EnableWindow(GetDlgItem(dlg, IDB_BPX_ADD), add1);
	EnableWindow(GetDlgItem(dlg, IDB_BPX_DEL), del1);
	EnableWindow(GetDlgItem(dlg, IDB_MEM_ADD), add2);
	EnableWindow(GetDlgItem(dlg, IDB_MEM_DEL), del2);

	unsigned defid = 0;
	if (add0) defid = IDB_CBP_ADD;
	if (add1) defid = IDB_BPX_ADD;
	if (add2) defid = IDB_MEM_ADD;
	if (defid) SendMessage(dlg, DM_SETDEFID, defid, 0);
}

char move_bpx_from_box_to_edit(HWND dlg, unsigned box, unsigned edit)
{
	const auto hBox = GetDlgItem(dlg, box);
	const unsigned max = SendDlgItemMessage(dlg, box, LB_GETCOUNT, 0, 0);
	const unsigned
		cur = SendDlgItemMessage(dlg, box, LB_GETCURSEL, 0, 0);
	if (cur >= max) return 0;
	char tmp[0x200];
	SendMessage(hBox, LB_GETTEXT, cur, LPARAM(tmp));
	if (box == IDC_MEM && *tmp) {
		auto last = tmp + strlen(tmp);
		unsigned r = BST_UNCHECKED, w = BST_UNCHECKED;
		if (last[-1] == 'W') w = BST_CHECKED, last--;
		if (last[-1] == 'R') r = BST_CHECKED, last--;
		if (last[-1] == ' ') last--;
		*last = 0;
		CheckDlgButton(dlg, IDC_MEM_R, r);
		CheckDlgButton(dlg, IDC_MEM_W, w);
	}
	SetDlgItemText(dlg, edit, tmp);
	return 1;
}

typedef bool(__cdecl *func_t)();

unsigned calcerr;
unsigned calc(const Z80 *cpu, unsigned *script)
{
	unsigned stack[64];
	unsigned *sp = stack - 1, x;
	while (*script) {
		switch (*script++) {
		case 'M':             *sp = cpu->DirectRm(*sp);   break;
		case '!':             *sp = !*sp;     break;
		case '~':             *sp = ~*sp;     break;
		case '+':             *(sp - 1) += *sp; goto arith;
		case '-':             *(sp - 1) -= *sp; goto arith;
		case '*':             *(sp - 1) *= *sp; goto arith;
		case '/':             if (*sp) *(sp - 1) /= *sp; goto arith;
		case '%':             if (*sp) *(sp - 1) %= *sp; goto arith;
		case '&':             *(sp - 1) &= *sp; goto arith;
		case '|':             *(sp - 1) |= *sp; goto arith;
		case '^':             *(sp - 1) ^= *sp; goto arith;
		case WORD2('-', '>'):  *(sp - 1) = cpu->DirectRm(*sp + sp[-1]); goto arith;
		case WORD2('>', '>'):  *(sp - 1) >>= *sp; goto arith;
		case WORD2('<', '<'):  *(sp - 1) <<= *sp; goto arith;
		case WORD2('!', '='):  *(sp - 1) = (sp[-1] != *sp); goto arith;
		case '=':
		case WORD2('=', '='):  *(sp - 1) = (sp[-1] == *sp); goto arith;
		case WORD2('<', '='):  *(sp - 1) = (sp[-1] <= *sp); goto arith;
		case WORD2('>', '='):  *(sp - 1) = (sp[-1] >= *sp); goto arith;
		case WORD2('|', '|'):  *(sp - 1) = (sp[-1] || *sp); goto arith;
		case WORD2('&', '&'):  *(sp - 1) = (sp[-1] && *sp); goto arith;
		case '<':             *(sp - 1) = (sp[-1] < *sp); goto arith;
		case '>':             *(sp - 1) = (sp[-1] > *sp); goto arith;
		arith:                sp--;  break;
		case DB_CHAR:
		case DB_SHORT:        x = *script++; goto push;
		case DB_PCHAR:        x = *reinterpret_cast<u8*>(*script++); goto push;
		case DB_PSHORT:       x = 0xFFFF & *reinterpret_cast<unsigned*>(*script++); goto push;
		case DB_PINT:         x = *reinterpret_cast<unsigned*>(*script++); goto push;
		case DB_PFUNC:        x = reinterpret_cast<func_t>(*script++)(); goto push;
		push:                 *++sp = x; break;
		default:;
		} // switch (*script)
	} // while
	if (sp != stack) calcerr = 1;
	return *sp;
}

u8 toscript(char *script, unsigned *dst)
{
	unsigned *d1 = dst;
	static struct {
		u16 op;
		u8 prior;
	} prio[] = {
	   { '(', 10 },
	   { ')', 0 },
	   { '!', 1 },
	   { '~', 1 },
	   { 'M', 1 },
	   { WORD2('-','>'), 1 },
	   { '*', 2 },
	   { '%', 2 },
	   { '/', 2 },
	   { '+', 3 },
	   { '-', 3 },
	   { WORD2('>','>'), 4 },
	   { WORD2('<','<'), 4 },
	   { '>', 5 },
	   { '<', 5 },
	   { '=', 5 },
	   { WORD2('>','='), 5 },
	   { WORD2('<','='), 5 },
	   { WORD2('=','='), 5 },
	   { WORD2('!','='), 5 },
	   { '&', 6 },
	   { '^', 7 },
	   { '|', 8 },
	   { WORD2('&','&'), 9 },
	   { WORD2('|','|'), 10 }
	};

	const Z80 &cpu = TCpuMgr::get_cpu();

	DECL_REGS(regs, cpu);

	unsigned sp = 0;
	unsigned stack[128];
	for (auto p = script; *p; p++)
		if (p[1] != 0x27)
			*p = toupper(*p);

	while (*script)
	{
		if (*reinterpret_cast<u8*>(script) <= ' ')
		{
			script++;
			continue;
		}

		if (*script == '\'')
		{ // char
			*dst++ = DB_CHAR;
			*dst++ = script[1];
			if (script[2] != '\'') return 0;
			script += 3; continue;
		}

		if (isalnum(*script))
		{
			auto r = UINT_MAX;
			const auto p = *reinterpret_cast<unsigned*>(script);
			unsigned ln = 0;
			for (unsigned i = 0; i < _countof(regs); i++)
			{
				unsigned mask = 0xFF; ln = 1;
				if (regs[i].reg & 0xFF00) mask = 0xFFFF, ln = 2;
				if (regs[i].reg & 0xFF0000) mask = 0xFFFFFF, ln = 3;
				if (regs[i].reg & 0xFF000000) mask = 0xFFFFFFFF, ln = 4;
				if (regs[i].reg == (p & mask)) { r = i; break; }
			}
			if (r != UINT_MAX)
			{
				script += ln;
				switch (regs[r].size)
				{
				case 0: *dst++ = DB_PFUNC; break;
				case 1: *dst++ = DB_PCHAR; break;
				case 2: *dst++ = DB_PSHORT; break;
				case 4: *dst++ = DB_PINT; break;
				default: errexit("BUG01");
				}
				*dst++ = unsigned(regs[r].ptr);

				continue;
			}
			else if (*script != 'M')
			{ // number
				if (*script > 'F') return 0;
				for (r = 0; isalnum(*script) && *script <= 'F'; script++)
					r = r * 0x10 + ((*script >= 'A') ? *script - 'A' + 10 : *script - '0');
				*dst++ = DB_SHORT;
				*dst++ = r;

				continue;
			}
		}
		// find operation
		u8 pr = 0xFF;
		unsigned r = *script++;
		if (strchr("<>=&|-!", char(r)) && strchr("<>=&|", *script))
			r = r + 0x100 * (*script++);
		for (auto& i : prio)
		{
			if (i.op == r)
			{
				pr = i.prior;
				break;
			}
		}
		if (pr == 0xFF)
			return 0;
		if (r != '(')
		{
			while (sp && ((stack[sp] >> 16 <= pr) || (r == ')' && (stack[sp] & 0xFF) != '(')))
			{ // get from stack
				*dst++ = stack[sp--] & 0xFFFF;
			}
		}
		if (r == ')')
			sp--; // del opening bracket
		else
			stack[++sp] = r + 0x10000 * pr; // put to stack
		if (int(sp) < 0)
			return 0; // no opening bracket
	}
	// empty stack
	while (sp)
	{
		if ((stack[sp] & 0xFF) == '(')
			return 0; // no closing bracket
		*dst++ = stack[sp--] & 0xFFFF;
	}
	*dst = DB_STOP;

	calcerr = 0;
	calc(&cpu, d1);
	return (1 - calcerr);
}

struct MemRange { unsigned start, end; };

int get_mem_ramge(char *str, MemRange &range)
{
	while (*str == ' ') str++;
	for (range.start = 0; ishex(*str); str++)
		range.start = range.start * 0x10 + hex(*str);
	if (*str == '-') {
		for (range.end = 0, str++; ishex(*str); str++)
			range.end = range.end * 0x10 + hex(*str);
	}
	else range.end = range.start;
	while (*str == ' ') str++;
	if (*str) return 0;

	if (range.start > 0xFFFF || range.end > 0xFFFF || range.start > range.end) return 0;
	return 1;
}


INT_PTR CALLBACK conddlg(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_INITDIALOG)
	{
		fill_cond_box(dlg, 0);
		fill_bpx_box(dlg, 0);
		fill_mem_box(dlg, 0);
		SetFocus(GetDlgItem(dlg, IDE_CBP));

	set_buttons_and_return:
		set_bpx_buttons(dlg);
		return 1;
	}
	if (msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_CLOSE) EndDialog(dlg, 0);
	if (msg != WM_COMMAND) return 0;

	unsigned id = LOWORD(wp);
	const unsigned code = HIWORD(wp);
	u8 mask;
	if (id == IDCANCEL || id == IDOK) EndDialog(dlg, 0);
	char tmp[0x200];

	if (((id == IDE_BPX || id == IDE_CBP || id == IDE_MEM) && (code == EN_SETFOCUS || code == EN_CHANGE)) ||
		((id == IDC_BPX || id == IDC_CBP || id == IDC_MEM) && code == LBN_SETFOCUS)) goto set_buttons_and_return;

	if (id == IDC_MEM_R || id == IDC_MEM_W) goto set_buttons_and_return;

	if (code == LBN_DBLCLK)
	{
		if (id == IDC_CBP && move_bpx_from_box_to_edit(dlg, IDC_CBP, IDE_CBP)) goto del_cond;
		if (id == IDC_BPX && move_bpx_from_box_to_edit(dlg, IDC_BPX, IDE_BPX)) goto del_bpx;
		if (id == IDC_MEM && move_bpx_from_box_to_edit(dlg, IDC_MEM, IDE_MEM)) goto del_mem;
	}

	if (id == IDB_CBP_ADD)
	{
		SendDlgItemMessage(dlg, IDE_CBP, WM_GETTEXT, sizeof tmp, LPARAM(tmp));
		SetFocus(GetDlgItem(dlg, IDE_CBP));
		auto& cpu = TCpuMgr::get_cpu();
		if (!toscript(tmp, cpu.cbp[cpu.cbpn])) {
			MessageBox(dlg, "Error in expression\nPlease do RTFM", 0, MB_ICONERROR);
			return 1;
		}
		SendDlgItemMessage(dlg, IDE_CBP, WM_SETTEXT, 0, 0);
		fill_cond_box(dlg, cpu.cbpn++);
		cpu.dbgchk = TCpuMgr::isbrk(cpu);
		goto set_buttons_and_return;
	}

	if (id == IDB_BPX_ADD)
	{
		SendDlgItemMessage(dlg, IDE_BPX, WM_GETTEXT, sizeof tmp, LPARAM(tmp));
		SetFocus(GetDlgItem(dlg, IDE_BPX));
		MemRange range{};
		if (!get_mem_ramge(tmp, range))
		{
			MessageBox(dlg, "Invalid breakpoint address / range", nullptr, MB_ICONERROR);
			return 1;
		}

		auto& cpu = TCpuMgr::get_cpu();
		for (auto i = range.start; i <= range.end; i++)
			cpu.membits[i] |= MEMBITS_BPX;
		SendDlgItemMessage(dlg, IDE_BPX, WM_SETTEXT, 0, 0);
		fill_bpx_box(dlg, range.start);
		cpu.dbgchk = TCpuMgr::isbrk(cpu);
		goto set_buttons_and_return;
	}

	if (id == IDB_MEM_ADD)
	{
		SendDlgItemMessage(dlg, IDE_MEM, WM_GETTEXT, sizeof tmp, LPARAM(tmp));
		SetFocus(GetDlgItem(dlg, IDE_MEM));
		MemRange range{};
		if (!get_mem_ramge(tmp, range))
		{
			MessageBox(dlg, "Invalid watch address / range", 0, MB_ICONERROR);
			return 1;
		}

		mask = 0;
		if (IsDlgButtonChecked(dlg, IDC_MEM_R) == BST_CHECKED) mask |= MEMBITS_BPR;
		if (IsDlgButtonChecked(dlg, IDC_MEM_W) == BST_CHECKED) mask |= MEMBITS_BPW;

		auto& cpu = TCpuMgr::get_cpu();
		for (auto i = range.start; i <= range.end; i++)
			cpu.membits[i] |= mask;
		SendDlgItemMessage(dlg, IDE_MEM, WM_SETTEXT, 0, 0);
		CheckDlgButton(dlg, IDC_MEM_R, BST_UNCHECKED);
		CheckDlgButton(dlg, IDC_MEM_W, BST_UNCHECKED);
		fill_mem_box(dlg, range.start);
		cpu.dbgchk = TCpuMgr::isbrk(cpu);
		goto set_buttons_and_return;
	}

	if (id == IDB_CBP_DEL)
	{
	del_cond:
		SetFocus(GetDlgItem(dlg, IDE_CBP));
		unsigned cur = SendDlgItemMessage(dlg, IDC_CBP, LB_GETCURSEL, 0, 0);
		auto& cpu = TCpuMgr::get_cpu();
		if (cur >= cpu.cbpn)
			return 0;
		cpu.cbpn--;
		memcpy(cpu.cbp[cur], cpu.cbp[cur + 1], sizeof(cpu.cbp[0])*(cpu.cbpn - cur));

		if (cur && cur == cpu.cbpn)
			cur--;
		fill_cond_box(dlg, cur);
		cpu.dbgchk = TCpuMgr::isbrk(cpu);
		goto set_buttons_and_return;
	}

	if (id == IDB_BPX_DEL)
	{
	del_bpx:
		SetFocus(GetDlgItem(dlg, IDE_BPX));
		id = IDC_BPX; mask = ~MEMBITS_BPX;
	del_range:
		unsigned cur = SendDlgItemMessage(dlg, id, LB_GETCURSEL, 0, 0);
		const unsigned max = SendDlgItemMessage(dlg, id, LB_GETCOUNT, 0, 0);
		if (cur >= max) return 0;
		SendDlgItemMessage(dlg, id, LB_GETTEXT, cur, LPARAM(tmp));
		unsigned start, end;
		sscanf(tmp, "%X", &start);
		if (tmp[4] == '-') sscanf(tmp + 5, "%X", &end); else end = start;

		auto& cpu = TCpuMgr::get_cpu();
		for (auto i = start; i <= end; i++)
			cpu.membits[i] &= mask;
		if (id == IDC_BPX) fill_bpx_box(dlg, 0); else fill_mem_box(dlg, 0);
		if (cur && cur == max)
			cur--;
		SendDlgItemMessage(dlg, id, LB_SETCURSEL, cur, 0);
		cpu.dbgchk = TCpuMgr::isbrk(cpu);
		goto set_buttons_and_return;
	}

	if (id == IDB_MEM_DEL)
	{
	del_mem:
		SetFocus(GetDlgItem(dlg, IDE_MEM));
		id = IDC_MEM; mask = ~(MEMBITS_BPR | MEMBITS_BPW);
		goto del_range;
	}

	return 0;
}


auto Dialogs::mon_bpdialog() const -> void
{
	view_.show_dialog(MAKEINTRESOURCE(IDD_COND), conddlg);
}

INT_PTR CALLBACK watchdlg(HWND dlg, UINT msg, WPARAM wp, LPARAM lp)
{
	char tmp[0x200]; unsigned i;
	static const int ids1[] = { IDC_W1_ON, IDC_W2_ON, IDC_W3_ON, IDC_W4_ON };
	static const int ids2[] = { IDE_W1, IDE_W2, IDE_W3, IDE_W4 };
	if (msg == WM_INITDIALOG) {
		for (i = 0; i < 4; i++) {
			CheckDlgButton(dlg, ids1[i], watch_enabled[i] ? BST_CHECKED : BST_UNCHECKED);
			script2text(tmp, watch_script[i]); SetWindowText(GetDlgItem(dlg, ids2[i]), tmp);
		}
		CheckDlgButton(dlg, IDC_TR_RAM, trace_ram ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(dlg, IDC_TR_ROM, trace_ram ? BST_CHECKED : BST_UNCHECKED);
	reinit:
		for (i = 0; i < 4; i++)
			EnableWindow(GetDlgItem(dlg, ids2[i]), watch_enabled[i]);
		return 1;
	}
	if (msg == WM_COMMAND && (LOWORD(wp) == ids1[0] || LOWORD(wp) == ids1[1] || LOWORD(wp) == ids1[2] || LOWORD(wp) == ids1[3])) {
		for (i = 0; i < 4; i++)
			watch_enabled[i] = IsDlgButtonChecked(dlg, ids1[i]) == BST_CHECKED;
		goto reinit;
	}
	if ((msg == WM_SYSCOMMAND && (wp & 0xFFF0) == SC_CLOSE) || (msg == WM_COMMAND && LOWORD(wp) == IDCANCEL)) {
		trace_ram = IsDlgButtonChecked(dlg, IDC_TR_RAM) == BST_CHECKED;
		trace_rom = IsDlgButtonChecked(dlg, IDC_TR_ROM) == BST_CHECKED;
		for (i = 0; i < 4; i++)
			if (watch_enabled[i]) {
				SendDlgItemMessage(dlg, ids2[i], WM_GETTEXT, sizeof tmp, LPARAM(tmp));
				if (!toscript(tmp, watch_script[i])) {
					sprintf(tmp, "Watch %d: error in expression\nPlease do RTFM", i + 1);
					MessageBox(dlg, tmp, nullptr, MB_ICONERROR); watch_enabled[i] = 0;
					SetFocus(GetDlgItem(dlg, ids2[i]));
					return 0;
				}
			}
		EndDialog(dlg, 0);
	}
	return 0;
}

auto Dialogs::mon_watchdialog() const -> void
{
	view_.show_dialog(MAKEINTRESOURCE(IDD_OSW), watchdlg);
}

auto Dialogs::subscrible() -> void
{
	actions.mon_gs += [this]() { mon_gsdialog(); };
	actions.mon_osw += [this]() { mon_watchdialog(); };
	actions.mon_settings += []() { mon_setup_dlg(); };
	actions.mon_bp_dialog += [this]() { mon_bpdialog(); };

	actions.dialog_find1 += [this](auto addr) { return find1dlg(addr); };
	actions.dialog_find2 += [this](auto addr) { return find2dlg(addr); };
}

auto Dialogs::mon_setup_dlg() -> void
{
#ifdef MOD_SETTINGS
	setup_dlg();
	temp.rflags = RF_MONITOR;
	set_video();
#endif
}
