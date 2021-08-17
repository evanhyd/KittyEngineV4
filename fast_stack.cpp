#include "fast_stack.h"


bool FastStack::Empty()
{
	return this->size == 0;
}
void FastStack::PushBack(const Boardstate& new_move)
{
	this->stack[this->size++] = new_move;
}
void FastStack::PopBack()
{
	--this->size;
}
void FastStack::Clear()
{
	this->size = 0;
}

Boardstate& FastStack::Back()
{
	return this->stack[size - 1];
}


FastStack::FastStack(int size)
{
	this->stack = new Boardstate[size]();
	this->size = size;
}