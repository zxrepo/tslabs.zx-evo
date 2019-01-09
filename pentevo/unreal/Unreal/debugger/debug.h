#pragma once


// debug breakpoints format descriptor
typedef struct
{
	unsigned reg;       // token string (e.g. 'DOS','OUT')
	const void *ptr;    // pointer to variable, containing queried value
	u8 size;            // size of variable (1, 2, 4) or 0 for address of function: bool func()
} BPXR;

extern unsigned dbg_extport;
extern u8 dgb_extval; // extended memory port like 1FFD or DFFD

extern unsigned mem_disk;
extern unsigned mem_track;
extern unsigned mem_max;

extern unsigned show_scrshot;


extern unsigned ripper; // ripper mode (none/read/write)

extern unsigned user_watches[3];

extern u8 trace_labels;

u8 isbrk(const Z80 &cpu); // is there breakpoints active or any other reason to use debug z80 loop?
void debugscr();

