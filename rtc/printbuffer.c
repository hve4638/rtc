#include <stdio.h>
#include "printbuffer.h"

void printbuffer(void* buffer, int length, int interval) {
	const unsigned char* buf = (const unsigned char*)buffer;

	printf("        \t");
	for (size_t i = 0; i < interval; i++) {
		printf("%2X ", (int)i);
	}
	printf("\n");

	for (size_t pos = 0; pos < length; pos += interval) {
		printf("%08X\t", (int)pos);

		for (size_t i = 0; i < interval; i++) {
			if (pos + i < length) printf("%02X ", buf[pos + i]);
			else printf("   ");
		}
		printf("\t");
		for (size_t i = 0; i < interval; i++) {
			if (pos + i >= length) break;
			size_t ch = buf[pos + i];

			if (ch >= 0x21 && ch <= 0x7E) { // printable char
				printf("%c", (int)ch);
			}
			else {
				printf(".");
			}
		}
		printf("\n");
	}
}
