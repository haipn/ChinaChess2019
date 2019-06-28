/*
 * engine.cpp
 *
 *  Created on: 2012-11-22
 *      Author: Nguyễn Hoàng Trường
 */

#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <algorithm>

namespace chess {



extern const unsigned int book_length;
extern const unsigned char book[];


Zobrist Engine::zobrist;

// Xác định mảng bàn cờ
const char Engine::ccInBoard[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Xác định xem tốt có nằm trong 9 nước nhập cung
const char Engine::ccInFort[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Để xác định xem các bước để đáp ứng các mảng di chuyển cụ thể，
//1=Tướng，2=Sĩ，3=Tịnh
const char Engine::ccLegalSpan[512] = {
						 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 2, 1, 2, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0
};
// Các bước để xác định xem Mã có trái chân hay không
const signed char Engine::ccKnightPin[512] = {
							  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,-16,  0,-16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0, 16,  0, 16,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0
};

// Thiết lập bàn cờ ban đầu
const byte Engine::cucpcStartup[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 20, 19, 18, 17, 16, 17, 18, 19, 20,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0, 21,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,
  0,  0,  0, 22,  0, 22,  0, 22,  0, 22,  0, 22,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 14,  0, 14,  0, 14,  0, 14,  0, 14,  0,  0,  0,  0,
  0,  0,  0,  0, 13,  0,  0,  0,  0,  0, 13,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0, 12, 11, 10,  9,  8,  9, 10, 11, 12,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

// Vị trí của bảng giá trị các quân
const byte Engine::cucvlPiecePos[7][256] = {
	{ //  Tướng
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 11, 15, 11,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Sĩ
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0, 20,  0, 20,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Tượng
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 18,  0,  0,  0, 23,  0,  0,  0, 18,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0, 20,  0,  0,  0, 20,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Mã
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0, 90, 90, 90, 96, 90, 96, 90, 90, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 96,103, 97, 94, 97,103, 96, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 98, 99,103, 99,103, 99, 98, 92,  0,  0,  0,  0,
	0,  0,  0, 93,108,100,107,100,107,100,108, 93,  0,  0,  0,  0,
	0,  0,  0, 90,100, 99,103,104,103, 99,100, 90,  0,  0,  0,  0,
	0,  0,  0, 90, 98,101,102,103,102,101, 98, 90,  0,  0,  0,  0,
	0,  0,  0, 92, 94, 98, 95, 98, 95, 98, 94, 92,  0,  0,  0,  0,
	0,  0,  0, 93, 92, 94, 95, 92, 95, 94, 92, 93,  0,  0,  0,  0,
	0,  0,  0, 85, 90, 92, 93, 78, 93, 92, 90, 85,  0,  0,  0,  0,
	0,  0,  0, 88, 85, 90, 88, 90, 88, 90, 85, 88,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Xe
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,206,208,207,213,214,213,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,212,209,216,233,216,209,212,206,  0,  0,  0,  0,
	0,  0,  0,206,208,207,214,216,214,207,208,206,  0,  0,  0,  0,
	0,  0,  0,206,213,213,216,216,216,213,213,206,  0,  0,  0,  0,
	0,  0,  0,208,211,211,214,215,214,211,211,208,  0,  0,  0,  0,
	0,  0,  0,208,212,212,214,215,214,212,212,208,  0,  0,  0,  0,
	0,  0,  0,204,209,204,212,214,212,204,209,204,  0,  0,  0,  0,
	0,  0,  0,198,208,204,212,212,212,204,208,198,  0,  0,  0,  0,
	0,  0,  0,200,208,206,212,200,212,206,208,200,  0,  0,  0,  0,
	0,  0,  0,194,206,204,212,200,212,204,206,194,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Pháo
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,100,100, 96, 91, 90, 91, 96,100,100,  0,  0,  0,  0,
	0,  0,  0, 98, 98, 96, 92, 89, 92, 96, 98, 98,  0,  0,  0,  0,
	0,  0,  0, 97, 97, 96, 91, 92, 91, 96, 97, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 99, 99, 98,100, 98, 99, 99, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96,100, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 95, 96, 99, 96,100, 96, 99, 96, 95,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 96, 96, 96, 96, 96, 96, 96,  0,  0,  0,  0,
	0,  0,  0, 97, 96,100, 99,101, 99,100, 96, 97,  0,  0,  0,  0,
	0,  0,  0, 96, 97, 98, 98, 98, 98, 98, 97, 96,  0,  0,  0,  0,
	0,  0,  0, 96, 96, 97, 99, 99, 99, 97, 96, 96,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}, { // Tốt
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  9,  9,  9, 11, 13, 11,  9,  9,  9,  0,  0,  0,  0,
	0,  0,  0, 19, 24, 34, 42, 44, 42, 34, 24, 19,  0,  0,  0,  0,
	0,  0,  0, 19, 24, 32, 37, 37, 37, 32, 24, 19,  0,  0,  0,  0,
	0,  0,  0, 19, 23, 27, 29, 30, 29, 27, 23, 19,  0,  0,  0,  0,
	0,  0,  0, 14, 18, 20, 27, 29, 27, 20, 18, 14,  0,  0,  0,  0,
	0,  0,  0,  7,  0, 13,  0, 16,  0, 13,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  7,  0,  7,  0, 15,  0,  7,  0,  7,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	}
};

// Nước đi của Tướng
const signed char Engine::ccKingDelta[4] = {-16, -1, 1, 16};
// Nước đi của Sĩ
const signed char Engine::ccAdvisorDelta[4] = {-17, -15, 15, 17};
// Nước đi của Mã，Các nước cản
const signed char Engine::ccKnightDelta[4][2] =
	{{-33, -31}, {-18, 14}, {-14, 18}, {31, 33}};
// Kiểm tra nước đi của mã
const signed char Engine::ccKnightCheckDelta[4][2] =
	{{-33, -18}, {-31, -14}, {14, 31}, {18, 33}};

////////////////////////////////////////////////////////////////////////


// Tạo các nước đi，Nếu "bCapture"cho"true"Các nước ăn được
int Engine::generateMoves(int *mvs, bool bCapture) const {
	ENTER("generateMoves");
	int i, j, nGenMoves, nDelta, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcSrc, pcDst;
	// Tạo ra các bước đi，Cần đi qua các bước sau：
	nGenMoves = 0;
	pcSelfSide = SIDE_TAG(player);
	pcOppSide = OPP_SIDE_TAG(player);
	for (sqSrc = 0; sqSrc < 256; sqSrc++) {

		// 1. Để tìm một con Tốt：
		pcSrc = board[sqSrc];
		if ((pcSrc & pcSelfSide) == 0) {
			continue;
		}

		// 2. Xác định theo sự di chuyển
		switch (pcSrc - pcSelfSide) {
		case PIECE_KING:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = board[sqDst];
				if (bCapture ?
						(pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("KING move out of ranges: %d, ", mvs[nGenMoves]);
						exit(-1);
					}
#endif
					nGenMoves++;
				}
			}
			break;
		case PIECE_ADVISOR:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!IN_FORT(sqDst)) {
					continue;
				}
				pcDst = board[sqDst];
				if (bCapture ?
						(pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("ADVISOR move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
					nGenMoves++;
				}
			}
			break;
		case PIECE_BISHOP:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccAdvisorDelta[i];
				if (!(IN_BOARD(sqDst) && HOME_HALF(sqDst, player)
						&& board[sqDst] == 0)) {
					continue;
				}
				LOG1("BISHOP debug sqDst: %d", sqDst);
				sqDst += ccAdvisorDelta[i];
#if defined(ANDROID_DEBUG)
				if (sqDst > 255) {
					LOG1("BISHOP i: %d", i);
					LOG1("BISHOP ccAdvisorDelta[i]: %d", ccAdvisorDelta[i]);
				}
#endif
				pcDst = board[sqDst];
				if (bCapture ?
						(pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("BISHOP sqSrc: %d", sqSrc);
						LOG1("BISHOP sqDst: %d", sqDst);
						LOG1("BISHOP move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
					nGenMoves++;
				}
			}
			break;
		case PIECE_KNIGHT:
			for (i = 0; i < 4; i++) {
				sqDst = sqSrc + ccKingDelta[i];
				if (board[sqDst] != 0) {
					continue;
				}
				for (j = 0; j < 2; j++) {
					sqDst = sqSrc + ccKnightDelta[i][j];
					if (!IN_BOARD(sqDst)) {
						continue;
					}
					pcDst = board[sqDst];
					if (bCapture ?
							(pcDst & pcOppSide) != 0 :
							(pcDst & pcSelfSide) == 0) {
						mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("KNIGHT move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
						nGenMoves++;
					}
				}
			}
			break;
		case PIECE_ROOK:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = board[sqDst];
					if (pcDst == 0) {
						if (!bCapture) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("ROOK move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
							nGenMoves++;
						}
					} else {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("ROOK move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_CANNON:
			for (i = 0; i < 4; i++) {
				nDelta = ccKingDelta[i];
				sqDst = sqSrc + nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = board[sqDst];
					if (pcDst == 0) {
						if (!bCapture) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("CANNON move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
							nGenMoves++;
						}
					} else {
						break;
					}
					sqDst += nDelta;
				}
				sqDst += nDelta;
				while (IN_BOARD(sqDst)) {
					pcDst = board[sqDst];
					if (pcDst != 0) {
						if ((pcDst & pcOppSide) != 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("CANNON move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
							nGenMoves++;
						}
						break;
					}
					sqDst += nDelta;
				}
			}
			break;
		case PIECE_PAWN:
			sqDst = SQUARE_FORWARD(sqSrc, player);
			if (IN_BOARD(sqDst)) {
				pcDst = board[sqDst];
				if (bCapture ?
						(pcDst & pcOppSide) != 0 : (pcDst & pcSelfSide) == 0) {
					mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("PAWN move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
					nGenMoves++;
				}
			}
			if (AWAY_HALF(sqSrc, player)) {
				for (nDelta = -1; nDelta <= 1; nDelta += 2) {
					sqDst = sqSrc + nDelta;
					if (IN_BOARD(sqDst)) {
						pcDst = board[sqDst];
						if (bCapture ?
								(pcDst & pcOppSide) != 0 :
								(pcDst & pcSelfSide) == 0) {
							mvs[nGenMoves] = MOVE(sqSrc, sqDst);
#if defined(ANDROID_DEBUG)
					if (mvs[nGenMoves] > 65535) {
						LOG1("PAWN move value out of ranges: %d", mvs[nGenMoves]);
						exit(-1);
					}
#endif
							nGenMoves++;
						}
					}
				}
			}
			break;
		}
	}
	return nGenMoves;
}

// Nước đi hợp lý
bool Engine::testMove(int mv) const {
	int sqSrc, sqDst, sqPin;
	int pcSelfSide, pcSrc, pcDst, nDelta;
	// Đánh giá hợp lý qua các nước sau đây：

	// 1. Xác định khởi đầu
	sqSrc = SRC(mv);
	pcSrc = board[sqSrc];
	pcSelfSide = SIDE_TAG(player);
	if ((pcSrc & pcSelfSide) == 0) {
		return false;
	}

	// 2. Xác định đích
	sqDst = DST(mv);
	pcDst = board[sqDst];
	if ((pcDst & pcSelfSide) != 0) {
		return false;
	}

	// 3. Kiểm tra vị trí đầu và đích
	switch (pcSrc - pcSelfSide) {
	case PIECE_KING:
		return IN_FORT(sqDst) && KING_SPAN(sqSrc, sqDst);
	case PIECE_ADVISOR:
		return IN_FORT(sqDst) && ADVISOR_SPAN(sqSrc, sqDst);
	case PIECE_BISHOP:
		return SAME_HALF(sqSrc, sqDst) && BISHOP_SPAN(sqSrc, sqDst)
				&& board[BISHOP_PIN(sqSrc, sqDst)] == 0;
	case PIECE_KNIGHT:
		sqPin = KNIGHT_PIN(sqSrc, sqDst);
		return sqPin != sqSrc && board[sqPin] == 0;
	case PIECE_ROOK:
	case PIECE_CANNON:
		if (SAME_RANK(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -1 : 1);
		} else if (SAME_FILE(sqSrc, sqDst)) {
			nDelta = (sqDst < sqSrc ? -16 : 16);
		} else {
			return false;
		}
		sqPin = sqSrc + nDelta;
		while (sqPin != sqDst && board[sqPin] == 0) {
			sqPin += nDelta;
		}
		if (sqPin == sqDst) {
			return pcDst == 0 || pcSrc - pcSelfSide == PIECE_ROOK;
		} else if (pcDst != 0 && pcSrc - pcSelfSide == PIECE_CANNON) {
			sqPin += nDelta;
			while (sqPin != sqDst && board[sqPin] == 0) {
				sqPin += nDelta;
			}
			return sqPin == sqDst;
		} else {
			return false;
		}
	case PIECE_PAWN:
		if (AWAY_HALF(sqDst, player)
				&& (sqDst == sqSrc - 1 || sqDst == sqSrc + 1)) {
			return true;
		}
		return sqDst == SQUARE_FORWARD(sqSrc, player);
	default:
		return false;
	}
}

// Trường hợp chiếu tướng
bool Engine::checked() const {
	int i, j, sqSrc, sqDst;
	int pcSelfSide, pcOppSide, pcDst, nDelta;
	pcSelfSide = SIDE_TAG(player);
	pcOppSide = OPP_SIDE_TAG(player);
	// Xác định Tướng

	for (sqSrc = 0; sqSrc < 256; sqSrc++) {
		if (board[sqSrc] != pcSelfSide + PIECE_KING) {
			continue;
		}

		// 1. Xác định những quân Tốt
		if (board[SQUARE_FORWARD(sqSrc, player)] == pcOppSide + PIECE_PAWN) {
			return true;
		}
		for (nDelta = -1; nDelta <= 1; nDelta += 2) {
			if (board[sqSrc + nDelta] == pcOppSide + PIECE_PAWN) {
				return true;
			}
		}

		// 2. Xác định chân Mã
		for (i = 0; i < 4; i++) {
			if (board[sqSrc + ccAdvisorDelta[i]] != 0) {
				continue;
			}
			for (j = 0; j < 2; j++) {
				pcDst = board[sqSrc + ccKnightCheckDelta[i][j]];
				if (pcDst == pcOppSide + PIECE_KNIGHT) {
					return true;
				}
			}
		}

		// 3. Xác đinh Xe
		for (i = 0; i < 4; i++) {
			nDelta = ccKingDelta[i];
			sqDst = sqSrc + nDelta;
			while (IN_BOARD(sqDst)) {
				pcDst = board[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_ROOK
							|| pcDst == pcOppSide + PIECE_KING) {
						return true;
					}
					break;
				}
				sqDst += nDelta;
			}
			sqDst += nDelta;
			while (IN_BOARD(sqDst)) {
				int pcDst = board[sqDst];
				if (pcDst != 0) {
					if (pcDst == pcOppSide + PIECE_CANNON) {
						return true;
					}
					break;
				}
				sqDst += nDelta;
			}
		}
		return false;
	}
	return false;
}

// Xác định Đã Chết hay chưa
bool Engine::isMate(void) {
	int i, nGenMoveNum, pcCaptured;
	std::vector<int> mvs(MAX_GEN_MOVES);
#if defined(ANDROID_DEBUG)
	if (mvs.size() != (unsigned)MAX_GEN_MOVES) {
		LOG("Gen mvs did not allocate enough memory in isMate.");
		exit(-1);
	}
#endif
	nGenMoveNum = generateMoves(begin_ptr(mvs));
#if defined(ANDROID_DEBUG)
	if (nGenMoveNum > 127) {
		LOG1("nGenMoveNum out of ranges: %d in isMate", nGenMoveNum);
		exit(-1);
	}
#endif
	for (i = 0; i < nGenMoveNum; i++) {
		pcCaptured = putPiece(mvs[i]);
		if (!checked()) {
			undoPut(mvs[i], pcCaptured);
			return false;
		} else {
			undoPut(mvs[i], pcCaptured);
		}
	}
	return true;
}

// Phát hiện các thế cục trùng lặp
int Engine::getRepeatPos(int nRecur) const {
	bool bSelfSide, bPerpCheck, bOppPerpCheck;
	const MoveStruct *lpmvs;

	bSelfSide = false;
	bPerpCheck = bOppPerpCheck = true;
	lpmvs = mvsList + nMoveNum - 1;
	while (lpmvs->wmv != 0 && lpmvs->ucpcCaptured == 0) {
		if (bSelfSide) {
			bPerpCheck = bPerpCheck && lpmvs->ucbCheck;
			if (lpmvs->dwKey == zobr.dwKey) {
				nRecur--;
				if (nRecur == 0) {
					return 1 + (bPerpCheck ? 2 : 0) + (bOppPerpCheck ? 4 : 0);
				}
			}
		} else {
			bOppPerpCheck = bOppPerpCheck && lpmvs->ucbCheck;
		}
		bSelfSide = !bSelfSide;
		lpmvs--;
	}
	return 0;
}

// Phản chiếu lại thế cục
void Engine::mirror(Engine &posMirror) const {
	int sq, pc;
	posMirror.clear();
	for (sq = 0; sq < 256; sq++) {
		pc = board[sq];
		if (pc != 0) {
			posMirror.addPiece(MIRROR_SQUARE(sq), pc);
		}
	}
	if (player == 1) {
		posMirror.changeSide();
	}
	posMirror.clearHistory();
}

// Khởi tạo bàn cờ
void Engine::startup(int direction) {
	int sq, pc;
	clear();
	for (sq = 0; sq < 256; sq++) {
		pc = cucpcStartup[sq];
		if (pc != 0) {
			addPiece(sq, pc);
		}
	}
	clearHistory();
	history.clear();
	this->direction = direction;
}

/////////////////////////////////////////////////////////////////////////
// Chức năng tìm kiếm trong từ điển

static int compareBook(const void *lpbk1, const void *lpbk2) {
	return ((BookItem *) lpbk1)->dwLock - ((BookItem *) lpbk2)->dwLock;
}

// Mở đầu tìm kiếm
int Engine::searchBook() {
	ENTER("searchBook");
	int nBookSize = book_length / sizeof(BookItem);	// Kích thước của từ điển
	if (nBookSize > BOOK_SIZE) {
		nBookSize = BOOK_SIZE;
	}
	BookItem* bookTable = (BookItem*)book; 		   	// Bắt đầu từ điển
	int i, vl, nBookMoves, mv;
	std::vector<int> mvs(MAX_GEN_MOVES), vls(MAX_GEN_MOVES);
	bool bMirror;
	BookItem bkToSearch, *lpbk;

	// Quá trình tìm kiếm theo các bước sau

	// 1. Nếu từ điển không mở sẽ đóng lại ngay lập tức
	if (nBookSize == 0) {
		return 0;
	}
	// 2. Tìm kiếm xem thế cục hiện nay có không
	bMirror = false;
	bkToSearch.dwLock = zobr.dwLock1;
	lpbk = (BookItem *) bsearch(&bkToSearch, bookTable, nBookSize,
			sizeof(BookItem), compareBook);
	// 3. Nếu không tìm thấy thì tìm kiếm phản hồi lại thế cục hiện tại
	if (lpbk == 0) {
		bMirror = true;
		Engine* posMirror = new Engine();
		mirror(*posMirror);
		bkToSearch.dwLock = posMirror->zobr.dwLock1;
		lpbk = (BookItem *) bsearch(&bkToSearch, bookTable,
				nBookSize, sizeof(BookItem), compareBook);
		delete posMirror;
	}
	// 4. Nếu không có phản hồi sẽ trở về ngay lập tức
	if (lpbk == 0) {
		return 0;
	}
	// 5. Nếu được phát hiện sẽ bắt đầu từ một mục của từ điển
	while (lpbk >= bookTable && lpbk->dwLock == bkToSearch.dwLock) {
		lpbk--;
	}
	lpbk++;
	// 6. Điểm số văn bản"mvs"và"vls"mảng
	vl = nBookMoves = 0;
	while (lpbk < bookTable + nBookSize
			&& lpbk->dwLock == bkToSearch.dwLock) {
		mv = (bMirror ? MIRROR_MOVE(lpbk->wmv) : lpbk->wmv);
		if (testMove(mv)) {
			mvs[nBookMoves] = mv;
			vls[nBookMoves] = lpbk->wvl;
			vl += vls[nBookMoves];
			nBookMoves++;
			if (nBookMoves == MAX_GEN_MOVES) {
				break; // ngăn chặn"BOOK.DAT" Chứa dữ liệu bất bình thường
			}
		}
		lpbk++;
	}
	if (vl == 0) {
		return 0; // Ngăn chặn "BOOK.DAT"Chứa dữ liệu bất thường
	}
	// 7. Ngẫu nhiên lựa chọn theo ưu tiên
	vl = rand() % vl;
	for (i = 0; i < nBookMoves; i++) {
		vl -= vls[i];
		if (vl < 0) {
			break;
		}
	}
	return mvs[i];
}

// Trích xuất các mục thay thế
int Engine::probeHash(int vlAlpha, int vlBeta, int nDepth,
		int &mv) {
	bool bMate; // biến cờ Mate ： Nếu bạn，Không cần đáp ứng điều kiện sâu
	HashItem hsh;

	hsh = search.hashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.dwLock0 != zobr.dwLock0 || hsh.dwLock1 != zobr.dwLock1) {
		mv = 0;
		return -MATE_VALUE;
	}
	mv = hsh.wmv;
	bMate = false;
	if (hsh.svl > WIN_VALUE) {
		if (hsh.svl < BAN_VALUE) {
			// Có thể gây ra sự bất ổn của tìm kiếm，Thoát ngay lập tức，việc di chuyển tốt nhất có thể
			return -MATE_VALUE;
		}
		hsh.svl -= nDistance;
		bMate = true;
	} else if (hsh.svl < -WIN_VALUE) {
		if (hsh.svl > -BAN_VALUE) {
			return -MATE_VALUE; // Như trên
		}
		hsh.svl += nDistance;
		bMate = true;
	}
	if (hsh.ucDepth >= nDepth || bMate) {
		if (hsh.ucFlag == HASH_BETA) {
			return (hsh.svl >= vlBeta ? hsh.svl : -MATE_VALUE);
		} else if (hsh.ucFlag == HASH_ALPHA) {
			return (hsh.svl <= vlAlpha ? hsh.svl : -MATE_VALUE);
		}
		return hsh.svl;
	}
	return -MATE_VALUE;
}

// Tiết kiệm mục thay thế
void Engine::recordHash(int nFlag, int vl, int nDepth, int mv) {
	HashItem hsh;
	hsh = search.hashTable[zobr.dwKey & (HASH_SIZE - 1)];
	if (hsh.ucDepth > nDepth) {
		return;
	}
	hsh.ucFlag = nFlag;
	hsh.ucDepth = nDepth;
	if (vl > WIN_VALUE) {
		if (mv == 0 && vl <= BAN_VALUE) {
			return; // Tìm kiếm bất ổn, không có di chuyển tốt nhất, thoát ngay lập tức
		}
		hsh.svl = vl + nDistance;
	} else if (vl < -WIN_VALUE) {
		if (mv == 0 && vl >= -BAN_VALUE) {
			return; // Như trên
		}
		hsh.svl = vl - nDistance;
	} else {
		hsh.svl = vl;
	}
	hsh.wmv = mv;
	hsh.dwLock0 = zobr.dwLock0;
	hsh.dwLock1 = zobr.dwLock1;
	search.hashTable[zobr.dwKey & (HASH_SIZE - 1)] = hsh;
}


// MVV/LVA Giá trị của từng hạt giống
static byte cucMvvLva[24] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	5, 1, 1, 3, 4, 3, 2, 0,
	5, 1, 1, 3, 4, 3, 2, 0
};

// Nhấn  MVV/LVA Giá trị chức năng so sánh loại
class CompareMvvLva
{
public:
	inline CompareMvvLva(byte* board);
	inline bool operator()(uint32 mv1, uint32 mv2);
	// Yêu cầu MVV/LVA Giá trị
	inline int32 mvv_lva(uint32 mv);
private:
	byte* board;
};

CompareMvvLva::CompareMvvLva(byte* board)
	: board(board)
{
}

inline bool CompareMvvLva::operator()(uint32 mv1, uint32 mv2)
{
	return mvv_lva(mv1) > mvv_lva(mv2);
}

// Sự kết thúc của nước đi
inline uint32 to(uint32 mv)
{
	return mv >> 8;
}

// Điểm khởi đầu của nước đi
inline uint32 from(uint32 mv)
{
	return mv & 0xFF;
}

// Yêu cầu MVV/LVA Giá trị
inline int32 CompareMvvLva::mvv_lva(uint32 mv)
{
	return (cucMvvLva[board[to(mv)]] << 3) - cucMvvLva[board[from(mv)]];
}


// Lịch sử bảng so sánh loại
class CompareHistory
{
public:
	inline CompareHistory(int32* history_table);
	inline bool operator()(uint32 mv1, uint32 mv2);
private:
	int32* _history_table;
};
inline CompareHistory::CompareHistory(int32* history_table)
: _history_table(history_table)
{
}
inline bool CompareHistory::operator()(uint32 mv1, uint32 mv2)
{
#if defined(ANDROID_DEBUG)
	if (mv1 > 65535) {
		LOG1("CompareHistory::operator() mv1 of range: %d", mv1);
		exit(-1);
	}
	if (mv2 > 65535) {
		LOG1("CompareHistory::operator() mv2 out of range: %d", mv2);
		exit(-1);
	}

#endif
	return _history_table[mv1] > _history_table[mv2];
}

// Di chuyển theo giai đoạn
const int PHASE_HASH = 0;
const int PHASE_KILLER_1 = 1;
const int PHASE_KILLER_2 = 2;
const int PHASE_GEN_MOVES = 3;
const int PHASE_REST = 4;

// Di chuyển theo cấu trúc
typedef struct SortStruct {
	Engine& engine;
	Search& search;
	int mvHash, mvKiller1, mvKiller2; // Hoán vị và di chuyển nước ăn
	int nPhase, nIndex, nGenMoves;    // Giai đoạn hiện nay，Các bước đi，Các nước đi tổng cộng
	std::vector<int> mvs;           // Tất cả các nước

	// Khơi tạo, thiết lập di chuyển bảng hoán vị
	inline SortStruct(Engine& engine, Search& search, int mvHash);
	// Được di chuyển tiếp theo
	int next(void);
} SortStruct;

inline SortStruct::SortStruct(Engine& engine, Search& search, int mvHash) :
		engine(engine), search(search), nIndex(0), nGenMoves(0),
		mvs(MAX_GEN_MOVES) {
	this->mvHash = mvHash;
#if defined(ANDROID_DEBUG)
	if (engine.nDistance >= 64 || engine.nDistance < 0) {
		LOG("engine.nDistance out of range.");
		exit(-1);
	}
#endif
	mvKiller1 = search.mvKillers[engine.nDistance][0];
	mvKiller2 = search.mvKillers[engine.nDistance][1];
	nPhase = PHASE_HASH;
}

//Được di chuyển tiếp theo
int SortStruct::next(void) {
	int mv;
	CompareHistory compareHistory(search.nHistoryTable);

	// "nPhase"Một số giai đoạn：

	// 0. Phương pháp lấy bảng hoán vị；
	if (nPhase == PHASE_HASH) {
		nPhase = PHASE_KILLER_1;
		if (mvHash != 0) {
			return mvHash;
		}
	}

	// 1. Di chuyển ngay khi hoàn thành；
	if (nPhase == PHASE_KILLER_1) {
		nPhase = PHASE_KILLER_2;
		if (mvKiller1 != mvHash && mvKiller1 != 0
				&& engine.testMove(mvKiller1)) {
			return mvKiller1;
		}
	}

	// 2. Di chuyển ngay khi hoàn thành；
	if (nPhase == PHASE_KILLER_2) {
		nPhase = PHASE_GEN_MOVES;
		if (mvKiller2 != mvHash && mvKiller2 != 0
				&& engine.testMove(mvKiller2)) {
			return mvKiller2;
		}
	}

	// 3. Tạo tất cả di chuyển vào giai đoạn tiếp theo；
	if (nPhase == PHASE_GEN_MOVES) {
		nPhase = PHASE_REST;
		nGenMoves = engine.generateMoves(begin_ptr(mvs));
		LOG1("nGenMoves: %d SortStruct::next", nGenMoves);
		std::sort(begin_ptr(mvs), begin_ptr(mvs) + nGenMoves, compareHistory);
		nIndex = 0;
	}

	// 4. Bảng lịch sử truyền các nước đi；
	if (nPhase == PHASE_REST) {
		while (nIndex < nGenMoves) {
			mv = mvs[nIndex];
			nIndex++;
			if (mv != mvHash && mv != mvKiller1 && mv != mvKiller2) {
				return mv;
			}
		}
	}
	// 5. Không có bươc đi。
	return 0;
}


// Trạng thái tĩnh (Quiescence)Quá trình tìm kiếm
int Engine::searchQuiescence(int vlAlpha, int vlBeta) {
	ENTER("searchQuiescence");
	int i, nGenMoves;
	int vl, vlBest;
	std::vector<int> mvs(MAX_GEN_MOVES);

	CompareHistory compareHistory(search.nHistoryTable);
	CompareMvvLva compareMvvLva(board);

	// Tìm kiếm tĩnh chia các giai đoạn sau

	// 1. Kiểm tra tình trạng trùng lặp
	vl = getRepeatPos();
	if (vl != 0) {
		LOG("searchQuiescence done!");
		return getRepeatValue(vl);
	}

	// 2. Đạt đến giới hạn trở lại chiều sâu cho đánh giá
	if (nDistance == LIMIT_DEPTH) {
		LOG("searchQuiescence done!");
		return evaluate();
	}

	// 3. Khởi tạo các giá trị tốt nhất
	vlBest = -MATE_VALUE; // Vì vậy, có thể biết đến động thái không dùng đến

	if (inCheck()) {
		// 4. Nếu các quân Tướng
#if defined(ANDROID_DEBUG)
	if (mvs.size() != (unsigned)MAX_GEN_MOVES) {
		LOG("Gen mvs did not allocate enough memory in searchQuiescence.");
		exit(-1);
	}
#endif
		nGenMoves = generateMoves(begin_ptr(mvs));
#if defined(ANDROID_DEBUG)
	if (nGenMoves > 127) {
		LOG1("nGenMoveNum out of ranges: %d  in searchQuiescence.", nGenMoves);
		exit(-1);
	}
#endif
		ENTER("SORT");
		LOG1("nGenMoves: %d searchQuiescence 1", nGenMoves);
		std::sort(begin_ptr(mvs), begin_ptr(mvs) + nGenMoves, compareHistory);
	} else {
		// 5. Đánh giá tình hình đầu tiên
		vl = evaluate();
		if (vl > vlBest) {
			vlBest = vl;
			if (vl >= vlBeta) {
				return vl;
			}
			if (vl > vlAlpha) {
				vlAlpha = vl;
			}
		}
		// 6. Đánh giá tình hình tạo ra ăn quân
#if defined(ANDROID_DEBUG)
	if (mvs.size() != (unsigned)MAX_GEN_MOVES) {
		LOG("Gen mvs did not allocate enough memory in searchQuiescence.");
		exit(-1);
	}
#endif
		nGenMoves = generateMoves(begin_ptr(mvs), true);
#if defined(ANDROID_DEBUG)
	if (nGenMoves > 127) {
		LOG1("nGenMoveNum out of ranges: %d  in searchQuiescence.", nGenMoves);
		exit(-1);
	}
#endif
		ENTER("SORT");
		LOG1("nGenMoves: %d searchQuiescence 2", nGenMoves);
		std::sort(begin_ptr(mvs), begin_ptr(mvs) + nGenMoves, compareMvvLva);
	}

	// 7. Đi từng quân một
	for (i = 0; i < nGenMoves; i++) {
		if (move(mvs[i])) {
			vl = -searchQuiescence(-vlBeta, -vlAlpha);
			undoMove();

			// 8. Tiến hành Alpha-Beta Kích thước ngắn
			if (vl > vlBest) {    // Tìm thấy những giá trị tốt nhất
				vlBest = vl;        // "vlBest"Để trả về những giá trị tốt nhất phải vượt qua ranh giới Al - Bt
				if (vl >= vlBeta) { // Tìm thấy một di chuyển
					return vl;        // Beta cắt
				}
				if (vl > vlAlpha) { // Tìm thấy một di chuyển
					vlAlpha = vl;     // Biến giới Al - Bt hẹp
				}
			}
		}
	}
	LOG("searchQuiescence done!");
	// 9. Tất cả tìm kiếm kết thúc, trả lại giá trị tốt nhất
	return vlBest == -MATE_VALUE ? nDistance - MATE_VALUE : vlBest;
}

// Xử lý di chuyển tốt nhất
void Engine::setBestMove(int mv, int nDepth) {
	int *lpmvKillers;
#if defined(ANDROID_DEBUG)
	if (mv > 65535 || mv < 0) {
		LOG("setBestMove mv out of range");
		exit(mv);
	}
#endif
	search.nHistoryTable[mv] += nDepth * nDepth;
	lpmvKillers = search.mvKillers[nDistance];
	if (lpmvKillers[0] != mv) {
		lpmvKillers[1] = lpmvKillers[0];
		lpmvKillers[0] = mv;
	}
}

// Quá trình tìm kiếm ngoài giới hạn Alpha - Beta
int Engine::searchFull(int vlAlpha, int vlBeta, int nDepth,
		bool bNoNull) {
	ENTER("searchFull");
	int nHashFlag, vl, vlBest;
	int mv, mvBest, mvHash, nNewDepth;

	// Các giai đoạn tìm kiếm

	// 1. Đặt các đường ngang gọi là tìm kiếm tĩnh
	if (nDepth <= 0) {
		LOG("searchFull done!");
		return searchQuiescence(vlAlpha, vlBeta);
	}

	// 1-1. Kiểm tra tình hình trùng lặp
	vl = getRepeatPos();
	if (vl != 0) {
		LOG("searchFull done!");
		return getRepeatValue(vl);
	}

	// 1-2. Đạt đến giới hạn chiều sâu cho thế cục
	if (nDistance == LIMIT_DEPTH) {
		LOG("searchFull done!");
		return evaluate();
	}

	// 1-3. Cố gắng để ghi đè lên cắt bảng và di chuyển bảng hoán vị
	vl = probeHash(vlAlpha, vlBeta, nDepth, mvHash);
	if (vl > -MATE_VALUE) {
		LOG("searchFull done!");
		return vl;
	}
	LOG("searchFull probeHash passed!");

	// 1-4. Giá trị Beta ở nút gốc là MATE_VALUE
	if (!bNoNull && !inCheck() && allowNullMove()) {
		nullMove();
		vl = -searchFull(-vlBeta, 1 - vlBeta, nDepth - NULL_DEPTH - 1,
				true);
		undoNullMove();
		if (vl >= vlBeta) {
			return vl;
		}
	}
	LOG("searchFull allowNullMove passed!");

	// 2. Khởi tạo các giá tri tốt nhất và di chuyển tốt nhất
	nHashFlag = HASH_ALPHA;
	vlBest = -MATE_VALUE; // Vì vậy, biết đước các động thái không dùng đến
	mvBest = 0;           // Vì vậy, tìm kiếm các bản Beta để tiết kiệm bảng lịch sử

	// 3. Cấu trúc theo
	SortStruct sort(*this, search, mvHash);
	LOG("searchFull sort passed!");

	// 4. Đi từng động thái và đệ quy
	while ((mv = sort.next()) != 0) {
		if (move(mv)) {
			// Mở rộng chung
			nNewDepth = inCheck() ? nDepth : nDepth - 1;
			// PVS
			if (vlBest == -MATE_VALUE) {
				vl = -searchFull(-vlBeta, -vlAlpha, nNewDepth);
			} else {
				vl = -searchFull(-vlAlpha - 1, -vlAlpha, nNewDepth);
				if (vl > vlAlpha && vl < vlBeta) {
					vl = -searchFull(-vlBeta, -vlAlpha, nNewDepth);
				}
			}
			undoMove();

			// 5. Alpha-Beta Xác định kích thước và cắt ngắn
			if (vl > vlBest) {    // Tìm thấy những giá trị tốt nhất
				vlBest = vl;        // "vlBest"Để trả lại giá trị tốt nhất vượt quá ranh giới Alpha - Beta
				if (vl >= vlBeta) { // Tìm thấy một di chuyển
					nHashFlag = HASH_BETA;
					mvBest = mv;      // Beta Có luật được lưu vào bảng
					break;            // Beta Cắt
				}
				if (vl > vlAlpha) { // Tìm thấy một di chuyển
					nHashFlag = HASH_PV;
					mvBest = mv;      // PV Có luật được lưu vào lịch sử
					vlAlpha = vl;     // biên giới Alpha-Beta hẹp
				}
			}
		}
	}
	LOG("searchFull all move tested!");


	// 5. Tim kiem tot nhat de di vao bang lich su va trả về giá trị tốt nhất
	if (vlBest == -MATE_VALUE) {
		// Đánh giá qua tần suất di chuyển để ăn quân
		LOG("searchFull done!");
		return nDistance - MATE_VALUE;
	}
	// Bản ghi vào bảng hoán vị
	recordHash(nHashFlag, vlBest, nDepth, mvBest);
	if (mvBest != 0) {
		// Nếu không di
		setBestMove(mvBest, nDepth);
	}
	LOG("searchFull done!");
	return vlBest;
}

// Nút gốc Alpha-Beta Quá trình tìm kiếm
int Engine::searchRoot(int nDepth) {
	ENTER("searchRoot");
	int vl, vlBest, mv, nNewDepth;
	vlBest = -MATE_VALUE;
	SortStruct sort(*this, search, search.mvResult);
	while ((mv = sort.next()) != 0) {
		if (move(mv)) {
			nNewDepth = inCheck() ? nDepth : nDepth - 1;
			if (vlBest == -MATE_VALUE) {
				vl = -searchFull(-MATE_VALUE, MATE_VALUE, nNewDepth,
						true);
			} else {
				vl = -searchFull(-vlBest - 1, -vlBest, nNewDepth);
				if (vl > vlBest) {
					vl = -searchFull(-MATE_VALUE, -vlBest, nNewDepth,
							true);
				}
			}
			undoMove();
			if (vl > vlBest) {
				vlBest = vl;
				search.mvResult = mv;
				if (vlBest > -WIN_VALUE && vlBest < WIN_VALUE) {
					vlBest += (rand() & RANDOM_MASK) - (rand() & RANDOM_MASK);
				}
			}
		}
	}
	recordHash(HASH_PV, vlBest, nDepth, search.mvResult);
	setBestMove(search.mvResult, nDepth);
	return vlBest;
}

// Lặp lại trong quá trình tìm kiếm
int Engine::searchMain(float seconds) {

	ENTER("searchMain");
	int i, t, vl, nGenMoves;
	std::vector<int> mvs(MAX_GEN_MOVES);

	// Khởi tạo một tìm kiếm
	search.init();

	t = clock();       // Khởi tạo thời gian
	nDistance = 0; // Số lượng các bước ban đầu

	// Mở từ điển tìm kiếm
	search.mvResult = searchBook();

	if (search.mvResult != 0) {
		move(search.mvResult);
		if (getRepeatPos(3) == 0) {
			undoMove();
			return search.mvResult;
		}
		undoMove();
	}

	// Kiểm tra chỉ số di chuyển
	vl = 0;
#if defined(ANDROID_DEBUG)
	if (mvs.size() != (unsigned)MAX_GEN_MOVES) {
		LOG("Gen mvs did not allocate enough memory in searchMain.");
		exit(-1);
	}
#endif
	nGenMoves = generateMoves(begin_ptr(mvs));
#if defined(ANDROID_DEBUG)
	if (nGenMoves > 127) {
		LOG1("nGenMoveNum out of ranges: %d  in searchMain.", nGenMoves);
		exit(-1);
	}
#endif
	for (i = 0; i < nGenMoves; i++) {
		if (move(mvs[i])) {
			undoMove();
			search.mvResult = mvs[i];
			vl++;
		}
	}
	if (vl == 1) {
		return search.mvResult;
	}

	// Lặp lại
	for (i = 1; i <= LIMIT_DEPTH; i++) {
		vl = searchRoot(i);
		// Chấm dứt việc tìm kiếm
		if (vl > WIN_VALUE || vl < -WIN_VALUE) {
			break;
		}

		if (clock() - t > (CLOCKS_PER_SEC * seconds)) {
			break;
		}
	}

	return search.mvResult;
}

// Chơi cờ
bool Engine::move(int fromX, int fromY, int toX, int toY) {
	ENTER("move");
	int mv;
	if (direction == 0) {
		int from = COORD_XY(fromX + 3, fromY + 3);
		int to = COORD_XY(toX + 3, toY + 3);
		mv = MOVE(from, to);
	} else {
		int from = COORD_XY(fromX + 3, RANK_FLIP(fromY + 3));
		int to = COORD_XY(toX + 3, RANK_FLIP(toY + 3));
		mv = MOVE(from, to);
	}
	if (!testMove(mv))
		return false;

	if (isMate())
		return false;

	bool ret = move(mv);
	if (ret) {
		history.push_back(mvsList[nMoveNum - 1]);
		if (captured()) {
			clearHistory();
		}
	}
	return ret;
}

// Lùi lại
void Engine::undo() {
	if (nMoveNum > 1) {
		undoMove();
		history.pop_back();
	} else if (history.size() > 0) {
		MoveStruct& mvinfo = history.back();
		changeSide();
		undoPut(mvinfo.wmv, mvinfo.ucpcCaptured);
		history.pop_back();
	}
}

// Nhận được trạng thái của ván cờ
int Engine::getState(byte* board) {
	if (board != 0) {
		for (int y = RANK_TOP; y <= RANK_BOTTOM; y++) {
			for (int x = FILE_LEFT; x <= FILE_RIGHT; x ++) {
				if (direction == 0)
					*board++ = this->board[COORD_XY(x, y)];
				else
					*board++ = this->board[COORD_XY(x, RANK_FLIP(y))];
			}
		}
	}
	if (isMate()) {
		if (player == 0)
			return BLACK_WIN;
		else
			return RED_WIN;
	}
	int rep = getRepeatPos(3);
	if (rep > 0) {
		rep = getRepeatValue(rep);
		if (rep > WIN_VALUE) {
			if (player == 0)
				return BLACK_WIN | REPEATED;
			else
				return RED_WIN | REPEATED;
		} else if (rep < -WIN_VALUE) {
			if (player == 0)
				return RED_WIN | REPEATED;
			else
				return BLACK_WIN | REPEATED;
		} else {
			return DRAW | REPEATED;
		}
	} else if (nMoveNum > 100) {
		return DRAW | EXCEEDED_100;
	} else {
		// Nếu không có người chiến thắng
		if (inCheck())
			return NORMAL_CHECKED;
		else if (captured())
			return NORMAL_CAPTURED;
		else
			return NORMAL;
	}
}

// Tìm giải pháp
bool Engine::findSolution(float searchSeconds, int& fromX,
		int& fromY, int& toX, int& toY) {
	ENTER("findSolution");
	if (isMate()) {
		return false;
	}
	int mv = searchMain(searchSeconds);
	if (mv == 0) {
		return false;
	}
	int from = SRC(mv);
	int to = DST(mv);
	fromX = FILE_X(from) - 3;
	if (direction == 0)
		fromY = RANK_Y(from) - 3;
	else
		fromY = RANK_FLIP(RANK_Y(from)) - 3;
	toX = FILE_X(to) - 3;
	if (direction == 0)
		toY = RANK_Y(to) - 3;
	else
		toY = RANK_FLIP(RANK_Y(to)) - 3;
	return true;
}


}
