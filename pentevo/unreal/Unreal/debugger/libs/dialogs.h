#pragma once
#include "view.h"
#include "debugger/views/mem.h"

class Dialogs
{
	DebugView& view_;
	MemView& mem_;

public:
	explicit Dialogs(DebugView& view, MemView& mem);

	auto find1dlg(unsigned start) const -> unsigned;
	auto find2dlg(unsigned start) const -> unsigned;
	auto mon_gsdialog() const -> void;
	auto mon_bpdialog() const -> void;

};
