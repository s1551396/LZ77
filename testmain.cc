#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cerrno>
#include "LZ77.h"

#define size 1024

using namespace std;

void LZ77(LDPair &output, char buffer[size], int window, int len);
int LZ77Decomp(LDPair input, char buffer[size], int window, int len);

int main() {
    const char* fileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/Canterbury/alice29.txt";
    const char* compFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyComp/alice29.txt";
    const char* decompFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyDecomp/alice29.txt";
    int fd = open(fileName, O_RDONLY | O_BINARY);
    int fdComp = open(compFileName, O_WRONLY | O_BINARY);
    int fdDecomp;
    int i, j;
    uint8_t o, l;
    char s;
	char buffer[size] = {'\0'};
	char LDPairbuffer[3];
	int len = size;
	int newChars;
	const int window = 255;
	int window2 = 0;			// Used during compression when window isn't filled yet
	LDPair IOLDPair;

	if (fd == -1) {
		cerr << "Read failed: " << strerror(errno);
		return 1;
	}
	if (fdComp == -1) {
		cerr << "Write failed: " << strerror(errno);
		return 1;
	}


	// Compress
	ftruncate(fdComp, 0);		// Clear file before writing to it.

	newChars = 0;
	while (newChars < size) {
		i = read(fd, buffer+newChars, size-newChars);
		if (i <= 0)
			break;
		newChars += i;
	}
	len -= size-newChars;
	newChars = 0;

	while (len > window2) {

		if (len < 300) {
			j = 5;
		}

		LZ77(IOLDPair, buffer, window2, len);

		LDPairbuffer[0] = IOLDPair.offset;
		LDPairbuffer[1] = IOLDPair.length;
		LDPairbuffer[2] = IOLDPair.symbol;


		if(write(fdComp, LDPairbuffer, 3) == -1) {
			cerr << "Write failed: " << strerror(errno);
			return 1;
		}


		window2 += IOLDPair.length+1;
		if (window2 >= window) {
			newChars = window2 - window;
			window2 = window;
		}

		for (i = 0; i < len-newChars; i++) {
			buffer[i] = buffer[i+newChars];
		}

		while (newChars > 0) {
			i = read(fd, buffer+size-newChars, newChars);
			if	(i <= 0)
				break;
			newChars -= i;
		}
		len -= newChars;
		newChars = 0;
	}

	close(fd);
	close(fdComp);


	// Decompress
	fdComp = open(compFileName, O_RDONLY | O_BINARY);
	fdDecomp = open(decompFileName, O_WRONLY | O_BINARY);
	ftruncate(fdDecomp, 0);

	for (i = 0; i < size; i++) {
		buffer[i] = '\0';
	}

	bool finished = false;
	for (i = 0; i < 3; i++) {
		if (!read(fdComp, LDPairbuffer+i, 1)) {
			finished = true;
			break;
		}
	}
	while (!finished) {

		o = (uint8_t)LDPairbuffer[0];
		l = (uint8_t)LDPairbuffer[1];
		s = LDPairbuffer[2];

		IOLDPair.set(o, l, s);

		len = LZ77Decomp(IOLDPair, buffer, window, size);

		i = 1;
		j = 0;
		while (i > 0) {
			i = write(fdDecomp, buffer+window+j, len-j);
			j += i;
		}

		// Move last window characters to start of buffer
		j = len;
		for (i = 0; i < window; i++) {
			buffer[i] = buffer[j];
			j++;
		}

		// Clear buffer except for first window characters
		for (i = window; i < size; i++) {
			buffer[i] = '\0';
		}

		for (i = 0; i < 3; i++) {
			if (!read(fdComp, LDPairbuffer+i, 1)) {
				finished = true;
				break;
			}
		}
	}

	close(fdComp);
	close(fdDecomp);

	return 0;
}
