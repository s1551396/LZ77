#include <stdint.h>

struct LDPair {
	uint8_t offset;
	uint8_t length;
	char symbol;

	LDPair(uint8_t o, uint8_t l, char s) {
		offset = o;
		length = l;
		symbol = s;
	}

	LDPair() {
		offset = 0;
		length = 0;
		symbol = 'A';
	}

	void set(uint8_t o, uint8_t l, char s) {
		offset = o;
		length = l;
		symbol = s;
		return;
	}
};
