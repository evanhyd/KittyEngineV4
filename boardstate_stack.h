#pragma once
#include "boardstate.h"

using namespace std;

class BoardstateStack
{
	Boardstate* stack;
	int size;

public:
	bool Empty();
	void PushBack(const Boardstate& new_boardstate);
	void PopBack();
	void Clear();
	int Size();
	Boardstate& Back();

	BoardstateStack(size_t size);
};

