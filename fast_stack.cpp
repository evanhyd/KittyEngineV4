#include "fast_stack.h"


int FastStack::Size()
{
	return this->size;
}
bool FastStack::Empty()
{
	return this->size == 0;
}
void FastStack::PushBack(Move new_move)
{
	this->stack[this->size++] = new_move;
}
void FastStack::PopBack()
{
	--this->size;
}

Move& FastStack::Front()
{
	return this->stack[0];
}

Move& FastStack::Back()
{
	return this->stack[size - 1];
}

Move& FastStack::operator[](int index)
{
	return this->stack[index];
}

FastStack::FastStack(int size)
{
	this->stack = new Move[size]();
	this->size = size;
}