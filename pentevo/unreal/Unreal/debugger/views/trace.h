#pragma once
#include "debugger/views/mem.h"

#define TWF_BRANCH  0x010000
#define TWF_BRADDR  0x020000
#define TWF_LOOPCMD 0x040000
#define TWF_CALLCMD 0x080000
#define TWF_BLKCMD  0x100000

class DebugCore;

class TraceView final
{
	IDebugView& view_;
	MemView& mem_;

	unsigned save_pos[8] = { UINT_MAX };
	unsigned save_cur[8] = { UINT_MAX };
	unsigned stack_pos[32] = { UINT_MAX }, stack_cur[32] = { UINT_MAX };
	unsigned trcurs_y{};
	char asmpc[64]{}, dumppc[12]{};

	auto csave(unsigned n) -> void;
	auto crest(unsigned n) -> void;
	auto push_pos() -> void;
	auto cpu_up(unsigned ip) const -> unsigned;
	auto tracewndflags() const -> unsigned;

	auto subscrible() -> void;
public:
	const unsigned cs[3][2] = { {0,4}, {5,10}, {16,16} };

	u8 trace_labels{};
	unsigned asmii{};

	TraceView(MemView& mem);

	auto disasm_line(unsigned addr, char *line) const -> int;

	auto cfindtext() const -> void;
	auto cfindcode() const -> void;
	auto center() -> void;
	auto pop_pos() -> void;
	auto cjump() -> void;
	auto cdjump() -> void;

	auto show_trace() -> void;
	auto dispatch_trace() -> char;
};
