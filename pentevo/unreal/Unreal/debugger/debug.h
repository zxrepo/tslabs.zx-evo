#pragma once


// debug breakpoints format descriptor
typedef struct
{
	unsigned reg;       // token string (e.g. 'DOS','OUT')
	const void *ptr;    // pointer to variable, containing queried value
	u8 size;            // size of variable (1, 2, 4) or 0 for address of function: bool func()
} BPXR;

extern unsigned mem_disk;
extern unsigned mem_track;
extern unsigned mem_max;


extern unsigned ripper; // ripper mode (none/read/write)


extern u8 trace_labels;

void debugscr();

