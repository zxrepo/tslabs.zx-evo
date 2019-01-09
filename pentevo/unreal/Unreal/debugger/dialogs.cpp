#include "std.h"
#include "sysdefs.h"
#include "dialogs.h"
#include "consts.h"
#include "vars.h"
#include "util.h"
#include "resource.h"

char str[80];

Dialogs::Dialogs(DebugView& view, MemView& mem): view_(view), mem_(mem)
{
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
	while (!process_msgs());
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
