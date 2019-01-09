#pragma once
#include "view.h"

class Dialogs
{
	DebugView * view_{};

public:
	explicit Dialogs(DebugView *view);

	auto find1dlg(unsigned start) -> unsigned;
	auto find2dlg(unsigned start) -> unsigned;

};
