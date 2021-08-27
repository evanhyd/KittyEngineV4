#include "timer.h"

using namespace std;
using namespace std::chrono;

void Timer::StartTimer(long long remaining_time_ms, long long increment_time_ms)
{
	this->thinking_time_ms = max(remaining_time_ms / THINKING_TIME_RATIO, increment_time_ms);
	this->start_point = high_resolution_clock::now();
}
bool Timer::IsTimeOut()
{
	long long passed_time_ms = duration_cast<milliseconds>(high_resolution_clock::now() - this->start_point).count();

	return passed_time_ms > this->thinking_time_ms;
}