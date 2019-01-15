#pragma once
#include "debugger/core.h"
#include "debugger/libs/view.h"

void __cdecl BankNames(int i, char *name);

class WatchView final
{
	DebugCore& core_;
	DebugView& view_;
	unsigned show_scrshot{};
	unsigned user_watches[3] = { 0x4000, 0x8000, 0xC000 };

	auto subsrible() -> void;
public:
	WatchView(DebugCore& core, DebugView& view);

	auto mon_setwatch() -> void;
	auto mon_scrshot() -> void;
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

	static auto subscrible() -> void;
public:
	AyView(DebugCore& core, DebugView& view);

	static auto mon_switchay() -> void;
	auto render() const -> void;
};

class BanksView final
{
	DebugCore& core_;
	DebugView& view_;

	auto subscrible() -> void;

	auto benter() const -> void;
public:
	unsigned selbank = 0;
	bool showbank = false;

	BanksView(DebugCore& core, DebugView& view);

	auto editbank() const -> void;

	auto dispatch() const -> char;
	auto render() const -> void;
};

class PortsView final
{
	DebugCore& core_;
	DebugView& view_;

	unsigned dbg_extport{};
	u8 dgb_extval{}; // extended memory port like 1FFD or DFFD

	auto subscrible() -> void;
public:
	PortsView(DebugCore& core, DebugView& view);

	auto editextbank() const -> void;
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
