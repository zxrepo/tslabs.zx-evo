#include "std.h"
#include "debugger.h"
#include "libs/view.h"
#include "views/devs.h"

IServiceLocator *service_locator = new ServiceLocator();
std::vector<IDebugViewPart*> debug_parts{};

auto init_parts() -> void
{
	debug_parts.push_back(new WatchView());
	debug_parts.push_back(new StackView());
	debug_parts.push_back(new AyView());
	debug_parts.push_back(new BanksView());
	debug_parts.push_back(new PortsView());
	debug_parts.push_back(new DosView());
	debug_parts.push_back(new TimeView());

	for(auto& item: debug_parts)
		item->subscrible();
}

auto init_debugger() -> void
{
	service_locator->RegisterService<IDebugView>(new DebugView());
	init_parts();
}

/*

view_ = new DebugView(wnd);
mem_ = new MemView(*view_);
dialogs_ = new Dialogs(*view_, *mem_);
regs_ = new RegView(*view_, *mem_);

watch_ = new WatchView(*view_);
stack_ = new StackView(*view_);
ay_ = new AyView(*view_);
banks_ = new BanksView(*view_);
ports_ = new PortsView(*view_);
dos_ = new DosView(*view_);
time_ = new TimeView(*view_);
trace_ = new TraceView(*view_, *mem_);
tsconf_ = new TsconfView(*view_);

*/