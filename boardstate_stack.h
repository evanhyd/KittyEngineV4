#pragma once
#include "boardstate.h"

class BoardstateStack
{
	Boardstate* stack;
	size_t size;

public:
	bool Empty();
	void PushBack(const Boardstate& new_boardstate);
	void PopBack();
	void Clear();
	size_t Size();
	Boardstate& Back();
	const Boardstate* Data() const;

	BoardstateStack(size_t size);
};

