#pragma once
#include "boardstate.h"

using namespace std;

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

	BoardstateStack(size_t size);
};

