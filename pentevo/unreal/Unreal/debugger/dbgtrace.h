#pragma once

extern unsigned asmii;
extern const unsigned cs[3][2];

int disasm_line(unsigned addr, char *line);



void cfindpc();
void cfindtext();
void cfindcode();
void cgoto();
void cbpx();
void center();
void csetpc();
void cup();
void cdown();
void cleft();
void cright();
void cpgup();
void cpgdn();
void pop_pos();
void cjump();
void cdjump();


void cfliplabels();
void c_lbl_import();


void show_trace();
char dispatch_trace();
