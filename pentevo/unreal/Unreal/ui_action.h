#pragma once

#include <string>
#include <functional>
#include <utility>
#include <vector>

enum class ActionType
{
	main,
	monitor,
	reg,
	trace,
	memory,
	banks,
	xt,

	empty,
};

template<class Arg = std::_Any_tag>
class UIAction final
{
	std::vector<std::function<void(Arg&)>> actions_{};
public:
	static UIAction<> empty;
	const ActionType type;
	const std::string name;

	UIAction(const ActionType type, std::string name) : type(type), name(std::move(name)) {}

	auto is_empty() const -> bool
	{
		return type == ActionType::empty && name == "empty";
	}

	auto get_full_name() const -> std::string
	{
		std::string prefix;
		switch (type)
		{
		case ActionType::memory:
		case ActionType::banks:
			prefix = "mem.";
			break;
		case ActionType::xt:
		case ActionType::main:
			prefix = "main.";
			break;
		case ActionType::monitor:
			prefix = "mon.";
			break;
		case ActionType::reg:
			prefix = "reg.";
			break;
		case ActionType::trace:
			prefix = "cpu.";
			break;
		default:
			prefix = "`error#";
		}

		return prefix + name;
	}

	auto invoke(Arg& arg) const
	{
		for (auto& item : actions_)
			item(arg);
	}

	auto subscrible(const std::function<void(Arg)> action)
	{
		actions_.push_back(action);
	}

	auto operator +=(const std::function<void(Arg)>& action)
	{
		subscrible(action);
	}

	auto operator() (Arg& arg)
	{
		invoke(arg);
	}
	
};

template<>
class UIAction<> final
{
	std::vector<std::function<void()>> actions_{};
public:
	static UIAction<> empty;
	const ActionType type;
	const std::string name;

	UIAction(const ActionType type, std::string name) : type(type), name(std::move(name)) {}

	auto is_empty() const -> bool
	{
		return type == ActionType::empty && name == "empty";
	}

	auto get_full_name() const -> std::string
	{
		std::string prefix;
		switch (type)
		{
		case ActionType::memory:
		case ActionType::banks:
			prefix = "mem.";
			break;
		case ActionType::xt:
		case ActionType::main:
			prefix = "main.";
			break;
		case ActionType::monitor:
			prefix = "mon.";
			break;
		case ActionType::reg:
			prefix = "reg.";
			break;
		case ActionType::trace:
			prefix = "cpu.";
			break;
		default:
			prefix = "`error#";
		}

		return prefix + name;
	}

	auto invoke() const
	{
		for (auto& item : actions_)
			item();
	}

	auto subscrible(const std::function<void()> action)
	{
		actions_.push_back(action);
	}

	auto operator +=(const std::function<void()>& action)
	{
		subscrible(action);
	}

	auto operator() () const
	{
		invoke();
	}
};

class Actions final
{
public:
	UIAction<> MonEmul = UIAction<>(ActionType::monitor, "emul");
	UIAction<> MonSaveBlock = UIAction<>(ActionType::monitor, "saveblock");
	UIAction<> MonLoadBlock = UIAction<>(ActionType::monitor, "loadblock");
	UIAction<> MonFillBlock = UIAction<>(ActionType::monitor, "fillblock");
	UIAction<> MonStep = UIAction<>(ActionType::monitor, "step");
	UIAction<> MonStepOver = UIAction<>(ActionType::monitor, "stepover");
	UIAction<> MonExitSub = UIAction<>(ActionType::monitor, "exitsub");
	UIAction<> MonDump = UIAction<>(ActionType::monitor, "dump");

	UIAction<> MonSwitchDump = UIAction<>(ActionType::monitor, "switchdump");
	UIAction<> MonNext = UIAction<>(ActionType::monitor, "next");
	UIAction<> MonPrev = UIAction<>(ActionType::monitor, "prev");
	UIAction<> MonRipper = UIAction<>(ActionType::monitor, "rip");
	UIAction<> MonHelp = UIAction<>(ActionType::monitor, "help");
	UIAction<> MonCpu = UIAction<>(ActionType::monitor, "cpu");
	UIAction<> MonExit = UIAction<>(ActionType::monitor, "exit");

	UIAction<> MonPokeDialog = UIAction<>(ActionType::monitor, "pokedialog");
	UIAction<> MonTapeBrowser = UIAction<>(ActionType::monitor, "tapebrowser");
	UIAction<> MonReset = UIAction<>(ActionType::monitor, "reset");
	UIAction<> MonReset128 = UIAction<>(ActionType::monitor, "reset128");
	UIAction<> MonResetSys = UIAction<>(ActionType::monitor, "resetsys");
	UIAction<> MonReset48 = UIAction<>(ActionType::monitor, "reset48");
	UIAction<> MonResetBasic = UIAction<>(ActionType::monitor, "resetbasic");
	UIAction<> MonResetDos = UIAction<>(ActionType::monitor, "resetdos");
	UIAction<> MonResetCache = UIAction<>(ActionType::monitor, "resetcache");
	UIAction<> MonSetWatch = UIAction<>(ActionType::monitor, "setwatch");
	UIAction<> MonScreenShot = UIAction<>(ActionType::monitor, "scrshot");
	UIAction<> MonSwitchAY = UIAction<>(ActionType::monitor, "switchay");

	UIAction<> MonNmi = UIAction<>(ActionType::monitor, "nmi");
	UIAction<> MonNmiDos = UIAction<>(ActionType::monitor, "nmidos");
	UIAction<> MonNmiCache = UIAction<>(ActionType::monitor, "nmicache");
	UIAction<> MonSaveSnap = UIAction<>(ActionType::monitor, "save");
	UIAction<> MonLoadSnap = UIAction<>(ActionType::monitor, "load");
	UIAction<> MonSaveSound = UIAction<>(ActionType::monitor, "savesound");
	UIAction<> MonQSave1 = UIAction<>(ActionType::monitor, "qsave1");
	UIAction<> MonQSave2 = UIAction<>(ActionType::monitor, "qsave2");
	UIAction<> MonQSave3 = UIAction<>(ActionType::monitor, "qsave3");
	UIAction<> MonQLoad1 = UIAction<>(ActionType::monitor, "qload1");
	UIAction<> MonQLoad2 = UIAction<>(ActionType::monitor, "qload2");
	UIAction<> MonQLoad3 = UIAction<>(ActionType::monitor, "qload3");
	UIAction<> MonLabels = UIAction<>(ActionType::monitor, "labels");
	UIAction<> MonSetBank = UIAction<>(ActionType::monitor, "setbank");
	UIAction<> MonMemSearch = UIAction<>(ActionType::monitor, "memsearch");
	UIAction<> MonSetHiMem = UIAction<>(ActionType::monitor, "sethimem");

	UIAction<> MainExit = UIAction<>(ActionType::main, "exit");
	UIAction<> MainMonitor = UIAction<>(ActionType::main, "monitor");
	UIAction<> MainFullSceen = UIAction<>(ActionType::main, "fullscreen");
	UIAction<> MainPause = UIAction<>(ActionType::main, "pause");
	UIAction<> MainSelectFix = UIAction<>(ActionType::main, "selectfix");
	UIAction<> MainSelectSnd = UIAction<>(ActionType::main, "selectsnd");
	UIAction<> MainIncFix = UIAction<>(ActionType::main, "incfix");
	UIAction<> MainDecFix = UIAction<>(ActionType::main, "decfix");
	UIAction<> MainIncFix10 = UIAction<>(ActionType::main, "incfix10");
	UIAction<> MainDecFix10 = UIAction<>(ActionType::main, "decfix10");

	UIAction<> MainLeds = UIAction<>(ActionType::main, "leds");
	UIAction<> MainStatus = UIAction<>(ActionType::main, "status");
	UIAction<> MainMaxSpeed = UIAction<>(ActionType::main, "maxspeed");
	UIAction<> MainSelectFilter = UIAction<>(ActionType::main, "selectfilter");
	UIAction<> MainSelectDriver = UIAction<>(ActionType::main, "selectdriver");
	UIAction<> MainPokeDialog = UIAction<>(ActionType::main, "pokedialog");
	UIAction<> MainStartTape = UIAction<>(ActionType::main, "starttape");
	UIAction<> MainScreenShoot = UIAction<>(ActionType::main, "screenshot");
	UIAction<> MainSaveVideo = UIAction<>(ActionType::main, "savevideo");

	UIAction<> MainReset = UIAction<>(ActionType::main, "reset");
	UIAction<> MainReset128 = UIAction<>(ActionType::main, "reset128");
	UIAction<> MainResetSys = UIAction<>(ActionType::main, "resetsys");
	UIAction<> MainReset48 = UIAction<>(ActionType::main, "reset48");
	UIAction<> MainResetBasic = UIAction<>(ActionType::main, "resetbasic");
	UIAction<> MainResetDos = UIAction<>(ActionType::main, "resetdos");
	UIAction<> MainResetCache = UIAction<>(ActionType::main, "resetcache");

	UIAction<> MainNmi = UIAction<>(ActionType::main, "nmi");
	UIAction<> MainNmiDos = UIAction<>(ActionType::main, "nmidos");
	UIAction<> MainNmiCache = UIAction<>(ActionType::main, "nmicache");

	UIAction<> MainTapeBrowser = UIAction<>(ActionType::main, "tapebrowser");
	UIAction<> MainSettings = UIAction<>(ActionType::main, "settings");
	UIAction<> MainSaveSnap = UIAction<>(ActionType::main, "save");
	UIAction<> MainLoadSnap = UIAction<>(ActionType::main, "load");
	UIAction<> MainSaveSound = UIAction<>(ActionType::main, "savesound");
	UIAction<> MainQSave1 = UIAction<>(ActionType::main, "qsave1");
	UIAction<> MainQSave2 = UIAction<>(ActionType::main, "qsave2");
	UIAction<> MainQSave3 = UIAction<>(ActionType::main, "qsave3");
	UIAction<> MainQLoad1 = UIAction<>(ActionType::main, "qload1");
	UIAction<> MainQLoad2 = UIAction<>(ActionType::main, "qload2");
	UIAction<> MainQLoad3 = UIAction<>(ActionType::main, "qload3");

	UIAction<> MainKeyStick = UIAction<>(ActionType::main, "keystick");
	UIAction<> MainAutoFire = UIAction<>(ActionType::main, "autofire");
	UIAction<> MainSaveRam = UIAction<>(ActionType::main, "saveram");
	UIAction<> MainSaveAll = UIAction<>(ActionType::main, "saveall");
	UIAction<> MainLockMouse = UIAction<>(ActionType::main, "lockmouse");
	UIAction<> MainAtmKeyboard = UIAction<>(ActionType::main, "xtkbd");
	UIAction<> MainPasteText = UIAction<>(ActionType::main, "pastetext");
	UIAction<> MainSize1 = UIAction<>(ActionType::main, "size1");
	UIAction<> MainSize2 = UIAction<>(ActionType::main, "size2");
	UIAction<> MainSizeMax = UIAction<>(ActionType::main, "sizem");
	UIAction<> MainMemSearch = UIAction<>(ActionType::main, "memsearch");
	UIAction<> MainHelp = UIAction<>(ActionType::main, "help");
	UIAction<> MainTsuToggle = UIAction<>(ActionType::main, "tsutoggle");
	UIAction<> MainFlicToggle = UIAction<>(ActionType::main, "flictoggle");

	UIAction<> AtmKeyboard = UIAction<>(ActionType::xt, "xtkbd");

	UIAction<> BanksUp = UIAction<>(ActionType::banks, "up");
	UIAction<> BanksDown = UIAction<>(ActionType::banks, "down ");
	UIAction<> BanksEdit = UIAction<>(ActionType::banks, "edit");

	UIAction<> MemLeft = UIAction<>(ActionType::memory, "left");
	UIAction<> MemRight = UIAction<>(ActionType::memory, "right");
	UIAction<> MemUp = UIAction<>(ActionType::memory, "up");
	UIAction<> MemDown = UIAction<>(ActionType::memory, "down");
	UIAction<> MemPgUp = UIAction<>(ActionType::memory, "pgup");
	UIAction<> MemPgDown = UIAction<>(ActionType::memory, "pgdn");

	UIAction<> MemSwitch = UIAction<>(ActionType::memory, "switch");
	UIAction<> MemStartLine = UIAction<>(ActionType::memory, "stline");
	UIAction<> MemEndLine = UIAction<>(ActionType::memory, "endline");
	UIAction<> MemFindText = UIAction<>(ActionType::memory, "findtext");
	UIAction<> MemFindCode = UIAction<>(ActionType::memory, "findcode");

	UIAction<> MemGoto = UIAction<>(ActionType::memory, "goto");
	UIAction<> MemView = UIAction<>(ActionType::memory, "mem");
	UIAction<> MemDiskPhys = UIAction<>(ActionType::memory, "diskphys");
	UIAction<> MemDiskLog = UIAction<>(ActionType::memory, "disklog");
	UIAction<> MemDiskGo = UIAction<>(ActionType::memory, "diskgo");

	UIAction<> MemViewPC = UIAction<>(ActionType::memory, "pc");
	UIAction<> MemViewSP = UIAction<>(ActionType::memory, "sp");
	UIAction<> MemViewBC = UIAction<>(ActionType::memory, "bc");
	UIAction<> MemViewDE = UIAction<>(ActionType::memory, "de");
	UIAction<> MemViewHL = UIAction<>(ActionType::memory, "hl");
	UIAction<> MemViewIX = UIAction<>(ActionType::memory, "ix");
	UIAction<> MemViewIY = UIAction<>(ActionType::memory, "iy");

	UIAction<> RegLeft = UIAction<>(ActionType::reg, "left");
	UIAction<> RegRight = UIAction<>(ActionType::reg, "right");
	UIAction<> RegUp = UIAction<>(ActionType::reg, "up");
	UIAction<> RegDown = UIAction<>(ActionType::reg, "down");
	UIAction<> RegEdit = UIAction<>(ActionType::reg, "edit");

	UIAction<> RegA = UIAction<>(ActionType::reg, "a");
	UIAction<> RegF = UIAction<>(ActionType::reg, "f");
	UIAction<> RegBC = UIAction<>(ActionType::reg, "bc");
	UIAction<> RegDE = UIAction<>(ActionType::reg, "de");
	UIAction<> RegHL = UIAction<>(ActionType::reg, "hl");
	UIAction<> RegPC = UIAction<>(ActionType::reg, "pc");
	UIAction<> RegSP = UIAction<>(ActionType::reg, "sp");
	UIAction<> RegIX = UIAction<>(ActionType::reg, "ix");
	UIAction<> RegIY = UIAction<>(ActionType::reg, "iy");
	UIAction<> RegI = UIAction<>(ActionType::reg, "i");
	UIAction<> RegR = UIAction<>(ActionType::reg, "r");

	UIAction<> RegIM = UIAction<>(ActionType::reg, "im");
	UIAction<> RegIFF1 = UIAction<>(ActionType::reg, "iff1");
	UIAction<> RegIFF2 = UIAction<>(ActionType::reg, "iff2");

	UIAction<> RegSF = UIAction<>(ActionType::reg, "SF");
	UIAction<> RegZF = UIAction<>(ActionType::reg, "ZF");
	UIAction<> RegF5 = UIAction<>(ActionType::reg, "F5");
	UIAction<> RegHF = UIAction<>(ActionType::reg, "HF");
	UIAction<> RegF3 = UIAction<>(ActionType::reg, "F3");
	UIAction<> RegPF = UIAction<>(ActionType::reg, "PF");
	UIAction<> RegNF = UIAction<>(ActionType::reg, "NF");
	UIAction<> RegCF = UIAction<>(ActionType::reg, "CF");

	UIAction<> RegCodeJump = UIAction<>(ActionType::reg, "codejump");
	UIAction<> RegDataJump = UIAction<>(ActionType::reg, "datajump");

	UIAction<> TraceFindPC = UIAction<>(ActionType::trace, "findpc");
	UIAction<> TraceHere = UIAction<>(ActionType::trace, "here");
	UIAction<> TraceFindText = UIAction<>(ActionType::trace, "findtext");
	UIAction<> TraceFindCode = UIAction<>(ActionType::trace, "findcode");
	UIAction<> TraceGoto = UIAction<>(ActionType::trace, "goto");
	UIAction<> TraceBpx = UIAction<>(ActionType::trace, "bpx");
	UIAction<> TraceAsm = UIAction<>(ActionType::trace, "asm");
	UIAction<> TraceSetPC = UIAction<>(ActionType::trace, "setpc");

	UIAction<> TraceUp = UIAction<>(ActionType::trace, "up");
	UIAction<> TraceDown = UIAction<>(ActionType::trace, "down");
	UIAction<> TraceLeft = UIAction<>(ActionType::trace, "left");
	UIAction<> TraceRight = UIAction<>(ActionType::trace, "right");
	UIAction<> TracePgUp = UIAction<>(ActionType::trace, "pgdn");
	UIAction<> TracePgDown = UIAction<>(ActionType::trace, "pgup");

	UIAction<> TraceSave1 = UIAction<>(ActionType::trace, "save1");
	UIAction<> TraceSave2 = UIAction<>(ActionType::trace, "save2");
	UIAction<> TraceSave3 = UIAction<>(ActionType::trace, "save3");
	UIAction<> TraceSave4 = UIAction<>(ActionType::trace, "save4");
	UIAction<> TraceSave5 = UIAction<>(ActionType::trace, "save5");
	UIAction<> TraceSave6 = UIAction<>(ActionType::trace, "save6");
	UIAction<> TraceSave7 = UIAction<>(ActionType::trace, "save7");
	UIAction<> TraceSave8 = UIAction<>(ActionType::trace, "save8");

	UIAction<> TraceRestore1 = UIAction<>(ActionType::trace, "crest1");
	UIAction<> TraceRestore2 = UIAction<>(ActionType::trace, "crest2");
	UIAction<> TraceRestore3 = UIAction<>(ActionType::trace, "crest3");
	UIAction<> TraceRestore4 = UIAction<>(ActionType::trace, "crest4");
	UIAction<> TraceRestore5 = UIAction<>(ActionType::trace, "crest5");
	UIAction<> TraceRestore6 = UIAction<>(ActionType::trace, "crest6");
	UIAction<> TraceRestore7 = UIAction<>(ActionType::trace, "crest7");
	UIAction<> TraceRestore8 = UIAction<>(ActionType::trace, "crest8");

	UIAction<> TraceBack = UIAction<>(ActionType::trace, "back");
	UIAction<> TraceContext = UIAction<>(ActionType::trace, "context");
	UIAction<> TraceDataJump = UIAction<>(ActionType::trace, "datajump");
	UIAction<> TraceLabels = UIAction<>(ActionType::trace, "labels");
	UIAction<> TraceImportLabels = UIAction<>(ActionType::trace, "importl");

	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
	//UIAction<> Trace = UIAction<>(ActionType::trace, "emul");
};

/*

class ActionManager final
{
	static ActionManager * instance_;
	std::vector<UIActionBase> actions_{};
public:

	static auto get_instance() -> ActionManager*
	{
		if (instance_ == nullptr)
			instance_ = new ActionManager;

		return instance_;
	}

	static auto subscrible(ActionType type, const std::string& name, ActionFunctor action) -> void
	{
		auto item = find(type, name);

		if (item.is_empty())
			get_instance()->actions_.emplace_back(type, name, action);
		else
			item.subscrible(action);
	}

	static auto find(ActionType type, const std::string& name) -> UIAction&
	{
		for (auto& item : get_instance()->actions_)
		{
			if (item.type == type && item.name == name)
				return item;
		}

		return UIAction::empty;
	}

	static auto invoke(const ActionType type, const std::string& name) -> void
	{
		auto item = find(type, name);

		if (!item.is_empty())
			item.invoke();
	}

	static auto get_actions(const std::vector<ActionType>& types) -> std::vector<UIAction*>
	{
		auto result = std::vector<UIAction*>();

		for (auto& type : types)
			for (auto& item : get_instance()->actions_)
			{
				if (item.type == type)
					result.push_back(&item);
			}

		return result;
	}
};

*/