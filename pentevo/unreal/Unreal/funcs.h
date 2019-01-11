void __declspec(noreturn) exit();

void qsave(char*);
void qload(char*);
void savesnap(int);


void spectrum_frame();

void tape_traps();
void fast_tape();
void reset_tape();
u8 tape_bit();

void out(unsigned, u8);
u8 in(unsigned port);
void ts_ext_port_wr(u8, u8);
void set_banks();

void apply_gs();
void setup_dlg();
void savesnddialog();

void init_gs_frame();
void flush_gs_frame();
void reset_gs();
void reset_gs_sound();

void restart_sound();

void reset(ROM_MODE mode);

int loadsnap(char *filename);
u8 what_is(char *filename);

u8 getcheck(unsigned ID);
