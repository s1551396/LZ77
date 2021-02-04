// Compresses string in S[size] to an LDPair in output.
#include <stdint.h>
#include "LZ77.h"

#define size 1024

void LZ77(LDPair &output, char buffer[size], int window, int len) {
	#pragma HLS INLINE
	uint8_t o = 0, l = 0, o2 = 0, l2 = 0;
	char c = buffer[window];

	l = l2 = o2 = 0;

	for (int i = 0; i < len; i++) {
		if ((l == 0 && i >= window) || window + i > len) {
			break;
		}
		if (buffer[i] == buffer[window+l]) {
			if (l <= 0) {
				o = window-i;
			}
			l++;
		}
		else {
			if (l > 0) {
				i -= l; // Reset search to character right past previous match
			}
			l = 0;
			o = 0;
		}
		if (l >= l2) {
			l2 = l;
			o2 = o;
			if (window + l < len) {
				c = buffer[window+l];
			}
			else {
				l2--; // Decrement length to avoid duplicating last symbol
			}
		}
		if (window + l >= len) {
			c = buffer[len-1];
			break;
		}
	}
	output.set(o2, l2, c);
}
