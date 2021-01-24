#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cerrno>
#include "LZ77.h"

#define size 1024

using namespace std;

void LZ77(char S[size], int len, int window, LDPair &output);
int LZ77Decomp(LDPair input, char buffer[size], int window, int len);

int main() {
    string text;
    string fileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/Canterbury/alice29.txt";
    string compFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyComp/alice29.txt";
    string decompFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyDecomp/alice29.txt";
    ifstream file(fileName, ios::in | ios::binary);
    fstream fileComp(compFileName, fstream::out | fstream::in| ios::binary);
    int j;
    uint8_t o, l;
    char s;
	char buffer[size] = {'\0'};
	char S3[size];
	int len = size;
	int newCharacters;
	const int window = 255;
	int window2 = 0;			// Used during compression when window isn't filled yet
	LDPair output;
	LDPair input;


	if(!file || file.fail() || !file.is_open()) {
		cerr << "Read failed: " << strerror(errno);
		return 1;
	}

	// Compress
	newCharacters = size;

	while (newCharacters > 0 && file.peek() != EOF) {
		buffer[len-newCharacters] = file.get();
		newCharacters--;
	}
	len -= newCharacters;
	newCharacters = 0;

	while (len > window2) {

		if (len < 300) {
			j = 5;
		}

		LZ77(buffer, len, window2, output);

		fileComp << output.offset << output.length << output.symbol;

		window2 += output.length+1;
		if (window2 >= window) {
			newCharacters = window2 - window;
			window2 = window;
		}

		for (int i = 0; i < len-newCharacters; i++) {
			buffer[i] = buffer[i+newCharacters];
		}


		while (newCharacters > 0 && file.peek() != EOF) {
			buffer[len-newCharacters] = file.get();
			newCharacters--;
		}
		len -= newCharacters;
		newCharacters = 0;
	}

	if(!fileComp || fileComp.fail() || !fileComp.is_open()) {
		cerr << "Write failed: " << strerror(errno);
		return 1;
	}

	file.close();
	//fileComp.close();



	// Decompress
	fileComp.seekg(0);
	//fileComp(compFileName, fstream::in | ios::binary);
	fstream fileDecomp(decompFileName, fstream::out | ios::binary);

	for (int i = 0; i < size; i++) {
		buffer[i] = '\0';
	}

	while (fileComp.peek() != EOF) {

		o = (uint8_t)fileComp.get();
		l = (uint8_t)fileComp.get();
		s = fileComp.get();

		if (fileComp.peek() == EOF) {
			j += 1;
		}

		input.set(o, l, s);

		if (!fileComp || fileComp.fail() || !fileComp.is_open()) {
			cerr << "Read failed: " << strerror(errno);
			return 1;
		}

		len = LZ77Decomp(input, buffer, window, size);

		j = len;
		for (int i = window; i < size; i++) {
			if (j <= 0)
				break;
			fileDecomp << buffer[i];
			j--;
		}

		// Move last window characters to start of buffer
		j = len;
		for (int i = 0; i < window; i++) {
			buffer[i] = buffer[j];
			j++;
		}

		// Clear buffer except for first window characters
		for (int i = window; i < size; i++) {
			buffer[i] = '\0';
		}


		if (!fileDecomp || fileDecomp.fail() || !fileDecomp.is_open()) {
			cerr << "Write failed: " << strerror(errno);
			return 1;
		}
	}



	fileComp.close();
	fileDecomp.close();

	return 0;
}
