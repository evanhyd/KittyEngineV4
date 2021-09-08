#include "Connection.h"
#include <random>
#include <ctime>


Connection::Connection() : diff_weight(0.0)
{
	this->weight = double(rand()) / double(RAND_MAX);
}
