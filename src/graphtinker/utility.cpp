#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "graphtinker/graphtinker.h"

namespace gt
{
	uint Graphtinker::bitcombine_AB(uint A, uint B)
	{
		uint bitcomb;

		if ((A == 0) && (B == 0))
		{
			bitcomb = 00;
		}
		else if ((A == 0) && (B == 1))
		{
			bitcomb = 01;
		}
		else if ((A == 1) && (B == 0))
		{
			bitcomb = 10;
		}
		else if ((A == 1) && (B == 1))
		{
			bitcomb = 11;
		}
		else
		{
			//printf("Shouldn't get here: 45\n" );
		}
		return bitcomb;
	}

	uint Graphtinker::bitselect_ABC(uint A, uint B, uint C)
	{
		uint bitsel;

		if ((A == 0) && (B == 0) && (C == 1))
		{
			bitsel = 001;
		}
		else if ((A == 0) && (B == 1) && (C == 0))
		{
			bitsel = 010;
		}
		else if ((A == 1) && (B == 0) && (C == 0))
		{
			bitsel = 100;
		}
		else
		{
			//printf("Shouldn't get here: 45\n" );
		}
		return bitsel;
	}

	float Graphtinker::min_float(float A, float B)
	{
		if (A < B)
		{
			return A;
		}
		else
		{
			return B;
		}
	}

	vertexdata_t Graphtinker::min_type1(vertexdata_t A, vertexdata_t B)
	{
		if (A < B)
		{
			return A;
		}
		else
		{
			return B;
		}
	}

	uint Graphtinker::getpartitionid(uint vid)
	{
		return (vid % page_block_height_) / sub_block_height_;
	}
} // namespace gt