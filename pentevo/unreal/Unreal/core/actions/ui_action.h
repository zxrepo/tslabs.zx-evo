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
	sys,

	empty,
};

template<class InType, class OutType>
class UIActionBase
{
protected:
	std::vector<std::function<OutType(InType)>> actions_{};

public:
	const ActionType type;
	const std::string name;

	UIActionBase(const ActionType type, std::string name) : type(type), name(std::move(name)) { }

	auto subscrible(const std::function<OutType(InType)>& action) -> void
	{
		actions_.push_back(action);
	}

	auto operator +=(const std::function<OutType(InType)>& action) -> void
	{
		subscrible(action);
	}

	auto is_empty() const -> bool
	{
		return type == ActionType::empty;
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
};

template<class InType = void, typename OutType = void>
class UIAction final: public UIActionBase<InType, OutType>
{
public:
	UIAction(ActionType type, const std::string& name) : UIActionBase<InType, OutType>(type, name) {}

	auto invoke(InType arg) -> OutType
	{
		static OutType result{};
		for (auto& item : actions_)
			result = item(arg);

		return result;
	}

	auto operator() (InType arg) -> OutType
	{
		return invoke(arg);
	}
};

extern std::vector<UIAction<>*> actions_registry;

template<class InType>
class UIAction<InType, void> final : public UIActionBase<InType, void>
{
public:
	UIAction(ActionType type, const std::string& name) : UIActionBase(type, name) { }

	auto invoke(InType arg) -> void
	{
		for (auto& item : actions_) item(arg);
	}

	auto operator() (InType arg) -> void
	{
		invoke(arg);
	}
};

template<>
class UIAction<void, void> final : public UIActionBase<void, void>
{
public:
	// only void(void) action may be binded to keyboard
	unsigned k1{}, k2{}, k3{}, k4{};

	UIAction(ActionType type, const std::string& name) : UIActionBase(type, name)
	{
		actions_registry.push_back(this);
	}

	auto invoke() -> void
	{
		for (auto& item : actions_) item();
	}

	auto operator() () -> void
	{
		invoke();
	}
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