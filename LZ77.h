#include <stdint.h>

struct LDPair {
	uint8_t offset;
	uint8_t length;
	char symbol;
	LDPair * next;

	LDPair(uint8_t o, uint8_t l, char s) {
		offset = o;
		length = l;
		symbol = s;
		next = NULL;
	}

	LDPair() {
		offset = 0;
		length = 0;
		symbol = 'A';
		next = NULL;
	}

	void Set(uint8_t o, uint8_t l, char s) {
		offset = o;
		length = l;
		symbol = s;
		return;
	}

	~LDPair() {
		if (next != NULL) {
			delete next;
		}
	}
};
