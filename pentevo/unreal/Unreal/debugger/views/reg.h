#pragma once
#include "debugger/core.h"

struct TRegLayout final
{
	size_t offs;
	u8 width;
	u8 x, y;
	u8 lf, rt, up, dn;
};

extern const std::vector<TRegLayout> regs_layout;

class RegView final
{
	DebugCore &core_;
	DebugView &view_;
	MemView& mem_;

public:
	unsigned regs_curs{};

	RegView(DebugCore& core, DebugView& view, MemView& mem);

	auto ra() -> void;
	auto rf() -> void;
	auto rbc() -> void;
	auto rde() -> void;
	auto rhl() -> void;
	auto rsp() -> void;
	auto rpc() -> void;
	auto rix() -> void;
	auto riy() -> void;
	auto ri() -> void;
	auto rr() -> void;
	auto rm() -> void;
	auto r_1() -> void;
	auto r_2() -> void;
	auto rSF() -> void;
	auto rZF() -> void;
	auto rF5() -> void;
	auto rHF() -> void;
	auto rF3() -> void;
	auto rPF() -> void;
	auto rNF() -> void;
	auto rCF() -> void;

	auto rcodejump() const -> void;
	auto rdatajump() const -> void;

	auto rleft() -> void;
	auto rright() -> void;
	auto rup() -> void;
	auto rdown() -> void;
	auto renter() const -> void;

	auto show_regs() const -> void;
	auto dispatch_regs() const -> char;
};
