#include "std.h"
#include "trace.h"
#include "debugger/cpu_manager.h"
#include "debugger/consts.h"
#include "vars.h"
#include "util.h"

char str[80];

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

auto TraceView::cfindpc() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	cpu.trace_top = cpu.trace_curs = cpu.pc;
}

auto TraceView::cfindtext() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	const auto oldmode = editor;
	editor = ed_mem;
	const auto rs = find1dlg(cpu.trace_curs);
	editor = oldmode;
	if (rs != UINT_MAX)
		cpu.trace_top = cpu.trace_curs = rs;
}

auto TraceView::cfindcode() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	const auto oldmode = editor;
	editor = ed_mem;
	const auto rs = find2dlg(cpu.trace_curs);
	editor = oldmode;
	if (rs != UINT_MAX)
		cpu.trace_top = cpu.trace_curs = rs;
}

auto TraceView::cgoto() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	const auto v = input4(trace_x, trace_y, cpu.trace_top);
	if (v != UINT_MAX)
		cpu.trace_top = cpu.trace_curs = v;
}

auto TraceView::cbpx() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	cpu.membits[cpu.trace_curs] ^= MEMBITS_BPX;
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
		if (!inputhex(trace_x + cs[cpu.trace_mode][0], trace_y + trcurs_y + asmii, cs[cpu.trace_mode][1], cpu.trace_mode < 2))
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
				cdown();
				break;
			}
		}
	}
}

auto TraceView::csetpc() const -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	cpu.pc = cpu.trace_curs;
}

auto TraceView::cup() -> void
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
}

auto TraceView::cdown() const -> void
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
}

auto TraceView::cleft() const -> void
{
	TCpuMgr::get_cpu().trace_mode--;
}

auto TraceView::cright() const -> void
{
	TCpuMgr::get_cpu().trace_mode++;
}

auto TraceView::cpgup() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	unsigned curs = 0;
	for (auto i = 0; i < trace_size; i++)
		if (cpu.trace_curs == cpu.trpc[i]) curs = i;
	for (auto i = 0; i < trace_size; i++)
		cpu.trace_top = cpu_up(cpu.trace_top);

	show_trace();
	cpu.trace_curs = cpu.trpc[curs];
}

auto TraceView::cpgdn() -> void
{
	auto& cpu = TCpuMgr::get_cpu();
	unsigned curs = 0;
	for (unsigned i = 0; i < trace_size; i++)
		if (cpu.trace_curs == cpu.trpc[i]) curs = i;
	cpu.trace_top = cpu.trpc[trace_size];
	show_trace();
	cpu.trace_curs = cpu.trpc[curs];
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
	char *ptr = 0;
	for (auto p = asmpc; *p; p++)
		if (ishex(p[0]) & ishex(p[1]) & ishex(p[2]) & ishex(p[3])) ptr = p;
	if (!ptr) return;
	unsigned addr; sscanf(ptr, "%04X", &addr);
	auto& cpu = TCpuMgr::get_cpu();
	cpu.mem_curs = addr;
	activedbg = dbgwnd::mem; 
	editor = ed_mem;
}

auto TraceView::csave1() -> void
{
	csave(0);
}

auto TraceView::csave2() -> void
{
	csave(1);
}

auto TraceView::csave3() -> void
{
	csave(2);
}

auto TraceView::csave4() -> void
{
	csave(3);
}

auto TraceView::csave5() -> void
{
	csave(4);
}

auto TraceView::csave6() -> void
{
	csave(5);
}

auto TraceView::csave7() -> void
{
	csave(6);
}

auto TraceView::csave8() -> void
{
	csave(7);
}

auto TraceView::crest1() -> void
{
	crest(0);
}

auto TraceView::crest2() -> void
{
	crest(1);
}

auto TraceView::crest3() -> void
{
	crest(2);
}

auto TraceView::crest4() -> void
{
	crest(3);
}

auto TraceView::crest5() -> void
{
	crest(4);
}

auto TraceView::crest6() -> void
{
	crest(5);
}

auto TraceView::crest7() -> void
{
	crest(6);
}

auto TraceView::crest8() -> void
{
	crest(7);
}
