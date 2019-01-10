#pragma once

void init_bpx(char* file);
void done_bpx();

unsigned calc(const Z80 *cpu, unsigned *script);
