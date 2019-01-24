#include "std.h"
#include "actions.h"

std::vector<TAction<>*> actions_registry = std::vector<TAction<>*>();
Actions actions{};

auto get_actions(const std::vector<ActionType>& types) -> std::vector<TAction<>*>
{
	auto result = std::vector<TFunc<>*>();

	for (auto type : types)
		for (auto item : actions_registry)
		{
			if (item->type == type)
				result.push_back(item);
		}

	return result;
}

