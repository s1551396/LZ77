#include <iostream>
#include <string>
#include "LZ77.h"

using namespace std;

void LZ77(char S[13], int len, int window, char EOS, LDPair * output);

int main() {
	char S[13] = "ABCDABCDABBA";
	char S2[13] = "ABABCBABABAA";
	int len = sizeof(S)/sizeof(*S) - 1;
	int window = 4;
	char EOS = '$';							// Make sure this character does not appear in the input string
	LDPair * output;
	LDPair * i;
	output = new LDPair;
	i = output;

	LZ77(S, len, window, EOS, output);
	cout << endl;

	while (i != NULL || i->symbol != EOS) {
		cout << "Offset: " << (unsigned)i->offset << ", Length: " << (unsigned)i->length << ", Symbol: " <<  i->symbol << "." << endl;
		cout << "EOS: " << EOS << endl;
		cout << "i->symbol: " << i->symbol << endl;
		i = i->next;
	}

	return 0;
}
