#include "std.h"
#include "emul.h"
#include "vars.h"
#include "debug.h"
#include "dbgtrace.h"
#include "dbglabls.h"
#include "dbgcmd.h"
#include "memory.h"
#include "z80asm.h"
#include "op_system.h"
#include "util.h"
#include "draw.h"
#include "emulkeys.h"
#include "consts.h"

extern VCTR vid;

int disasm_line(unsigned addr, char *line)
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

unsigned tracewndflags()
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

unsigned trcurs_y;
unsigned asmii;
char asmpc[64], dumppc[12];
const unsigned cs[3][2] = { {0,4}, {5,10}, {16,16} };

void show_trace()
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
	const u8 atr0 = (activedbg == dbgwnd::trace) ? w_sel : w_norm;
	unsigned ii; //Alone Coder 0.36.7
	for (/*unsigned*/ ii = 0; ii < trace_size; ii++)
	{
		pc &= 0xFFFF; cpu.trpc[ii] = pc;
		const auto len = disasm_line(pc, line);
		auto ptr = line + strlen(line);
		while (ptr < line + 32) *ptr++ = ' '; line[32] = 0;

		u8 atr = (pc == cpu.pc) ? w_tracepos : atr0;
		if (cpu.membits[pc] & MEMBITS_BPX) atr = (atr&~7) | 2;
		tprint(trace_x, trace_y + ii, line, atr);

		if (pc == cpu.trace_curs)
		{
			asmii = ii;
			if (activedbg == dbgwnd::trace)
				for (unsigned q = 0; q < cs[cpu.trace_mode][1]; q++)
					txtscr[debug_text_width * debug_text_height + (trace_y + ii) * debug_text_width + trace_x + cs[cpu.trace_mode][0] + q] = w_curs;
		}

		if (cpu.pc_trflags & TWF_BRANCH)
		{
			if (pc == cpu.pc)
			{
				const auto addr = cpu.pc_trflags & 0xFFFF;
				unsigned arr = (addr <= cpu.pc) ? 0x18 : 0x19; // up/down arrow
				const u8 color = (pc == cpu.trace_curs && activedbg == dbgwnd::trace && cpu.trace_mode == 2) ? w_trace_jinfo_curs_fg : w_trace_jinfo_nocurs_fg;
				if (cpu.pc_trflags & TWF_BRADDR) sprintf(line, "%04X%c", addr, arr), tprint_fg(trace_x + 32 - 5, trace_y + ii, line, color);
				else tprint_fg(trace_x + 32 - 1, trace_y + ii, reinterpret_cast<char*>(&arr), color);
			}

			if (pc == (cpu.pc_trflags & 0xFFFF))
			{
				unsigned arr = 0x11; // left arrow
				tprint_fg(trace_x + 32 - 1, trace_y + ii, reinterpret_cast<char*>(&arr), w_trace_jarrow_foregr);
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
	tprint(trace_x, trace_y - 1, cpu_num, w_title);

	char lbr[5];
	_snprintf(lbr, sizeof(lbr), "%04hX", cpu.last_branch);
	tprint(trace_x + 8, trace_y - 1, lbr, w_title);
	frame(trace_x, trace_y, 32, trace_size, FRAME);
}

void c_lbl_import()
{
	mon_labels.import_menu();
}

/* ------------------------------------------------------------- */


void push_pos()
{
	auto& cpu = TCpuMgr::get_cpu();
	memmove(&stack_pos[1], &stack_pos[0], sizeof stack_pos - sizeof *stack_pos);
	memmove(&stack_cur[1], &stack_cur[0], sizeof stack_cur - sizeof *stack_cur);
	stack_pos[0] = cpu.trace_top; stack_cur[0] = cpu.trace_curs;
}

unsigned cpu_up(unsigned ip)
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


char dispatch_trace()
{
	if (input.lastkey >= 'A' && input.lastkey < 'Z')
	{
		center();
		return 1;
	}
	return 0;
}

void cfliplabels()
{
	trace_labels = !trace_labels; show_trace();
}

