#include "std.h"
#include "trace.h"
#include "vars.h"
#include "util.h"
#include "z80asm.h"
#include "debugger/consts.h"
#include "debugger/libs/cpu_manager.h"
#include "debugger/libs/dbglabls.h"
#include "memory.h"
#include "core/ui/actions.h"

static char str[80];

auto TraceView::csave(unsigned n) -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	save_pos[n] = cpu.trace_top;
	save_cur[n] = cpu.trace_curs;
}

auto TraceView::crest(unsigned n) -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	if (save_pos[n] == UINT_MAX)
		return;
	push_pos();
	cpu.trace_top = save_pos[n];
	cpu.trace_curs = save_cur[n];
}

auto TraceView::push_pos() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	memmove(&stack_pos[1], &stack_pos[0], sizeof stack_pos - sizeof *stack_pos);
	memmove(&stack_cur[1], &stack_cur[0], sizeof stack_cur - sizeof *stack_cur);
	stack_pos[0] = cpu.trace_top; stack_cur[0] = cpu.trace_curs;
}

auto TraceView::cpu_up(unsigned ip) const -> unsigned
{
	auto& cpu = TCpuMgr::get_cpu();
	u8 buf1[0x10];
	const auto p1 = (ip > sizeof buf1) ? ip - sizeof buf1 : 0;
	for (unsigned i = 0; i < sizeof buf1; i++) buf1[i] = cpu.DirectRm(p1 + i);
	u8 *dispos = buf1, *prev;
	do {
		prev = dispos;
		dispos = disasm(dispos, 0, 0);
	} while (unsigned(dispos - buf1 + p1) < ip);
	return prev - buf1 + p1;
}

auto TraceView::tracewndflags() const -> unsigned
{
	auto& cpu = TCpuMgr::get_cpu();
	auto readptr = cpu.pc, base = cpu.hl;
	u8 opcode; u8 ed = 0;
	for (;;)
	{
		opcode = cpu.DirectRm(readptr++);
		if (opcode == 0xDD)
			base = cpu.ix;
		else if (opcode == 0xFD)
			base = cpu.iy;
		else if (opcode == 0xED)
			ed = 1;
		else
			break;
	}

	unsigned fl = 0;
	if (opcode == 0x76) // halt
	{
		return TWF_BLKCMD;
	}

	if (ed)
	{
		if ((opcode & 0xF4) == 0xB0) // ldir/lddr | cpir/cpdr | inir/indr | otir/otdr
			return TWF_BLKCMD;

		if ((opcode & 0xC7) != 0x45)
			return 0; // reti/retn

	ret:
		return (cpu.DirectRm(cpu.sp) | (cpu.DirectRm(cpu.sp + 1) << 8U)) | TWF_BRANCH | TWF_BRADDR;
	}

	if (opcode == 0xC9) // ret
		goto ret;
	if (opcode == 0xC3) // jp
	{
	jp: return (cpu.DirectRm(readptr) | (cpu.DirectRm(readptr + 1) << 8U)) | TWF_BRANCH | fl;
	}
	if (opcode == 0xCD) // call
	{
		fl = TWF_CALLCMD;
		goto jp;
	}

	static const u8 flags[] = { ZF,CF,PV,SF };

	if ((opcode & 0xC1) == 0xC0)
	{
		const auto flag = flags[(opcode >> 4) & 3];
		u8 res = cpu.f & flag;
		if (!(opcode & 0x08))
			res ^= flag;
		if (!res)
			return 0;
		if ((opcode & 0xC7) == 0xC0) // ret cc
			goto ret;
		if ((opcode & 0xC7) == 0xC4) // call cc
		{
			fl = TWF_CALLCMD;
			goto jp;
		}
		if ((opcode & 0xC7) == 0xC2) // jp cc
		{
			fl = TWF_LOOPCMD;
			goto jp;
		}
	}

	if (opcode == 0xE9)
		return base | TWF_BRANCH | TWF_BRADDR; // jp (hl/ix/iy)

	if ((opcode & 0xC7) == 0xC7)
		return (opcode & 0x38) | TWF_CALLCMD | TWF_BRANCH; // rst #xx

	if ((opcode & 0xC7) == 0x00)
	{
		if (!opcode || opcode == 0x08)
			return 0;
		const int offs = static_cast<char>(cpu.DirectRm(readptr++));
		const unsigned addr = (offs + readptr) | TWF_BRANCH;
		if (opcode == 0x18)
			return addr; // jr
		if (opcode == 0x10)
			return (cpu.b == 1) ? 0 : addr | TWF_LOOPCMD; // djnz

		const auto flag = flags[(opcode >> 4) & 1]; // jr cc
		u8 res = cpu.f & flag;
		if (!(opcode & 0x08))
			res ^= flag;
		return res ? addr | TWF_LOOPCMD : 0;
	}
	return 0;
}

auto TraceView::subscrible() -> void
{
	actions.trace_find_pc.subscrible([]()
	{
		auto& cpu = TCpuMgr::get_cpu();
		cpu.trace_top = cpu.trace_curs = cpu.pc;
	});

	actions.trace_here += []()
	{
		auto& cpu = TCpuMgr::get_cpu();
		cpu.dbgbreak = 0;
		dbgbreak = 0;
		cpu.dbgchk = 1;
		cpu.dbg_stophere = cpu.trace_curs;
	};

	actions.trace_find_text += [this]() { cfindtext(); };
	actions.trace_find_code += [this]() { cfindcode(); };

	actions.trace_goto += [this]()
	{
		auto& cpu = TCpuMgr::get_cpu();
		const auto v = view_.input4(trace_x, trace_y, cpu.trace_top);
		if (v != UINT_MAX)
			cpu.trace_top = cpu.trace_curs = v;
	};

	actions.trace_bpx += []()
	{
		auto& cpu = TCpuMgr::get_cpu();
		cpu.membits[cpu.trace_curs] ^= MEMBITS_BPX;
	};

	actions.trace_asm += [this]() { center(); };

	actions.trace_set_pc += []()
	{
		auto& cpu = TCpuMgr::get_cpu();
		cpu.pc = cpu.trace_curs;
	};

	actions.trace_up += [this]()
	{
		auto& cpu = TCpuMgr::get_cpu();
		if (cpu.trace_curs > cpu.trace_top)
		{
			for (unsigned i = 1; i < trace_size; i++)
				if (cpu.trpc[i] == cpu.trace_curs)
					cpu.trace_curs = cpu.trpc[i - 1];
		}
		else
			cpu.trace_top = cpu.trace_curs = cpu_up(cpu.trace_curs);
	};

	actions.trace_down += []()
	{
		auto& cpu = TCpuMgr::get_cpu();
		for (unsigned i = 0; i < trace_size; i++)
			if (cpu.trpc[i] == cpu.trace_curs)
			{
				cpu.trace_curs = cpu.trpc[i + 1];
				if (i + 1 == trace_size)
					cpu.trace_top = cpu.trpc[1];
				break;
			}
	};

	actions.trace_left += []()
	{
		TCpuMgr::get_cpu().trace_mode--;
	};

	actions.trace_right += []()
	{
		TCpuMgr::get_cpu().trace_mode++;
	};

	actions.trace_pg_down += [this]()
	{
		auto& cpu = TCpuMgr::get_cpu();
		unsigned curs = 0;
		for (unsigned i = 0; i < trace_size; i++)
			if (cpu.trace_curs == cpu.trpc[i]) curs = i;
		cpu.trace_top = cpu.trpc[trace_size];
		show_trace();
		cpu.trace_curs = cpu.trpc[curs];
	};

	actions.trace_pg_up += [this]()
	{
		auto& cpu = TCpuMgr::get_cpu();
		unsigned curs = 0;
		for (auto i = 0; i < trace_size; i++)
			if (cpu.trace_curs == cpu.trpc[i]) curs = i;
		for (auto i = 0; i < trace_size; i++)
			cpu.trace_top = cpu_up(cpu.trace_top);

		show_trace();
		cpu.trace_curs = cpu.trpc[curs];
	};

	actions.trace_save1 += [this]() { csave(0); };
	actions.trace_save2 += [this]() { csave(1); };
	actions.trace_save3 += [this]() { csave(2); };
	actions.trace_save4 += [this]() { csave(3); };
	actions.trace_save5 += [this]() { csave(4); };
	actions.trace_save6 += [this]() { csave(5); };
	actions.trace_save7 += [this]() { csave(6); };
	actions.trace_save8 += [this]() { csave(7); };

	actions.trace_restore1 += [this]() { crest(0); };
	actions.trace_restore2 += [this]() { crest(1); };
	actions.trace_restore3 += [this]() { crest(2); };
	actions.trace_restore4 += [this]() { crest(3); };
	actions.trace_restore5 += [this]() { crest(4); };
	actions.trace_restore6 += [this]() { crest(5); };
	actions.trace_restore7 += [this]() { crest(6); };
	actions.trace_restore8 += [this]() { crest(7); };

	actions.trace_back += [this]() { pop_pos(); };
	actions.trace_context += [this]() { cjump(); };
	actions.trace_data_jump += [this]() { cdjump(); };

	actions.trace_labels += [this]()
	{
		trace_labels = !trace_labels;
		show_trace();
	};

	actions.trace_import_labels += []()
	{
		mon_labels.import_menu();
	};
}

auto TraceView::disasm_line(unsigned addr, char* line) const -> int
{
	auto& cpu = TCpuMgr::get_cpu();
	u8 dbuf[16 + 129/*Alone Code 0.36.7*/];
	int i; //Alone Coder 0.36.7
	for (/*int*/ i = 0; i < 16; i++) dbuf[i] = cpu.DirectRm(addr + i);
	sprintf(line, "%04X ", addr);
	auto ptr = 5;
	const int len = disasm(dbuf, addr, trace_labels) - dbuf;
	//8000 ..DDCB0106 rr (ix+1)
	if (trace_labels)
	{
		const auto lbl = mon_labels.find(am_r(addr));
		//      if (lbl) for (int k = 0; k < 10 && lbl[k]; line[ptr++] = lbl[k++]); //Alone Coder
		if (lbl) for (int k = 0; (k < 10) && lbl[k]; )line[ptr++] = lbl[k++]; //Alone Coder
	}
	else
	{
		int len1 = len;
		if (len > 4) len1 = 4, *reinterpret_cast<short*>(line + ptr) = WORD2('.', '.'), ptr += 2;
		for (i = len - len1; i < len; i++)
			sprintf(line + ptr, "%02X", dbuf[i]), ptr += 2;
	}

	while (ptr < 16) line[ptr++] = ' ';
	strcpy(line + ptr, asmbuf);
	return len;
}

TraceView::TraceView(DebugCore& core, DebugView& view, MemView& mem): core_(core), view_(view), mem_(mem)
{
}

auto TraceView::cfindtext() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	const auto oldmode = mem_.editor;
	mem_.editor = ed_mem;
	const auto rs = actions.dialog_find1(cpu.trace_curs);
	mem_.editor = oldmode;
	if (rs != UINT_MAX)
		cpu.trace_top = cpu.trace_curs = rs;
}

auto TraceView::cfindcode() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	const auto oldmode = mem_.editor;
	mem_.editor = ed_mem;
	const auto rs = actions.dialog_find2(cpu.trace_curs);
	mem_.editor = oldmode;
	if (rs != UINT_MAX)
		cpu.trace_top = cpu.trace_curs = rs;
}

auto TraceView::center() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	if (!cpu.trace_mode)
		sprintf(str, "%04X", cpu.trace_curs);
	else if (cpu.trace_mode == 1)
		strcpy(str, dumppc);
	else
		strcpy(str, asmpc);

	if (input.lastkey != VK_RETURN)
	{
		*str = 0;
		PostThreadMessage(GetCurrentThreadId(), WM_KEYDOWN, input.lastkey, 1);
	}

	for (;;)
	{
		if (!view_.inputhex(trace_x + cs[cpu.trace_mode][0], trace_y + trcurs_y + asmii, cs[cpu.trace_mode][1], cpu.trace_mode < 2))
			break;
		if (!cpu.trace_mode)
		{
			push_pos();
			sscanf(str, "%X", &cpu.trace_top);
			cpu.trace_curs = cpu.trace_top;
			for (unsigned i = 0; i < asmii; i++)
				cpu.trace_top = cpu_up(cpu.trace_top);
			break;
		}
		else if (cpu.trace_mode == 1)
		{
			char *p; //Alone Coder 0.36.7
			for (/*char * */p = str + strlen(str) - 1; p >= str && *p == ' '; *p-- = 0) {}
			u8 dump[8]; unsigned i;
			for (p = str, i = 0; ishex(*p) && ishex(p[1]); p += 2)
				dump[i++] = hex(p);
			if (*p) continue;
			for (unsigned j = 0; j < i; j++)
				cpu.DirectWm(cpu.trace_curs + j, dump[j]);
			break;
		}
		else
		{
			const unsigned sz = assemble_cmd(reinterpret_cast<u8*>(str), cpu.trace_curs);
			if (sz)
			{
				for (unsigned i = 0; i < sz; i++)
					cpu.DirectWm(cpu.trace_curs + i, asmresult[i]);
				show_trace();
				actions.trace_down();
				break;
			}
		}
	}
}

auto TraceView::pop_pos() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	if (stack_pos[0] == UINT_MAX)
		return;

	cpu.trace_curs = stack_cur[0];
	cpu.trace_top = stack_pos[0];
	memcpy(&stack_pos[0], &stack_pos[1], sizeof stack_pos - sizeof *stack_pos);
	memcpy(&stack_cur[0], &stack_cur[1], sizeof stack_cur - sizeof *stack_cur);
	stack_pos[(sizeof stack_pos / sizeof *stack_pos) - 1] = -1;
}

auto TraceView::cjump() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	char *ptr = nullptr;

	for (auto p = asmpc; *p; p++)
		if (ishex(p[0]) & ishex(p[1]) & ishex(p[2]) & ishex(p[3])) ptr = p;

	if (!ptr) return;
	push_pos();
	unsigned addr;
	sscanf(ptr, "%04X", &addr);
	cpu.trace_curs = cpu.trace_top = addr;
}

auto TraceView::cdjump() -> void
{
	char *ptr = nullptr;
	for (auto p = asmpc; *p; p++)
		if (ishex(p[0]) & ishex(p[1]) & ishex(p[2]) & ishex(p[3])) ptr = p;
	if (!ptr) return;
	unsigned addr; sscanf(ptr, "%04X", &addr);
	auto& cpu = TCpuMgr::get_cpu();
	cpu.mem_curs = addr;
	view_.activedbg = dbgwnd::mem;
	mem_.editor = ed_mem;
}

auto TraceView::show_trace() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	//   char line[40]; //Alone Coder 0.36.7
	char line[16 + 129]; //Alone Coder 0.36.7

	cpu.trace_curs &= 0xFFFF;
	cpu.trace_top &= 0xFFFF;
	cpu.pc &= 0xFFFF;
	cpu.trace_mode = (cpu.trace_mode + 3) % 3;

	cpu.pc_trflags = tracewndflags();
	cpu.nextpc = (cpu.pc + disasm_line(cpu.pc, line)) & 0xFFFF;
	auto pc = cpu.trace_top;
	asmii = -1;
	const u8 atr0 = (view_.activedbg == dbgwnd::trace) ? w_sel : w_norm;
	unsigned ii; //Alone Coder 0.36.7
	for (/*unsigned*/ ii = 0; ii < trace_size; ii++)
	{
		pc &= 0xFFFF; cpu.trpc[ii] = pc;
		const auto len = disasm_line(pc, line);
		auto ptr = line + strlen(line);
		while (ptr < line + 32) *ptr++ = ' '; line[32] = 0;

		u8 atr = (pc == cpu.pc) ? w_tracepos : atr0;
		if (cpu.membits[pc] & MEMBITS_BPX) atr = (atr&~7) | 2;
		view_.tprint(trace_x, trace_y + ii, line, atr);

		if (pc == cpu.trace_curs)
		{
			asmii = ii;
			if (view_.activedbg == dbgwnd::trace)
				for (unsigned q = 0; q < cs[cpu.trace_mode][1]; q++)
					view_.set_scr(debug_text_width * debug_text_height + (trace_y + ii) * debug_text_width + trace_x + cs[cpu.trace_mode][0] + q, w_curs);
		}

		if (cpu.pc_trflags & TWF_BRANCH)
		{
			if (pc == cpu.pc)
			{
				const auto addr = cpu.pc_trflags & 0xFFFF;
				unsigned arr = (addr <= cpu.pc) ? 0x18 : 0x19; // up/down arrow
				const u8 color = (pc == cpu.trace_curs && view_.activedbg == dbgwnd::trace && cpu.trace_mode == 2) ? w_trace_jinfo_curs_fg : w_trace_jinfo_nocurs_fg;
				if (cpu.pc_trflags & TWF_BRADDR)
					sprintf(line, "%04X%c", addr, arr), view_.tprint_fg(trace_x + 32 - 5, trace_y + ii, line, color);
				else view_.tprint_fg(trace_x + 32 - 1, trace_y + ii, reinterpret_cast<char*>(&arr), color);
			}

			if (pc == (cpu.pc_trflags & 0xFFFF))
			{
				unsigned arr = 0x11; // left arrow
				view_.tprint_fg(trace_x + 32 - 1, trace_y + ii, reinterpret_cast<char*>(&arr), w_trace_jarrow_foregr);
			}
		}

		pc += len;
	}
	cpu.trpc[ii] = pc;

	u8 dbuf[16];
	int i; //Alone Coder
	for (/*int*/ i = 0; i < 16; i++) dbuf[i] = cpu.DirectRm(cpu.trace_curs + i);
	const int len = disasm(dbuf, cpu.trace_curs, 0) - dbuf; strcpy(asmpc, asmbuf);
	for (/*int*/ i = 0; i < len && i < 5; i++)
		sprintf(dumppc + i * 2, "%02X", cpu.DirectRm(cpu.trace_curs + i));

	char cpu_num[10];
	_snprintf(cpu_num, sizeof(cpu_num), "Z80(%d)", TCpuMgr::get_current_cpu());
	view_.tprint(trace_x, trace_y - 1, cpu_num, w_title);

	char lbr[5];
	_snprintf(lbr, sizeof(lbr), "%04hX", cpu.last_branch);
	view_.tprint(trace_x + 8, trace_y - 1, lbr, w_title);
	view_.add_frame(trace_x, trace_y, 32, trace_size, FRAME);
}

auto TraceView::dispatch_trace() -> char
{
	if (input.lastkey >= 'A' && input.lastkey < 'Z')
	{
		center();
		return 1;
	}
	return 0;
}

auto csave(unsigned n) -> void
{
}
