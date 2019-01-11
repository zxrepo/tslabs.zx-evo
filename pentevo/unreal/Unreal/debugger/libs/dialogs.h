#pragma once
#include "view.h"
#include "debugger/views/mem.h"

unsigned calc(const Z80 *cpu, unsigned *script);

class Dialogs final
{
	DebugView& view_;
	MemView& mem_;

public:
	static auto mon_setup_dlg() -> void;

	explicit Dialogs(DebugView& view, MemView& mem);

	auto find1dlg(unsigned start) const -> unsigned;
	auto find2dlg(unsigned start) const -> unsigned;
	auto mon_gsdialog() const -> void;
	auto mon_bpdialog() const -> void;
	auto mon_watchdialog() const -> void;

};
