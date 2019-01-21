#pragma once
#include "debugger/libs/dbgwidgets.h"
#include "debugger/libs/view.h"

class TsconfView
{
	DbgCanvas* canvas_{};

	auto init() const -> void;
public:
	TsconfView(DebugView& view);

	auto render() const -> void;
};