#include "std.h"
#include "debugger.h"
#include "libs/view.h"

IServiceLocator *serviceLocator = new ServiceLocator();
std::vector<IDebugViewPart*> debug_parts{};

auto init_debugger() -> void
{
	serviceLocator->RegisterService<IDebugView>(new DebugView());
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