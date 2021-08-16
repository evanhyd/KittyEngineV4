#pragma once
#include "move.h"

using namespace std;

class FastStack
{
	Move* stack;
	int size;


public:

	int Size();
	bool Empty();
	void PushBack(Move new_move);
	void PopBack();
	Move& Front();
	Move& Back();
	Move& operator[](int index);

	FastStack(int size);
};

