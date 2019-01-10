#pragma once
#include "sysdefs.h"
#include "defs.h"

class TCpuMgr final
{
	static const unsigned count;
	static Z80* cpus_[];
	static TZ80State prev_cpus_[];
	static unsigned current_cpu_;
public:
	static Z80& get_cpu();
	static Z80& get_cpu(const u32 idx);
	static TZ80State& prev_cpu(const u32 idx);
	static TZ80State& prev_cpu();
	static void switch_cpu();
	static unsigned get_current_cpu();
	static void set_current_cpu(const u32 idx);
	static void copy_to_prev();
	static unsigned get_count();
};
