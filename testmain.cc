#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
    const char* fileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/Canterbury/alice29.txt";
    const char* compFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyComp/alice29.txt";
    const char* decompFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyDecomp/alice29.txt";
    //ifstream file(fileName, ios::in | ios::binary);
    int fd = open(fileName, O_RDONLY | O_BINARY);
    int fdComp = open(compFileName, O_WRONLY | O_BINARY);
    int fdDecomp;
    //fstream fileComp(compFileName, fstream::out | fstream::in| ios::binary);
    int j;
    uint8_t o, l;
    char s;
	char buffer[size] = {'\0'};
	char buffer2[3];
	char S3[size];
	int len = size;
	int newCharacters;
	const int window = 255;
	int window2 = 0;			// Used during compression when window isn't filled yet
	LDPair output;
	char output2[3];
	LDPair input;
	int temp;

	if (fd == -1) {
		cerr << "Read failed: " << strerror(errno);
		return 1;
	}
	if (fdComp == -1) {
		cerr << "Write failed: " << strerror(errno);
		return 1;
	}

	//if(!file || file.fail() || !file.is_open()) {
		//cerr << "Read failed: " << strerror(errno);
		//return 1;
	//}

	// Compress
	ftruncate(fdComp, 0);

	newCharacters = 0;
	while (newCharacters < size) {
		temp = read(fd, buffer+newCharacters, size-newCharacters);
		if (temp <= 0)
			break;
		newCharacters += temp;
	}
	len -= size-newCharacters;
	newCharacters = 0;

	while (len > window2) {

		if (len < 300) {
			j = 5;
		}

		LZ77(buffer, len, window2, output);

		output2[0] = output.offset;
		output2[1] = output.length;
		output2[2] = output.symbol;


		if(write(fdComp, output2, 3) == -1) {
			cerr << "Write failed: " << strerror(errno);
			return 1;
		}


		window2 += output.length+1;
		if (window2 >= window) {
			newCharacters = window2 - window;
			window2 = window;
		}

		for (int i = 0; i < len-newCharacters; i++) {
			buffer[i] = buffer[i+newCharacters];
		}

		while (newCharacters > 0) {
			temp = read(fd, buffer+size-newCharacters, newCharacters);
			if	(temp <= 0)
				break;
			newCharacters -= temp;
		}
		len -= newCharacters;
		newCharacters = 0;
	}

	close(fd);
	close(fdComp);

	//if(!fileComp || fileComp.fail() || !fileComp.is_open()) {
		//cerr << "Write failed: " << strerror(errno);
		//return 1;
	//}

	//file.close();
	//fileComp.close();



	// Decompress
	fdComp = open(compFileName, O_RDONLY | O_BINARY);
	fdDecomp = open(decompFileName, O_WRONLY | O_BINARY);
	ftruncate(fdDecomp, 0);

	//fileComp.seekg(0);
	//fileComp(compFileName, fstream::in | ios::binary);
	//fstream fileDecomp(decompFileName, fstream::out | ios::binary);

	for (int i = 0; i < size; i++) {
		buffer[i] = '\0';
	}


	bool finished = false;
	for (int i = 0; i < 3; i++) {
		if (!read(fdComp, buffer2+i, 1)) {
			finished = true;
			break;
		}
	}
	while (!finished) {

		o = (uint8_t)buffer2[0];
		l = (uint8_t)buffer2[1];
		s = buffer2[2];

		input.set(o, l, s);

		len = LZ77Decomp(input, buffer, window, size);

		temp = 1;
		j = 0;
		while (temp > 0) {
			temp = write(fdDecomp, buffer+window+j, len-j);
			j += temp;
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

		for (int i = 0; i < 3; i++) {
			if (!read(fdComp, buffer2+i, 1)) {
				finished = true;
				break;
			}
		}
	}

	close(fdComp);
	close(fdDecomp);

	return 0;
}
