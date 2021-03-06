#pragma once
#include "sysdefs.h"

struct TAPEINFO
{
   char desc[280];
   unsigned pos;
   unsigned t_size;
};
extern TAPEINFO *tapeinfo;
extern unsigned tape_infosize;
extern u8 *tape_image;
extern unsigned tape_pulse[];

void start_tape();
void stop_tape();
void closetape();
void reset_tape();

void find_tape_index();
u8 tape_bit(); // used in io.cpp & sound.cpp

void tape_traps();
void fast_tape();

inline void init_tape() { closetape(); }
inline void done_tape() { closetape(); }

int readTAP();
int readTZX();
int readCSW();
