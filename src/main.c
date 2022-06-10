#include <stdio.h>
#include <string.h>

#include <aes.h>
#include <hash.h>

int main() {
	byte key[32];
	byte text[16];
	byte crypt[16];
	int i;

	key[0] = 0xc4;
	key[1] = 0x7b;
	key[2] = 0x02;
	key[3] = 0x94;
	key[4] = 0xdb;
	key[5] = 0xbb;
	key[6] = 0xee;
	key[7] = 0x0f;
	key[8] = 0xec;
	key[9] = 0x47;
	key[10] = 0x57;
	key[11] = 0xf2;
	key[12] = 0x2f;
	key[13] = 0xfe;
	key[14] = 0xee;
	key[15] = 0x35;
	key[16] = 0x87;
	key[17] = 0xca;
	key[18] = 0x47;
	key[19] = 0x30;
	key[20] = 0xc3;
	key[21] = 0xd3;
	key[22] = 0x3b;
	key[23] = 0x69;
	key[24] = 0x1d;
	key[25] = 0xf3;
	key[26] = 0x8b;
	key[27] = 0xab;
	key[28] = 0x07;
	key[29] = 0x6b;
	key[30] = 0xc5;
	key[31] = 0x58;

	memset(text, 0, sizeof text);

	for (i = 0; i < 32; ++i)
		printf("%02x", key[i]);
	putchar('\n');
	aes256encrypt(crypt, text, key);
	for (i = 0; i < 16; ++i)
		printf("%02x", crypt[i]);
	putchar('\n');
	for (i = 0; i < 32; ++i)
		printf("%02x", key[i]);
	putchar('\n');
	aes256decrypt(text, crypt, key);
	for (i = 0; i < 16; ++i)
		printf("%02x", text[i]);
	putchar('\n');

	return 0;
}
