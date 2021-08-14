#include "random.h"

U32 GenerateRandom32()
{
	static U32 state = 1804289383;
	U32 random_num = state;
	random_num ^= random_num << 13;
	random_num ^= random_num >> 17;
	random_num ^= random_num << 5;

	state = random_num;
	return random_num;
}


U64 GenerateRandom64()
{
	U64 n1 = GenerateRandom32() & 0xffff;
	U64 n2 = GenerateRandom32() & 0xffff;
	U64 n3 = GenerateRandom32() & 0xffff;
	U64 n4 = GenerateRandom32() & 0xffff;

	return  n1 | n2 << 16 | n3 << 32 | n4 << 48;
}
