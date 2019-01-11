#pragma once
#include "debugger/core.h"
#include "debugger/libs/view.h"
#include "debugger/views/mem.h"

#define TWF_BRANCH  0x010000
#define TWF_BRADDR  0x020000
#define TWF_LOOPCMD 0x040000
#define TWF_CALLCMD 0x080000
#define TWF_BLKCMD  0x100000

class DebugCore;

class TraceView final
{
	DebugCore& core_;
	DebugView& view_;
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

	
public:
	const unsigned cs[3][2] = { {0,4}, {5,10}, {16,16} };

	u8 trace_labels{};
	unsigned asmii{};

	TraceView(DebugCore& core, DebugView& view, MemView& mem);

	auto disasm_line(unsigned addr, char *line) const -> int;

	auto cfindpc() const -> void;
	auto cfindtext() const -> void;
	auto cfindcode() const -> void;
	auto cgoto() const -> void;
	auto cbpx() const -> void;
	auto center() -> void;
	auto csetpc() const -> void;
	auto cup() const -> void;
	auto cdown() const -> void;
	auto cleft() const -> void;
	auto cright() const -> void;
	auto cpgup() -> void;
	auto cpgdn() -> void;
	auto pop_pos() -> void;
	auto cjump() -> void;
	auto cdjump() -> void;

	auto csave1() -> void;
	auto csave2() -> void;
	auto csave3() -> void;
	auto csave4() -> void;
	auto csave5() -> void;
	auto csave6() -> void;
	auto csave7() -> void;
	auto csave8() -> void;
	auto crest1() -> void;
	auto crest2() -> void;
	auto crest3() -> void;
	auto crest4() -> void;
	auto crest5() -> void;
	auto crest6() -> void;
	auto crest7() -> void;
	auto crest8() -> void;

	auto cfliplabels() -> void;
	static auto c_lbl_import() -> void;


	auto show_trace() -> void;
	auto dispatch_trace() -> char;
};
