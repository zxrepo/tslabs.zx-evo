#pragma once

#define TWF_BRANCH  0x010000
#define TWF_BRADDR  0x020000
#define TWF_LOOPCMD 0x040000
#define TWF_CALLCMD 0x080000
#define TWF_BLKCMD  0x100000

class TraceView
{
	unsigned save_pos[8] = { UINT_MAX };
	unsigned save_cur[8] = { UINT_MAX };
	unsigned stack_pos[32] = { UINT_MAX }, stack_cur[32] = { UINT_MAX };

	auto csave(unsigned n) -> void;
	auto crest(unsigned n) -> void;

public:
	auto cfindpc() const -> void;
	auto cfindtext() -> void;
	auto cfindcode() -> void;
	auto cgoto() -> void;
	auto cbpx() const -> void;
	auto center() -> void;
	auto csetpc() const -> void;
	auto cup() -> void;
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
	auto c_lbl_import() -> void;


	auto show_trace() -> void;
	auto dispatch_trace() -> char;
};