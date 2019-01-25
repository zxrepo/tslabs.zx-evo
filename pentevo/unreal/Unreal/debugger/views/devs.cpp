#include "std.h"
#include "devs.h"
#include "emul.h"
#include "util.h"
#include "funcs.h"
#include "debugger/consts.h"
#include "debugger/libs/cpu_manager.h"
#include "core/ui/actions.h"

void __cdecl BankNames(int i, char *name)
{
	unsigned rom_bank = 0;
	unsigned ram_bank = 0;

	const bool is_ram = (RAM_BASE_M <= bankr[i]) && (bankr[i] < page_ram(MAX_RAM_PAGES));
	const bool is_rom = (ROM_BASE_M <= bankr[i]) && (bankr[i] < page_rom(MAX_ROM_PAGES));

	if (is_ram)
		ram_bank = ULONG((bankr[i] - RAM_BASE_M) / PAGE);

	if (is_rom)
		rom_bank = ULONG((bankr[i] - ROM_BASE_M) / PAGE);

	if (is_ram)
		sprintf(name, "RAM%2X", ram_bank);

	if (is_rom)
		sprintf(name, "ROM%2X", rom_bank);

	if (bankr[i] == base_sos_rom)
		strcpy(name, "BASIC");
	if (bankr[i] == base_dos_rom)
		strcpy(name, "TRDOS");
	if (bankr[i] == base_128_rom)
		strcpy(name, "B128K");
	if (bankr[i] == base_sys_rom && (conf.mem_model == MM_PROFSCORP || conf.mem_model == MM_SCORP || conf.mem_model == MM_GMX))
		strcpy(name, "SVM  ");
	if ((conf.mem_model == MM_PROFSCORP || conf.mem_model == MM_SCORP || conf.mem_model == MM_GMX) && is_rom && rom_bank > 3)
		sprintf(name, "ROM%2X", rom_bank);

	if (bankr[i] == CACHE_M)
		strcpy(name, (conf.cache != 32) ? "CACHE" : "CACH0");
	if (bankr[i] == CACHE_M + PAGE)
		strcpy(name, "CACH1");
}

auto WatchView::subsrible() -> void
{
	actions.mon_set_watch += [this]() { mon_setwatch(); };
	actions.mon_screenshot += [this]() { mon_scrshot(); };
}

WatchView::WatchView(DebugView& view): view_(view)
{
	subsrible();
}

auto WatchView::mon_setwatch() -> void
{
	if (show_scrshot_)
		show_scrshot_ = 0;

	for (unsigned i = 0; i < 3; i++) {
		actions.debug_screen();
		const auto addr = view_.input4(wat_x, wat_y + wat_sz - 3 + i, user_watches_[i]);
		if (addr == UINT_MAX) return;
		user_watches_[i] = addr;
	}
}

auto WatchView::mon_scrshot() -> void
{
	show_scrshot_++; 
	if (show_scrshot_ == 3)
		show_scrshot_ = 0;
}

auto WatchView::render() -> void
{
	if (show_scrshot_)
	{
		for (unsigned y = 0; y < wat_sz; y++)
			for (unsigned x = 0; x < 37; x++)
				view_.set_scr(debug_text_width * debug_text_height + (wat_y + y) * debug_text_width + (wat_x + x), 0xff);
	}
	else
	{
		auto& cpu = TCpuMgr::get_cpu();
		view_.wtline("PC", cpu.pc, 0);
		view_.wtline("SP", cpu.sp, 1);
		view_.wtline("BC", cpu.bc, 2);
		view_.wtline("DE", cpu.de, 3);
		view_.wtline("HL", cpu.hl, 4);
		view_.wtline("IX", cpu.ix, 5);
		view_.wtline("IY", cpu.iy, 6);
		view_.wtline("BC'", cpu.alt.bc, 7);
		view_.wtline("DE'", cpu.alt.de, 8);
		view_.wtline("HL'", cpu.alt.hl, 9);
		view_.wtline(nullptr, user_watches_[0], 10);
		view_.wtline(nullptr, user_watches_[1], 11);
		view_.wtline(nullptr, user_watches_[2], 12);
	}
	const char *text = "watches";
	if (show_scrshot_ == 1) text = "screen memory";
	if (show_scrshot_ == 2) text = "ray-painted";
	view_.tprint(wat_x, wat_y - 1, text, w_title);
	if (comp.flags & CF_DOSPORTS)
		view_.tprint(wat_x + 34, wat_y - 1, "DOS", w_dos);
	view_.add_frame(wat_x, wat_y, 37, wat_sz, FRAME);
}

StackView::StackView(DebugView& view) : view_(view)
{
}

auto StackView::render() const -> void
{
	Z80 &cpu = TCpuMgr::get_cpu();
	for (unsigned i = 0; i < stack_size; i++)
	{
		char xx[10]; //-2:1234
					 //SP:1234
					 //+2:
		if (!i) *reinterpret_cast<unsigned*>(xx) = WORD2('-', '2');
		else if (i == 1) *reinterpret_cast<unsigned*>(xx) = WORD2('S', 'P');
		else sprintf(xx, (i > 8) ? "%X" : "+%X", (i - 1) * 2);
		sprintf(xx + 2, ":%02X%02X", cpu.DirectRm(cpu.sp + (i - 1) * 2 + 1), cpu.DirectRm(cpu.sp + (i - 1) * 2));
		view_.tprint(stack_x, stack_y + i, xx, w_other);
	}
	view_.tprint(stack_x, stack_y - 1, "stack", w_title);
	view_.add_frame(stack_x, stack_y, 7, stack_size, FRAME);
}

auto AyView::subscrible() -> void
{
	actions.mon_switch_ay += []() { mon_switchay(); };
}

AyView::AyView(DebugView& view) : view_(view)
{
	subscrible();
}

auto AyView::mon_switchay() -> void
{
	comp.active_ay ^= 1;
}

auto AyView::render() const -> void
{
	if (!conf.sound.ay_scheme) return;
	const char *ayn = comp.active_ay ? "AY1" : "AY0";
	if (conf.sound.ay_scheme < AY_SCHEME_QUADRO) ayn = "AY:", comp.active_ay = 0;
	view_.tprint(ay_x - 3, ay_y, ayn, w_title);
	auto chip = &ay[comp.active_ay];
	char line[32];
	for (auto i = 0; i < 16; i++) {
		line[0] = "0123456789ABCDEF"[i]; line[1] = 0;
		view_.tprint(ay_x + i * 3, ay_y, line, w_aynum);
		sprintf(line, "%02X", chip->get_reg(i));
		view_.tprint(ay_x + i * 3 + 1, ay_y, line, i == (chip->get_activereg()) ? w_ayon : w_ayoff);
	}
	view_.add_frame(ay_x, ay_y, 48, 1, FRAME);
}

auto BanksView::subscrible() -> void
{
	actions.banks_up += [this]()
	{
		selbank--;
		selbank &= 3;
	};

	actions.banks_down += [this]()
	{
		selbank++;
		selbank &= 3;
	};

	actions.banks_edit += [this]() { benter(); };
	actions.mon_set_bank += [this]() { editbank(); };
	actions.show_banks += [this](auto show) { showbank = show; };
	actions.set_banks += [this](auto bank) { selbank = bank; };
}

BanksView::BanksView(DebugView& view) : view_(view)
{
	subscrible();
}

auto BanksView::dispatch() const -> char
{
	if ((conf.mem_model == MM_TSL) && (selbank != UINT_MAX) &&
		((input.lastkey >= '0' && input.lastkey <= '9') || (input.lastkey >= 'A' && input.lastkey <= 'F')))
	{
		benter();
		return 1;
	}
	return 0;
}

auto BanksView::benter() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	actions.debug_screen();
	view_.flip();

	char bankstr[64] = { 0 }; cpu.BankNames(selbank, bankstr);
	unsigned val;
	sscanf(&bankstr[3], "%x", &val);

	if ((input.lastkey >= '0' && input.lastkey <= '9') || (input.lastkey >= 'A' && input.lastkey <= 'F'))
		PostThreadMessage(GetCurrentThreadId(), WM_KEYDOWN, input.lastkey, 1);

	val = view_.input2(banks_x + 5, banks_y + selbank + 1, val);
	if (val != UINT_MAX) {
		// set new bank
		comp.ts.page[selbank] = val;
		set_banks();
	}
}

auto BanksView::editbank() const -> void
{
	const auto x = view_.input2(ports_x + 5, ports_y + 1, comp.p7FFD);
	if (x != UINT_MAX)
	{
		comp.p7FFD = x;
		set_banks();
	}
}

auto BanksView::render() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	for (unsigned i = 0; i < 4; i++)
	{
		char ln[64]; sprintf(ln, "%d:", i);
		char attr = ((selbank == i) && (showbank) ? (w_otheroff & 0xF) | w_curs : w_otheroff | (actions.is_active_dbg(dbgwnd::banks) ? 0x10 : 0));
		view_.tprint(banks_x, banks_y + i + 1, ln, attr);
		strcpy(ln, "?????");
		cpu.BankNames(i, ln);
		attr = ((selbank == i) && (showbank) ? w_curs : ((bankr[i] != bankw[i] ? w_bankro : w_bank) | (actions.is_active_dbg(dbgwnd::banks) ? 0x10 : 0)));
		view_.tprint(banks_x + 2, banks_y + i + 1, ln, attr);
	}
	view_.add_frame(banks_x, banks_y + 1, 7, 4, FRAME);
	view_.tprint(banks_x, banks_y, "pages", w_title);
}

auto PortsView::subscrible() -> void
{
	actions.mon_set_himem += [this]() { editextbank(); };
	actions.mon_exit += []() {};
}

PortsView::PortsView(DebugView& view) : view_(view)
{
}

auto PortsView::editextbank() const -> void
{
	if (dbg_extport == UINT_MAX)
		return;
	const auto x = view_.input2(ports_x + 5, ports_y + 2, dgb_extval);
	if (x != UINT_MAX)
		out(dbg_extport, u8(x));
}

auto PortsView::render() -> void
{
	char ln[64];
	sprintf(ln, "  FE:%02X", comp.pFE);
	view_.tprint(ports_x, ports_y, ln, w_other);
	sprintf(ln, "7FFD:%02X", comp.p7FFD);
	view_.tprint(ports_x, ports_y + 1, ln, (comp.p7FFD & 0x20) &&
		!((conf.mem_model == MM_PENTAGON && conf.ramsize == 1024) ||
		(conf.mem_model == MM_PROFI && (comp.pDFFD & 0x10))) ? w_48_k : w_other);

	switch (conf.mem_model)
	{
	case MM_KAY:
	case MM_SCORP:
	case MM_PROFSCORP:
	case MM_GMX:
	case MM_PLUS3:
	case MM_PHOENIX:
		dbg_extport = 0x1FFD;
		dgb_extval = comp.p1FFD;
		break;
	case MM_PROFI:
		dbg_extport = 0xDFFD;
		dgb_extval = comp.pDFFD;
		break;
	case MM_ATM450:
		dbg_extport = 0xFDFD;
		dgb_extval = comp.pFDFD;
		break;
	case MM_ATM710:
	case MM_ATM3:
		dbg_extport = (comp.aFF77 & 0xFFFF);
		dgb_extval = comp.pFF77;
		break;
	case MM_QUORUM:
		dbg_extport = 0x0000; dgb_extval = comp.p00;
		break;
	default:
		dbg_extport = -1;
	}
	if (dbg_extport != UINT_MAX)
		sprintf(ln, "%04X:%02X", dbg_extport, dgb_extval);
	else
		sprintf(ln, "cmos:%02X", comp.cmos_addr);
	view_.tprint(ports_x, ports_y + 2, ln, w_other);

	sprintf(ln, "EFF7:%02X", comp.pEFF7);
	view_.tprint(ports_x, ports_y + 3, ln, w_other);
	view_.add_frame(ports_x, ports_y, 7, 4, FRAME);
	view_.tprint(ports_x, ports_y - 1, "ports", w_title);
}

DosView::DosView(DebugView& view) : view_(view)
{
}

auto DosView::render() const -> void
{
	//    CD:802E
	//    STAT:24
	//    SECT:00
	//    T:00/01
	//    S:00/00
	//[vv]   if (conf.trdos_present) comp.wd.process();
	char ln[64];
	const u8 atr = conf.trdos_present ? w_other : w_otheroff;
	sprintf(ln, "CD:%02X%02X", comp.wd.cmd, comp.wd.data);
	view_.tprint(dos_x, dos_y, ln, atr);
	sprintf(ln, "STAT:%02X", comp.wd.status);
	view_.tprint(dos_x, dos_y + 1, ln, atr);
	sprintf(ln, "SECT:%02X", comp.wd.sector);
	view_.tprint(dos_x, dos_y + 2, ln, atr);
	sprintf(ln, "T:%02X/%02X", comp.wd.seldrive->track, comp.wd.track);
	view_.tprint(dos_x, dos_y + 3, ln, atr);
	sprintf(ln, "S:%02X/%02X", comp.wd.system, comp.wd.rqs);
	view_.tprint(dos_x, dos_y + 4, ln, atr);
	view_.add_frame(dos_x, dos_y, 7, 5, FRAME);
#if 1
	view_.tprint(dos_x, dos_y - 1, "beta128", w_title);
#else
	sprintf(ln, "%X-%X %d", comp.wd.state, comp.wd.state2, comp.wd.seldrive->track);
	tprint(dos_x, dos_y - 1, ln, atr);
#endif
}

TimeView::TimeView(DebugView& view) : view_(view)
{
}

auto TimeView::render() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	view_.tprint(time_x, time_y, "time delta:", w_otheroff);
	char text[32];
	sprintf(text, "%14I64d", cpu.Delta());
	view_.tprint(time_x + 11, time_y, text, w_other);
	view_.tprint(time_x + 25, time_y, "t", w_otheroff);
	view_.add_frame(time_x, time_y, 26, 1, FRAME);
}
