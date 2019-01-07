#pragma once

void mon_gsdialog();
void mon_setwatch();
void mon_switchay();

extern unsigned int selbank, showbank;
void __cdecl BankNames(int i, char *name);
int dispatch_banks();
void bup();
void bdown();
void benter();

void show_watch();
void show_stack();
void show_ay();
void show_banks();
void show_ports();
void show_dos();
void show_time();
