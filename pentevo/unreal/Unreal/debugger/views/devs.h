#pragma once
#include "debugger/core.h"

void __cdecl BankNames(int i, char *name);

class WatchView final
{
	DebugCore& core_;
	DebugView& view_;
	unsigned show_scrshot{};

	unsigned user_watches[3] = { 0x4000, 0x8000, 0xC000 };
public:
	WatchView(DebugCore& core, DebugView& view);

	auto mon_setwatch() -> void;
	auto render() -> void;
};

class StackView final
{
	DebugCore& core_;
	DebugView& view_;

public:
	StackView(DebugCore& core, DebugView& view);

	auto render() const -> void;
};

class AyView final
{
	DebugCore& core_;
	DebugView& view_;

public:
	AyView(DebugCore& core, DebugView& view);

	auto mon_switchay() const -> void;
	auto render() const -> void;
};

class BanksView final
{
	DebugCore& core_;
	DebugView& view_;

	unsigned selbank = 0;
	bool showbank = false;

public:
	BanksView(DebugCore& core, DebugView& view);

	auto benter() -> void;
	auto bup() -> void;
	auto bdown() -> void;
	auto editbank() -> void;

	auto dispatch_banks() -> char;
	auto render() const -> void;
};

class PortsView final
{
	DebugCore& core_;
	DebugView& view_;

	unsigned dbg_extport{};
	u8 dgb_extval{}; // extended memory port like 1FFD or DFFD
public:
	PortsView(DebugCore& core, DebugView& view);

	auto editextbank() -> void;
	auto render() -> void;
};

class DosView final
{
	DebugCore& core_;
	DebugView& view_;

public:
	DosView(DebugCore& core, DebugView& view);

	auto render() const -> void;
};

class TimeView final
{
	DebugCore& core_;
	DebugView& view_;

public:
	TimeView(DebugCore& core, DebugView& view);

	auto render() const -> void;
};
