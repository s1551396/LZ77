// Returns length of decompressed string

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cerrno>
#include "LZ77.h"

#define size 100

using namespace std;

int LZ77Decomp(LDPair input, char buffer[size], int window, int len) {
	#pragma HLS INLINE
	int i, j = 0;
	string s;


	for (i = window-input.offset; i < window-input.offset+input.length; i++) {
		if (i + input.offset < len) {
			buffer[i+input.offset] = buffer[i];
		}
		else {
			break;
		}
		j++;
	}

	if (i + input.offset < len) {
		buffer[i+input.offset] = input.symbol;
	}
	j++;

	return j;
}
