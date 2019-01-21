#include "std.h"
#include "ui_action.h"
#include "actions.h"

std::vector<UIAction<>*> actions_registry = std::vector<UIAction<>*>();
Actions actions{};

auto get_actions(const std::vector<ActionType>& types) -> std::vector<UIAction<>*>
{
	auto result = std::vector<UIAction<>*>();

	for (auto type : types)
		for (auto item : actions_registry)
		{
			if (item->type == type)
				result.push_back(item);
		}

	return result;
}

