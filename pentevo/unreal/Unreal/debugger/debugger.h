#pragma once

#include "std.h"
#include "sysdefs.h"

#include "debugger/consts.h"
#include "core/ioc.h"
#include <vector>

class IDebugView
{
public:
	virtual ~IDebugView() = default;

	virtual auto show_dialog(__in LPCSTR lpTemplateName, __in_opt DLGPROC lpDialogFunc) const -> void = 0;

	virtual auto set_scr(u32 addr, u8 val) const -> void = 0;

	virtual auto flip() -> void = 0;
	virtual auto add_frame(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 attr) -> void = 0;

	virtual auto clear_canvas() -> void = 0;

	virtual auto tprint(unsigned x, unsigned y, const char *str, u8 attr) const -> void = 0;
	virtual auto tprint_fg(unsigned x, unsigned y, const char *str, u8 attr) const -> void = 0;
	virtual auto filledframe(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color = fframe_inside) -> void = 0;
	virtual auto fillattr(unsigned x, unsigned y, unsigned dx, u8 color = fframe_inside) const -> void = 0;
	virtual auto fillrect(unsigned x, unsigned y, unsigned dx, unsigned dy, u8 color = fframe_inside) const -> void = 0;
	virtual auto wtline(const char *name, unsigned ptr, unsigned y) const -> void = 0;

	virtual auto inputhex(unsigned x, unsigned y, unsigned sz, bool hex) -> unsigned = 0;
	virtual auto input4(unsigned x, unsigned y, unsigned val) -> unsigned = 0;
	virtual auto input2(unsigned x, unsigned y, unsigned val) -> unsigned = 0;
};

class IDebugViewPart;

extern std::vector<IDebugViewPart*> debug_parts;
extern IServiceLocator *service_locator;

class IDebugViewPart
{
protected:
	IDebugView &view_;	
public:
	IDebugViewPart() : view_(*service_locator->Locate<IDebugView>()) { };

	virtual ~IDebugViewPart() = default;

	virtual auto render() const -> void = 0;

	virtual auto subscrible() -> void { }
	virtual auto dispatch() const -> char { return 0; }
};

auto init_debugger() -> void;
