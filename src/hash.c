#include <string.h>

#include <util.h>
#include <hash.h>

static int getblock256(byte *ret, const byte *data, size_t start, size_t len);
static void sha256block(u32 hvals[8], const byte block[64]);

void sha256(byte ret[32], const byte *data, size_t len) {
	u32 hvals[8];
	hvals[0] = 0x6a09e667;
	hvals[1] = 0xbb67ae85;
	hvals[2] = 0x3c6ef372;
	hvals[3] = 0xa54ff53a;
	hvals[4] = 0x510e527f;
	hvals[5] = 0x9b05688c;
	hvals[6] = 0x1f83d9ab;
	hvals[7] = 0x5be0cd19;
	{
		size_t i;
		for (i = 0;; i += 64) {
			int shouldend;
			byte block[64];
			shouldend = getblock256(block, data, i, len);

			sha256block(hvals, block);

			if (shouldend)
				break;
		}
	}
	{
		int i;
		for (i = 0; i < 8; ++i) {
			int j;
			for (j = 3; j >= 0; --j) {
				ret[i * 4 + j] = hvals[i] & 0xff;
				hvals[i] >>= 8;
			}
		}
	}
}

static int getblock256(byte *ret, const byte *data, size_t start, size_t len) {
	size_t effectivelen;
	data += start;
	effectivelen = len - start;
	if (effectivelen >= 64) {
		memcpy(ret, data, 64);
		return 0;
	}
	memset(ret, 0, 64);
	if (effectivelen >= 0) {
		memcpy(ret, data, effectivelen);
		ret[effectivelen] = 0x80;
	}
	if (64 - effectivelen - 1 >= 8) {
		int i;
		len *= 8;
		/* We want bits, not bytes */
		for (i = 0; i < 8; ++i) {
			ret[64 - i - 1] = len & 0xff;
			len >>= 8;
		}
		return 1;
	}
	return 0;
}

static void sha256block(u32 hvals[8], const byte block[64]) {
	const u32 k[] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
		0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
		0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
		0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
	};
	u32 w[64];
	u32 work[8];
	int i;
	for (i = 0; i < 16; ++i) {
		int j;
		w[i] = 0;
		for (j = 0; j < 4; ++j) {
			w[i] <<= 8;
			w[i] |= block[i * 4 + j];
		}
		/* I hate endianness */
	}
	for (i = 16; i < 64; ++i) {
		u32 s0, s1;
		s0 = RTR(w[i - 15], 7) ^ RTR(w[i - 15], 18) ^ (w[i - 15] >> 3);
		s1 = RTR(w[i - 2], 17) ^ RTR(w[i - 2], 19) ^ (w[i - 2] >> 10);
		w[i] = (w[i - 16] + s0 + w[i - 7] + s1) & 0xffffffff;
	}
	memcpy(work, hvals, LEN(work));
	for (i = 0; i < 64; ++i) {
		u32 s1, ch, tmp1, s0, maj, tmp2;
		s1 = RTR(work[4], 6) ^ RTR(work[4], 11) ^ RTR(work[4], 25);
		ch = (work[4] & work[5]) ^ ((~work[4]) & work[6]);
		tmp1 = (work[7] + s1 + ch + k[i] + w[i]);
		s0 = RTR(work[0], 2) ^ RTR(work[0], 13) ^ RTR(work[0], 22);
		maj = (work[0] & work[1]) ^
			(work[0] & work[2]) ^
			(work[1] & work[2]);
		tmp2 = (s0 + maj);
		work[7] = work[6];
		work[6] = work[5];
		work[5] = work[4];
		work[4] = (work[3] + tmp1) & 0xffffffff;
		work[3] = work[2];
		work[2] = work[1];
		work[1] = work[0];
		work[0] = (tmp1 + tmp2) & 0xffffffff;
	}
	for (i = 0; i < 8; ++i)
		hvals[i] = (hvals[i] + work[i]) & 0xffffffff;
}
