#pragma once

#include "sysdefs.h"
#include <initializer_list>
#include <string>
#include <functional>
#include <utility>
#include <vector>
#include <unordered_map>

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
	static UIAction empty;
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

	auto invoke(Arg &arg) const
	{
		for (auto& item : actions_)
			item(arg);
	}

	auto operator +=(const std::function<void(Arg)>& action)
	{
		subscrible(action);
	}

	auto subscrible(const std::function<void(Arg)>& action)
	{
		actions_.push_back(action);
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

	UIAction<> Mon = UIAction<>(ActionType::monitor, "emul");
	UIAction<> Mon = UIAction<>(ActionType::monitor, "emul");
	UIAction<> Mon = UIAction<>(ActionType::monitor, "emul");
	UIAction<> Mon = UIAction<>(ActionType::monitor, "emul");
	UIAction<> Mon = UIAction<>(ActionType::monitor, "emul");


	UIAction<> BanksUp = UIAction<>(ActionType::banks, "up");
	UIAction<> BanksDown = UIAction<>(ActionType::banks, "down ");
	UIAction<> BanksEdit = UIAction<>(ActionType::banks, "edit");

	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");
	UIAction<> Mem = UIAction<>(ActionType::memory, "emul");


	ActionManager::subscrible(ActionType::memory, "left", );
	ActionManager::subscrible(ActionType::memory, "right", );
	ActionManager::subscrible(ActionType::memory, "up", );
	ActionManager::subscrible(ActionType::memory, "down", );
	ActionManager::subscrible(ActionType::memory, "pgup", );
	ActionManager::subscrible(ActionType::memory, "pgdn", );
	ActionManager::subscrible(ActionType::memory, "switch",);
	ActionManager::subscrible(ActionType::memory, "stline", );
	ActionManager::subscrible(ActionType::memory, "endline", );
	ActionManager::subscrible(ActionType::memory, "findtext", );
	ActionManager::subscrible(ActionType::memory, "findcode", );
	ActionManager::subscrible(ActionType::memory, "goto", );
	ActionManager::subscrible(ActionType::memory, "mem", [this]() { editor = ed_mem; });
	ActionManager::subscrible(ActionType::memory, "diskphys", [this]() { editor = ed_phys; });
	ActionManager::subscrible(ActionType::memory, "disklog", [this]() { editor = ed_log; });
	ActionManager::subscrible(ActionType::memory, "diskgo", [this]() { mdiskgo(); });
	ActionManager::subscrible(ActionType::memory, "pc", );
	ActionManager::subscrible(ActionType::memory, "sp", );
	ActionManager::subscrible(ActionType::memory, "bc", );
	ActionManager::subscrible(ActionType::memory, "de", );
	ActionManager::subscrible(ActionType::memory, "hl", );
	ActionManager::subscrible(ActionType::memory, "ix", );
	ActionManager::subscrible(ActionType::memory, "iy", );
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