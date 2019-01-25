#include "std.h"
#include "actions.h"

std::vector<UIKey*> actions_registry = std::vector<UIKey*>();
Actions actions{};

UIKey::UIKey(ActionType type, std::string name): name(std::move(name)), type(type)
{
	actions_registry.push_back(this);
}

auto UIKey::is_empty() const -> bool
{
	return type == ActionType::empty;
}

auto UIKey::get_full_name() const -> std::string
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

auto get_actions(const std::vector<ActionType>& types) -> std::vector<UIKey*>
{
	auto result = std::vector<UIKey*>();

	for (auto type : types)
		for (auto item : actions_registry)
		{
			if (item->type == type)
				result.push_back(item);
		}

	return result;
}

