/*
 * zobrist.h
 *
 *  Created on: 2012-12-5
 *      Author: Nguyễn Hoàng Trường
 */

#ifndef ZOBRIST_H_
#define ZOBRIST_H_

typedef unsigned char byte;
typedef unsigned int uint32;

struct RC4Struct {
	byte s[256];
	int x, y;

	inline void InitZero(void);
	byte Nextbyte(void) {
		byte uc;
		x = (x + 1) & 255;
		y = (y + s[x]) & 255;
		uc = s[x];
		s[x] = s[y];
		s[y] = uc;
		return s[(s[x] + s[y]) & 255];
	}
	uint32 NextLong(void) {
		byte uc0, uc1, uc2, uc3;
		uc0 = Nextbyte();
		uc1 = Nextbyte();
		uc2 = Nextbyte();
		uc3 = Nextbyte();
		return uc0 + (uc1 << 8) + (uc2 << 16) + (uc3 << 24);
	}
};

inline void RC4Struct::InitZero(void) {
	int i, j;
	byte uc;

	x = y = j = 0;
	for (i = 0; i < 256; i++) {
		s[i] = i;
	}
	for (i = 0; i < 256; i++) {
		j = (j + s[i]) & 255;
		uc = s[i];
		s[i] = s[j];
		s[j] = uc;
	}
}

struct ZobristStruct {
	uint32 dwKey, dwLock0, dwLock1;

	void InitZero(void) {
		dwKey = dwLock0 = dwLock1 = 0;
	}
	void InitRC4(RC4Struct &rc4) {
		dwKey = rc4.NextLong();
		dwLock0 = rc4.NextLong();
		dwLock1 = rc4.NextLong();
	}
	void Xor(const ZobristStruct &zobr) {
		dwKey ^= zobr.dwKey;
		dwLock0 ^= zobr.dwLock0;
		dwLock1 ^= zobr.dwLock1;
	}
	void Xor(const ZobristStruct &zobr1, const ZobristStruct &zobr2) {
		dwKey ^= zobr1.dwKey ^ zobr2.dwKey;
		dwLock0 ^= zobr1.dwLock0 ^ zobr2.dwLock0;
		dwLock1 ^= zobr1.dwLock1 ^ zobr2.dwLock1;
	}
};

typedef struct _Zobrist {
	ZobristStruct Player;
	ZobristStruct Table[14][256];
	inline _Zobrist();
} Zobrist;

inline _Zobrist::_Zobrist() {
	int i, j;
	RC4Struct rc4;

	rc4.InitZero();
	Player.InitRC4(rc4);
	for (i = 0; i < 14; i++) {
		for (j = 0; j < 256; j++) {
			Table[i][j].InitRC4(rc4);
		}
	}
}

#endif /* ZOBRIST_H_ */
