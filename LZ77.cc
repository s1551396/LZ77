#include <iostream>
#include <string>
#include "LZ77.h"

#define size 100

using namespace std;

void LZ77(char S[size], int len, int window, LDPair &output) {
	#pragma HLS INLINE
	uint8_t o = 0, l = 0, o2 = 0, l2 = 0;
	char c = S[window];
	int i, j, k, m = 0;

	l = l2 = o2 = 0;

	for (j = 0; j < len; j++) {
		if ((l == 0 && j >= window) || window + j > len) {
			break;
		}
		if (S[j] == S[window+l]) {
			if (l <= 0) {
				o = window-j;
			}
			l++;
		}
		else {
			if (l > 0) {
				j -= l; // Reset search to character right past previous match
			}
			l = 0;
			o = 0;
		}
		if (l >= l2) {
			l2 = l;
			o2 = o;
			if (window + l < len) {
				c = S[window+l];
			}
		}
		if (window + l >= len) {
			c = S[len-1];
			break;
		}
	}
	output.set(o2, l2, c);
}
