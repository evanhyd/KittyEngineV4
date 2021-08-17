#pragma once
#include "boardstate.h"

using namespace std;

class FastStack
{
	Boardstate* stack;
	int size;

public:
	bool Empty();
	void PushBack(const Boardstate& new_move);
	void PopBack();
	void Clear();
	Boardstate& Back();

	FastStack(int size);
};

