#include "std.h"
#include "sysdefs.h"
#include "dialogs.h"
#include "consts.h"
#include "vars.h"

char str[80];

Dialogs::Dialogs(DebugView *view): view_(view)
{
}

auto Dialogs::find1dlg(unsigned start) -> unsigned
{
	static char ftext[12] = "";
	strcpy(str, ftext);
	view_->filledframe(10, 10, 16, 4);
	view_->tprint(10, 10, "  find string   ", frm_header);
	view_->tprint(11, 12, "text:", fframe_inside);
	if (!view_->inputhex(17, 12, 8, false)) return -1;
	strcpy(ftext, str);
	const auto len = strlen(ftext);
	unsigned i; //Alone Coder 0.36.7
	for (auto ptr = memadr(start + 1); ptr != start; ptr = memadr(ptr + 1)) {
		for (/*unsigned*/ i = 0; i < len; i++)
			if (editrm(memadr(ptr + i)) != ftext[i]) break;
		if (i == len) return ptr;
	}
	view_->tprint(11, 12, "  not found   ", fframe_error);
	view_->flip();
	while (!process_msgs());
	return -1;
}

auto Dialogs::find2dlg(unsigned start) -> unsigned
{
	static unsigned code = 0xF3, mask = 0xFF; char ln[64];
	view_->filledframe(10, 10, 16, 5);
	view_->tprint(10, 10, "   find data    ", frm_header);
	sprintf(ln, "code: %08lX", _byteswap_ulong(code));
	view_->tprint(11, 12, ln, fframe_inside);
	sprintf(ln, "mask: %08lX", _byteswap_ulong(mask));
	view_->tprint(11, 13, ln, fframe_inside);
	sprintf(str, "%08lX", _byteswap_ulong(code));
	if (!view_->inputhex(17, 12, 8, true)) return -1;
	sscanf(str, "%x", &code); code = _byteswap_ulong(code);
	view_->tprint(17, 12, str, fframe_inside);
	sprintf(str, "%08lX", _byteswap_ulong(mask));
	if (!view_->inputhex(17, 13, 8, true)) return -1;
	sscanf(str, "%x", &mask); mask = _byteswap_ulong(mask);
	unsigned i; //Alone Coder 0.36.7
	for (auto ptr = memadr(start + 1); ptr != start; ptr = memadr(ptr + 1)) {
		const auto cd = reinterpret_cast<u8*>(&code);
		const auto ms = reinterpret_cast<u8*>(&mask);
		for (/*unsigned*/ i = 0; i < 4; i++)
			if ((editrm(memadr(ptr + i)) & ms[i]) != (cd[i] & ms[i])) break;
		if (i == 4) return ptr;
	}
	view_->tprint(11, 12, "  not found   ", fframe_error);
	view_->tprint(11, 13, "              ", fframe_error);
	view_->flip();
	while (!process_msgs());
	return -1;
}
