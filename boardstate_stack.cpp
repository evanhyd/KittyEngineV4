#include "boardstate_stack.h"


bool BoardstateStack::Empty()
{
	return this->size == 0;
}
void BoardstateStack::PushBack(const Boardstate& new_boardstate)
{
	this->stack[this->size++] = new_boardstate;
}
void BoardstateStack::PopBack()
{
	--this->size;
}
void BoardstateStack::Clear()
{
	this->size = 0;
}
int BoardstateStack::Size()
{
	return this->size;
}
Boardstate& BoardstateStack::Back()
{
	return this->stack[size - 1];
}


BoardstateStack::BoardstateStack(size_t size)
{
	this->stack = new Boardstate[size]();
	this->size = size;
}