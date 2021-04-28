/* compile using:
g++ -Wall -g -o testfastlz testfastlz.cc FastLZ/fastlz.c
*/

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <stdint.h>
//#include "fastlz.h"

#define size 1024
//extern int errno;
//#define O_BINARY 0

int main(int argc, char *argv[]) {
	int level = 1;
	char buffer[size] = {'\0'};
	char buffer2[size*2] = {'\0'};
	char intBuffer[5] = "0000";
	int newChars, result, result2;

	if (argc != 2) {
		printf("Usage: testfastlz <1 for compress, 0 for decompress>\n");
		return 1;
	}
	bool compress = argv[1][0] - 48;

	const char* fileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/Canterbury/alice29.txt";
	const char* compFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyComp/alice29.txt";
	const char* decompFileName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyDecomp/alice29.txt";
	const char* blocksizeName = "C:/Users/Steffens/Desktop/Map/School/CS_Master_3de_jaar/CanterburyComp/alice29.txt.blocksizes";

	int fd;
	int fdComp;
	int fdBlocksize;
	int fdDecomp;

	// Compress
	if (compress) {
		fd = open(fileName, O_RDONLY | O_BINARY);
		fdBlocksize = open(blocksizeName, O_WRONLY | O_BINARY);
		fdComp = open(compFileName, O_WRONLY | O_BINARY);

		ftruncate(fdBlocksize, 0);	// Clear file before writing to it.
		ftruncate(fdComp, 0);		// Clear file before writing to it.
		while(true) {

			newChars = 0;
			while (newChars < size) {
				result = read(fd, buffer+newChars, size-newChars);
				if (result <= 0)
					break;
				newChars += result;
			}

			if (newChars == 0) {
				// No new characters, EOF.
				break;
			}

			newChars = fastlz_compress_level(level, (const uint8_t*)buffer, newChars, (uint8_t*)buffer2);

			result2 = 0;
			int temp = sprintf(intBuffer, /*sizeof(int), */"%i", newChars);
			if (temp < 0 || temp >= sizeof(int)) {
				fprintf(stderr, "Int to String conversion failed: %s\n", strerror(errno));
				return 1;
			}

			temp = 0;
			for (int i = 0; i < sizeof(intBuffer)-1; i++) {
				if (intBuffer[i] != '\0')
					temp++;
				else
					break;
			}
			temp++;

			while (result2 < temp) {
				result = write(fdBlocksize, intBuffer+result2, temp-result2);
				if (result == -1) {
					fprintf(stderr, "Write failed: %s\n", strerror(errno));
					return 1;
				}
				result2 += result;
			}

			result2 = 0;
			while (newChars > 0) {
				result = write(fdComp, buffer2+result2, newChars);
				if (result == -1) {
					fprintf(stderr, "Write failed: %s\n", strerror(errno));
					return 1;
				}
				newChars -= result;
				result2 += result;
			}
		}

		close(fd);
		close(fdBlocksize);
		close(fdComp);
	}
	// Decompress
	else {
		fdComp = open(compFileName, O_RDONLY | O_BINARY);
		fdBlocksize = open(blocksizeName, O_RDONLY | O_BINARY);
		fdDecomp = open(decompFileName, O_WRONLY | O_BINARY);
		ftruncate(fdDecomp, 0);

		while(true) {

			int blockSizeIterator = -1;
			do {
				blockSizeIterator++;
				if (blockSizeIterator > 4) {
					printf("block has a size exceeding four digits\n");
					return 1;
				}
				result = read(fdBlocksize, intBuffer+blockSizeIterator, 1);
				if (result <= 0)
					break;
			} while (intBuffer[blockSizeIterator] != '\0');
			int blockSize = atoi(intBuffer);

			newChars = 0;
			while (newChars < blockSize) {
				result = read(fdComp, buffer+newChars, blockSize-newChars);
				if (result <= 0)
					break;
				newChars += result;
			}

			if (newChars == 0) {
				// No new characters, EOF.
				break;
			}

			newChars = fastlz_decompress((const uint8_t*)buffer, newChars, (uint8_t*)buffer2, size*2);

			if (!newChars) {
				fprintf(stderr, "Decompression failed: %s\n", strerror(errno));
				return 1;
			}

			result2 = 0;
			while (newChars > 0) {
				result = write(fdDecomp, buffer2+result2, newChars);
				if (result == -1) {
					fprintf(stderr, "Write failed: %s\n", strerror(errno));
					return 1;
				}
				newChars -= result;
				result2 += result;
			}
		}

		close(fdComp);
		close(fdDecomp);
	}
	return 0;
}
