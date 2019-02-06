#pragma once
#include "debugger/debugger.h"

void __cdecl BankNames(int i, char *name);

class WatchView final
{
	IDebugView& view_;
	unsigned show_scrshot_{};
	unsigned user_watches_[3] = { 0x4000, 0x8000, 0xC000 };

	auto subsrible() -> void;
public:
	WatchView();

	auto mon_setwatch() -> void;
	auto mon_scrshot() -> void;
	auto render() -> void;
};

class StackView final
{
	IDebugView& view_;

public:
	StackView();

	auto render() const -> void;
};

class AyView final
{
	IDebugView& view_;

	static auto subscrible() -> void;
public:
	AyView();

	static auto mon_switchay() -> void;
	auto render() const -> void;
};

class BanksView final
{
	unsigned selbank = 0;
	bool showbank = false;

	IDebugView& view_;

	auto subscrible() -> void;
	auto benter() const -> void;
public:
	
	BanksView();

	auto editbank() const -> void;

	auto dispatch() const -> char;
	auto render() const -> void;
};

class PortsView final
{
	IDebugView& view_;

	unsigned dbg_extport{};
	u8 dgb_extval{}; // extended memory port like 1FFD or DFFD

	auto subscrible() -> void;
public:
	PortsView();

	auto editextbank() const -> void;
	auto render() -> void;
};

class DosView final
{
	IDebugView& view_;

public:
	DosView();

	auto render() const -> void;
};

class TimeView final
{
	IDebugView& view_;

public:
	TimeView();

	auto render() const -> void;
};
