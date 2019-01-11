#pragma once

#include "sysdefs.h"
#include <initializer_list>
#include <string>
#include <functional>
#include <utility>
#include <vector>
#include <unordered_map>

class UIAction;
typedef std::function<void()> ActionFunctor;

class UIAction final
{
	std::vector<ActionFunctor> actions_{};
public:
	const std::string name;

	UIAction(std::string name, const ActionFunctor& action) : name(std::move(name))
	{
		actions_.push_back(action);
	}

	auto invoke()
	{
		for (auto& item : actions_)
			item();
	}
};

enum class ActionType
{
	main,
	monitor,
	reg,
};

class ActionManager
{
	static ActionManager * instance_;
	std::unordered_map<std::tuple<ActionType, std::string>, UIAction> actions_{};
public:

	static auto get_instance() -> ActionManager*
	{
		if (instance_ == nullptr)
			instance_ = new ActionManager;

		return instance_;
	}

	static auto registred(ActionType type, const std::string& name, ActionFunctor action) -> void
	{
		auto manager = get_instance();
		auto key = std::tuple<ActionType, std::string>(type, name);

		manager->actions_.emplace(key, action);
	}

	static auto invoke(ActionType type, std::string name) -> void
	{
		auto actions = get_instance()->actions_;
		const auto key = std::tuple<ActionType, std::string>(type, std::move(name));

		auto val = actions.find(key);
		
		if (val != actions.end())
			val->second.invoke();
	}
};