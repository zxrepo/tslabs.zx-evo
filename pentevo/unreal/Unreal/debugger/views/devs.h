#pragma once
#include "debugger/libs/view.h"

void __cdecl BankNames(int i, char *name);

class WatchView final
{
	DebugView& view_;
	unsigned show_scrshot_{};
	unsigned user_watches_[3] = { 0x4000, 0x8000, 0xC000 };

	auto subsrible() -> void;
public:
	WatchView(DebugView& view);

	auto mon_setwatch() -> void;
	auto mon_scrshot() -> void;
	auto render() -> void;
};

class StackView final
{
	DebugView& view_;

public:
	StackView(DebugView& view);

	auto render() const -> void;
};

class AyView final
{
	DebugView& view_;

	static auto subscrible() -> void;
public:
	AyView(DebugView& view);

	static auto mon_switchay() -> void;
	auto render() const -> void;
};

class BanksView final
{
	unsigned selbank = 0;
	bool showbank = false;

	DebugView& view_;

	auto subscrible() -> void;
	auto benter() const -> void;
public:
	
	BanksView(DebugView& view);

	auto editbank() const -> void;

	auto dispatch() const -> char;
	auto render() const -> void;
};

class PortsView final
{
	DebugView& view_;

	unsigned dbg_extport{};
	u8 dgb_extval{}; // extended memory port like 1FFD or DFFD

	auto subscrible() -> void;
public:
	PortsView(DebugView& view);

	auto editextbank() const -> void;
	auto render() -> void;
};

class DosView final
{
	DebugView& view_;

public:
	DosView(DebugView& view);

	auto render() const -> void;
};

class TimeView final
{
	DebugView& view_;

public:
	TimeView(DebugView& view);

	auto render() const -> void;
};
