#pragma once

constexpr auto regs_x = 1;
constexpr auto regs_y = 1;

constexpr auto stack_x = 72;
constexpr auto stack_y = 12;
constexpr auto stack_size = 10;

constexpr auto ay_x = 31;
constexpr auto ay_y = 28;

constexpr auto time_x = 1;
constexpr auto time_y = 28;

constexpr auto copy_x = 1;
constexpr auto copy_y = 28;

constexpr auto banks_x = 72;
constexpr auto banks_y = 22;

constexpr auto ports_x = 72;
constexpr auto ports_y = 1;

constexpr auto dos_x = 72;
constexpr auto dos_y = 6;
constexpr auto trace_size = 21;
constexpr auto trace_x = 1;
constexpr auto trace_y = 6;

constexpr auto wat_x = 34;
constexpr auto wat_y = 1;
constexpr auto wat_sz = 13;

constexpr auto mem_size = 12;
constexpr auto mem_x = 34;
constexpr auto mem_y = 15;

constexpr auto w_sel = 0x17;
constexpr auto w_norm = 0x07;
constexpr auto w_curs = 0x30;
constexpr auto backgr = 0x50;
constexpr auto frame_curs = 0x02;
constexpr auto w_title = 0x59;
constexpr auto w_other = 0x40;
constexpr auto w_otheroff = 0x47;
constexpr auto backgr_ch = 0xB1;
constexpr auto w_aynum = 0x4F;
constexpr auto w_ayon = 0x41;
constexpr auto w_ayoff = 0x40;
constexpr auto w_bank = 0x40;
constexpr auto w_bankro = 0x41;
constexpr auto w_dihalt1 = 0x1A;
constexpr auto w_dihalt2 = 0x0A;
constexpr auto w_tracepos = 0x70;
constexpr auto w_inputcur = 0x60;
constexpr auto w_inputbg = 0x40;
constexpr auto w_48_k = 0x20;
constexpr auto w_dos = 0x20;

constexpr auto w_trace_jinfo_curs_fg = 0x0D;
constexpr auto w_trace_jinfo_nocurs_fg = 0x02;
constexpr auto w_trace_jarrow_foregr = 0x0D;

constexpr auto FRAME = 0x01;
constexpr auto fframe_frame = 0x04;

constexpr auto fframe_inside = 0x50;
constexpr auto fframe_error = 0x52;
constexpr auto frm_header = 0xD0;

constexpr auto menu_inside = 0x70;
constexpr auto menu_header = 0xF0;

constexpr auto menu_cursor = 0xE0;
constexpr auto menu_item = menu_inside;
constexpr auto menu_item_dis = 0x7A;

constexpr auto tool_x = 18;
constexpr auto tool_y = 12;

constexpr auto debug_text_width = 150;
constexpr auto debug_text_height = 30;
constexpr auto debug_text_size = (debug_text_width * debug_text_height);
constexpr auto DEBUG_WND_WIDTH = (debug_text_width * 8);
constexpr auto DEBUG_WND_HEIGHT = (debug_text_height * 16);

constexpr auto file_dlg_x = 6;
constexpr auto file_dlg_y = 10;
constexpr auto file_dlg_dx = 25;

constexpr auto dbg_gdibuf_size = (DEBUG_WND_WIDTH*DEBUG_WND_HEIGHT);
