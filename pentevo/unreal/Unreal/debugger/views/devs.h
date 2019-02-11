#pragma once
#include "debugger/debugger.h"

void __cdecl BankNames(int i, char *name);

class WatchView final: public IDebugViewPart
{
	unsigned show_scrshot_{};
	unsigned user_watches_[3] = { 0x4000, 0x8000, 0xC000 };

public:
	auto mon_setwatch() -> void;
	auto mon_scrshot() -> void;

	auto subscrible() -> void override;
	auto render() const -> void override;
};

struct StackView final: IDebugViewPart
{
	auto render() const -> void override;
};

struct AyView final: IDebugViewPart
{
	static auto mon_switchay() -> void;

	auto subscrible() -> void override;
	auto render() const -> void override;
};

class BanksView final: public IDebugViewPart
{
	unsigned selbank_ = 0;
	bool showbank_ = false;

	auto benter() const -> void;
public:

	auto editbank() const -> void;

	auto subscrible() -> void override;
	auto dispatch() const -> char override;
	auto render() const -> void override;
};

class PortsView final: public IDebugViewPart
{
	u16 dbg_extport_{};
	u8* dgb_extval_{}; // extended memory port like 1FFD or DFFD
public:
	auto editextbank() const -> void;

	auto subscrible() -> void override;
	auto render() const -> void override;
};

struct DosView final: IDebugViewPart
{
	auto render() const -> void override;
};

struct TimeView final: IDebugViewPart
{
	auto render() const -> void override;
};
