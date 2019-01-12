#pragma once
#include "sysdefs.h"
#include "debugger/core.h"
#include "wd93.h"
#include "debugger/libs/view.h"

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
	auto mdiskgo() -> void;

	auto subscrible() -> void;
	
public:
	unsigned mem_sz = 8;
	u8 mem_dump{};
	u8 editor = ed_mem;
	u8 mem_ascii{};

	MemView(DebugCore& core, DebugView& view);

	auto memadr(unsigned addr) const -> unsigned;
	auto editrm(unsigned addr)->u8;

	auto render() -> void;
	auto dispatch() -> char;
};
