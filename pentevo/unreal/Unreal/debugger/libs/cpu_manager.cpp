#include "std.h"
#include "cpu_manager.h"
#include "vars.h"
#include "gsz80.h"

Z80 *TCpuMgr::cpus_[] =
{
  &cpu,
#ifdef MOD_GSZ80
  &gscpu
#endif
};

const unsigned TCpuMgr::count = _countof(cpus_);
TZ80State TCpuMgr::prev_cpus_[TCpuMgr::count];
unsigned TCpuMgr::current_cpu_ = 0;


Z80& TCpuMgr::get_cpu()
{
	return *cpus_[current_cpu_];
}

Z80& TCpuMgr::get_cpu(const u32 idx)
{
	return *cpus_[idx];
}

TZ80State& TCpuMgr::prev_cpu(const u32 idx)
{
	return prev_cpus_[idx];
}

TZ80State& TCpuMgr::prev_cpu()
{
	return prev_cpus_[current_cpu_];
}

unsigned TCpuMgr::get_current_cpu()
{
	return current_cpu_;
}

void TCpuMgr::set_current_cpu(const u32 idx)
{
	current_cpu_ = idx;
}

void TCpuMgr::copy_to_prev()
{
	for (unsigned i = 0; i < count; i++)
		prev_cpus_[i] = TZ80State(*cpus_[i]);
}

unsigned TCpuMgr::get_count()
{
	return count;
}

auto TCpuMgr::isbrk(const Z80& cpu) -> u8
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

void TCpuMgr::switch_cpu()
{
	current_cpu_++;
	current_cpu_ %= count;
}
