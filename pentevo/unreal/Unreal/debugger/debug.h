#pragma once

enum class dbgwnd
{
	no = 0,
	mem,
	trace,
	regs,
	banks
};

enum { ed_mem, ed_phys, ed_log, ed_cmos, ed_nvram, ed_max };

class TCpuMgr final
{
	static const unsigned count;
	static Z80* cpus_[];
	static TZ80State prev_cpus_[];
	static unsigned current_cpu_;
public:
	static Z80 &get_cpu() { return *cpus_[current_cpu_]; }
	static Z80 &get_cpu(const u32 idx) { return *cpus_[idx]; }
	static TZ80State &prev_cpu(const u32 idx) { return prev_cpus_[idx]; }
	static TZ80State &prev_cpu() { return prev_cpus_[current_cpu_]; }
	static void switch_cpu();
	static unsigned get_current_cpu() { return current_cpu_; }
	static void set_current_cpu(const u32 idx) { current_cpu_ = idx; }
	static void copy_to_prev();
	static unsigned get_count() { return count; }
};

// debug breakpoints format descriptor
typedef struct
{
	unsigned reg;       // token string (e.g. 'DOS','OUT')
	const void *ptr;    // pointer to variable, containing queried value
	u8 size;            // size of variable (1, 2, 4) or 0 for address of function: bool func()
} BPXR;

extern u8 debug_gdibuf[];

extern TCpuMgr cpu_mgr;
extern dbgwnd activedbg;
extern unsigned dbg_extport;
extern u8 dgb_extval; // extended memory port like 1FFD or DFFD

extern unsigned mem_sz;
extern unsigned mem_disk;
extern unsigned mem_track;
extern unsigned mem_max;

extern u8 mem_dump;
extern unsigned show_scrshot;
extern u8 editor;
extern u8 mem_ascii;


extern unsigned ripper; // ripper mode (none/read/write)

extern unsigned user_watches[3];
extern unsigned regs_curs;

extern u8 trace_labels;

u8 isbrk(const Z80 &cpu); // is there breakpoints active or any other reason to use debug z80 loop?
void debugscr();
void debug_events(Z80 *cpu);

void init_debug();
