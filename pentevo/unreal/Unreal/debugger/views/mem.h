#pragma once
#include "sysdefs.h"
#include "wd93.h"
#include "debugger/view.h"
#include "debugger/core.h"

enum { ed_mem, ed_phys, ed_log, ed_cmos, ed_nvram, ed_max };

class MemView final
{
	DebugView &view_;
	DebugCore &core_;

	unsigned mem_disk{};
	unsigned mem_track{};
	unsigned mem_max{};

	TRKCACHE edited_track;

	unsigned sector_offset{};
	unsigned sector{};

	auto findsector(unsigned addr) -> void;
	auto editam(unsigned addr)->u8*;
	auto editwm(unsigned addr, u8 byte) -> void;
	
public:
	unsigned mem_sz = 8;
	u8 mem_dump{};
	u8 editor = ed_mem;
	u8 mem_ascii{};

	MemView(DebugCore& core, DebugView& view);

	auto memadr(unsigned addr) const -> unsigned;
	auto editrm(unsigned addr)->u8;

	auto mleft() const -> void;
	auto mright() const -> void;
	auto mup() const -> void;
	auto mdown() const -> void;
	auto mpgdn() const -> void;
	auto mpgup() const -> void;
	auto mswitch() -> void;
	auto mstl() const -> void;
	auto mendl() const -> void;
	auto mtext() const -> void;
	auto mcode() const -> void;
	auto mgoto() const -> void;
	auto mmodemem() -> void;
	auto mmodephys() -> void;
	auto mmodelog() -> void;
	auto mdiskgo() -> void;

	auto mpc() const -> void;
	auto msp() const -> void;
	auto mbc() const -> void;
	auto mde() const -> void;
	auto mhl() const -> void;
	auto mix() const -> void;
	auto miy() const -> void;
	auto show_mem() -> void;
	auto dispatch_mem() -> char;
};

