#pragma once
#include "debugger/libs/dbgwidgets.h"

class TsconfView
{
	DbgCanvas* canvas_{};

	auto init() const -> void;
public:
	TsconfView();

	auto render() const -> void;
};