#include "std.h"
#include "cpu_manager.h"
#include "vars.h"

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

void TCpuMgr::switch_cpu()
{
	current_cpu_++;
	current_cpu_ %= count;
}
