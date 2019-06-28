/*
 * engine.h
 *
 *  Created on: 2012-12-2
 *      Author: Nguyễn Hoàng Trường
 */

#ifndef ENGINE_H_
#define ENGINE_H_

#if defined(ANDROID_DEBUG)
#	include <android/log.h>
#	define DEBUG_LOG
#	define DEBUG_TRACE
#	define DEBUG_STATE
#endif

#include <string.h>
#include <vector>
#include <stdlib.h>

#include "zobrist.h"

#ifdef DEBUG_LOG
#	define LOG(x) __android_log_write( \
		ANDROID_LOG_DEBUG, "ChineseChess", x);
#	define LOG1(f,x) __android_log_print( \
		ANDROID_LOG_DEBUG, "ChineseChess", f, x);
#else
#	define LOG(x)
#	define LOG1(f,x)
#endif

#ifdef DEBUG_TRACE
#	define ENTER(x) LOG(x);
#else
#	define ENTER(x)
#endif

namespace chess {

static const int LIMIT_DEPTH = 64;
static const int HASH_SIZE = 1 << 20;
static const int BOOK_SIZE = 16384;
static const int MAX_MOVES = 256;
static const int MAX_GEN_MOVES = 128;
static const int MATE_VALUE = 10000;

static const int BAN_VALUE = MATE_VALUE - 100;

static const int WIN_VALUE = MATE_VALUE - 200;
static const int DRAW_VALUE = 20;
static const int ADVANCED_VALUE = 3;
static const int RANDOM_MASK = 7;
static const int NULL_MARGIN = 400;
static const int NULL_DEPTH = 2;
static const int HASH_ALPHA = 1;      	// ALPHA
static const int HASH_BETA = 2;       	// BETA
static const int HASH_PV = 3;         	// PV

typedef unsigned char byte;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef short int16;
typedef int int32;

template <class T>
inline T* begin_ptr(std::vector<T>& v)
{return  v.empty() ? NULL : &v[0];}

template <class T>
inline const T* begin_ptr(const std::vector<T>& v)
{return  v.empty() ? NULL : &v[0];}

template <class T>
inline T* end_ptr(std::vector<T>& v)
{return v.empty() ? NULL : (begin_ptr(v) + v.size());}

template <class T>
inline const T* end_ptr(const std::vector<T>& v)
{return v.empty() ? NULL : (begin_ptr(v) + v.size());}


typedef struct MoveStruct {
	uint16 wmv;
	byte ucpcCaptured;
	byte ucbCheck;
	uint32 dwKey;
	inline void set(int mv, int pcCaptured, bool bCheck, uint32 dwKey) {
		wmv = mv;
		ucpcCaptured = pcCaptured;
		ucbCheck = bCheck;
		this->dwKey = dwKey;
	}
} MoveStruct;

typedef struct HashItem {
	byte ucDepth, ucFlag;
	short svl;
	uint16 wmv, wReserved;
	uint32 dwLock0, dwLock1;
} HashItem;

typedef struct BookItem {
	uint32 dwLock;
	uint16 wmv, wvl;
} BookItem;


typedef struct Search {
	int mvResult;
	int nHistoryTable[65536];
	int mvKillers[LIMIT_DEPTH][2];
	HashItem hashTable[HASH_SIZE];
	inline Search();
	inline void init();
} Search;

inline Search::Search() {
	init();
}
inline void Search::init() {
	mvResult = 0;
	memset(nHistoryTable, 0, 65536 * sizeof(int));
	memset(mvKillers, 0, LIMIT_DEPTH * 2 * sizeof(int));
	memset(hashTable, 0, HASH_SIZE * sizeof(HashItem));
	LOG1("HashItem size: %d", sizeof(HashItem));
}

class Engine {
private:
	static const int RANK_TOP = 3;
	static const int RANK_BOTTOM = 12;
	static const int FILE_LEFT = 3;
	static const int FILE_RIGHT = 11;

private:
	///////////////////////////////////////////////////////////////////
	static const char ccInBoard[256];
	static const char ccInFort[256];
	static const char ccLegalSpan[512];
	static const signed char ccKnightPin[512];
	static const byte cucpcStartup[256];
	static const byte cucvlPiecePos[7][256];
	static const signed char ccKingDelta[4];
	static const signed char ccAdvisorDelta[4];
	static const signed char ccKnightDelta[4][2];
	static const signed char ccKnightCheckDelta[4][2];
private:
	///////////////////////////////////////////////////////////////////
	static inline bool IN_BOARD(int sq);
	static inline bool IN_FORT(int sq);
	static inline int RANK_Y(int sq);
	static inline int FILE_X(int sq);
	static inline int COORD_XY(int x, int y);
	static inline int SQUARE_FLIP(int sq);
	static inline int FILE_FLIP(int x);
	static inline int RANK_FLIP(int y);
	static inline int MIRROR_SQUARE(int sq);
	static inline int SQUARE_FORWARD(int sq, int sd);
	static inline bool KING_SPAN(int sqSrc, int sqDst);
	static inline bool ADVISOR_SPAN(int sqSrc, int sqDst);
	static inline bool BISHOP_SPAN(int sqSrc, int sqDst);
	static inline int BISHOP_PIN(int sqSrc, int sqDst);
	static inline int KNIGHT_PIN(int sqSrc, int sqDst);
	static inline bool HOME_HALF(int sq, int sd);
	static inline bool AWAY_HALF(int sq, int sd);
	static inline bool SAME_HALF(int sqSrc, int sqDst);
	static inline bool SAME_RANK(int sqSrc, int sqDst);
	static inline bool SAME_FILE(int sqSrc, int sqDst);
	static inline int SIDE_TAG(int sd);
	static inline int OPP_SIDE_TAG(int sd);
	static inline int SRC(int mv);
	static inline int DST(int mv);
	static inline int MOVE(int sqSrc, int sqDst);
	static inline int MIRROR_MOVE(int mv);

private:
	friend struct SortStruct;
	int player;
	byte board[256];
	int vlWhite, vlBlack;
	int nDistance, nMoveNum;
	MoveStruct mvsList[MAX_MOVES];
	ZobristStruct zobr;
	static Zobrist zobrist;
	std::vector<MoveStruct> history;

	int direction;
	Search search;

private:
	///////////////////////////////////////////////////////////////////
	inline void clear(void);
	inline void clearHistory(void);
	inline void changeSide(void);
	void addPiece(int sq, int pc);
	void delPiece(int sq, int pc);
	int evaluate(void) const;
	bool inCheck(void) const;
	bool captured(void) const;
	int putPiece(int mv);
	void undoPut(int mv, int pcCaptured);
	bool move(int mv);
	void undoMove(void);
	void nullMove(void);
	void undoNullMove(void);
	bool testMove(int mv) const;
	int generateMoves(int *mvs, bool bCapture = false) const;
	bool checked() const;
	bool isMate(void);
	inline int getDrawValue(void) const;
	int getRepeatPos(int nRecur = 1) const;
	inline int getRepeatValue(int nRepStatus) const;
	void mirror(Engine &posMirror) const;
	inline bool allowNullMove(void) const;

private:
	///////////////////////////////////////////////////////////////////
	int searchBook(void);
	int probeHash(int vlAlpha, int vlBeta, int nDepth, int &mv);
	void recordHash(int nFlag, int vl, int nDepth, int mv);
	int searchQuiescence(int vlAlpha, int vlBeta);
	void setBestMove(int mv, int nDepth);
	int searchFull(int vlAlpha, int vlBeta, int nDepth,
			bool bNoNull = false);
	int searchRoot(int nDepth);
	int searchMain(float seconds);

public:
	static const int PIECE_KING = 0;
	static const int PIECE_ADVISOR = 1;
	static const int PIECE_BISHOP = 2;
	static const int PIECE_KNIGHT = 3;
	static const int PIECE_ROOK = 4;
	static const int PIECE_CANNON = 5;
	static const int PIECE_PAWN = 6;

	static const int NORMAL = 0;
	static const int NORMAL_CAPTURED = 1;
	static const int NORMAL_CHECKED = 2;
	static const int RED_WIN = 3;
	static const int BLACK_WIN = 4;
	static const int DRAW = 5;
	static const int REPEATED = 0x100;
	static const int EXCEEDED_100 = 0x200;

public:
	void startup(int direction);
	bool move(int fromX, int fromY, int toX, int toY);
	void undo();
	inline int getMoveCount() const;
	inline int getPlayer() const;
	int getState(byte* board);
	bool findSolution(float searchSeconds, int& fromX, int& fromY,
			int& toX, int& toY);

	inline bool getLastMove(int& fromX, int& fromY, int& toX, int& toY);
};


///////////////////////////////////////////////////////////////////////////

inline bool Engine::IN_BOARD(int sq) {
#if defined(ANDROID_DEBUG)
	if (sq > 255) {
		exit(-1);
	}
#endif
	return ccInBoard[sq] != 0;
}

inline bool Engine::IN_FORT(int sq) {
	return ccInFort[sq] != 0;
}

inline int Engine::RANK_Y(int sq) {
	return sq >> 4;
}

inline int Engine::FILE_X(int sq) {
	return sq & 15;
}

inline int Engine::COORD_XY(int x, int y) {
	return x + (y << 4);
}

inline int Engine::SQUARE_FLIP(int sq) {
	return 254 - sq;
}

inline int Engine::FILE_FLIP(int x) {
	return 14 - x;
}

inline int Engine::RANK_FLIP(int y) {
	return 15 - y;
}

inline int Engine::MIRROR_SQUARE(int sq) {
	return COORD_XY(FILE_FLIP(FILE_X(sq)), RANK_Y(sq));
}

inline int Engine::SQUARE_FORWARD(int sq, int sd) {
	return sq - 16 + (sd << 5);
}

inline bool Engine::KING_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 1;
}

inline bool Engine::ADVISOR_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 2;
}

inline bool Engine::BISHOP_SPAN(int sqSrc, int sqDst) {
	return ccLegalSpan[sqDst - sqSrc + 256] == 3;
}

inline int Engine::BISHOP_PIN(int sqSrc, int sqDst) {
	return (sqSrc + sqDst) >> 1;
}

inline int Engine::KNIGHT_PIN(int sqSrc, int sqDst) {
	return sqSrc + ccKnightPin[sqDst - sqSrc + 256];
}

inline bool Engine::HOME_HALF(int sq, int sd) {
	return (sq & 0x80) != (sd << 7);
}

inline bool Engine::AWAY_HALF(int sq, int sd) {
	return (sq & 0x80) == (sd << 7);
}

inline bool Engine::SAME_HALF(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x80) == 0;
}

inline bool Engine::SAME_RANK(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0xf0) == 0;
}

inline bool Engine::SAME_FILE(int sqSrc, int sqDst) {
	return ((sqSrc ^ sqDst) & 0x0f) == 0;
}

inline int Engine::SIDE_TAG(int sd) {
	return 8 + (sd << 3);
}

inline int Engine::OPP_SIDE_TAG(int sd) {
	return 16 - (sd << 3);
}

inline int Engine::SRC(int mv) {
	return mv & 255;
}

inline int Engine::DST(int mv) {
	return mv >> 8;
}

inline int Engine::MOVE(int sqSrc, int sqDst) {
	return sqSrc + sqDst * 256;
}

inline int Engine::MIRROR_MOVE(int mv) {
	return MOVE(MIRROR_SQUARE(SRC(mv)), MIRROR_SQUARE(DST(mv)));
}

//////////////////////////////////////////////////////////////////////

inline void Engine::clear(void) {
	player = vlWhite = vlBlack = nDistance = 0;
	memset(board, 0, 256);
	zobr.InitZero();
}

inline void Engine::clearHistory(void) {
	mvsList[0].set(0, 0, checked(), zobr.dwKey);
	nMoveNum = 1;
}

inline void Engine::changeSide(void) {
	player = 1 - player;
	zobr.Xor(zobrist.Player);
}

inline void Engine::addPiece(int sq, int pc) {
	board[sq] = pc;
	if (pc < 16) {
		vlWhite += cucvlPiecePos[pc - 8][sq];
		zobr.Xor(zobrist.Table[pc - 8][sq]);
	} else {
		vlBlack += cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		zobr.Xor(zobrist.Table[pc - 9][sq]);
	}
}

inline void Engine::delPiece(int sq, int pc) {
	board[sq] = 0;
	if (pc < 16) {
		vlWhite -= cucvlPiecePos[pc - 8][sq];
		zobr.Xor(zobrist.Table[pc - 8][sq]);
	} else {
		vlBlack -= cucvlPiecePos[pc - 16][SQUARE_FLIP(sq)];
		zobr.Xor(zobrist.Table[pc - 9][sq]);
	}
}

inline int Engine::evaluate(void) const {
	return (player == 0 ? vlWhite - vlBlack : vlBlack - vlWhite)
			+ ADVANCED_VALUE;
}

inline bool Engine::inCheck(void) const {
	return mvsList[nMoveNum - 1].ucbCheck != 0;
}

inline bool Engine::captured(void) const {
	if (history.empty())
		return false;
	else
		return history.back().ucpcCaptured != 0;
//	return mvsList[nMoveNum - 1].ucpcCaptured != 0;
}

inline int Engine::putPiece(int mv) {
	int sqSrc, sqDst, pc, pcCaptured;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pcCaptured = board[sqDst];
	if (pcCaptured != 0) {
		delPiece(sqDst, pcCaptured);
	}
	pc = board[sqSrc];
	delPiece(sqSrc, pc);
	addPiece(sqDst, pc);
	return pcCaptured;
}

inline void Engine::undoPut(int mv, int pcCaptured) {
	int sqSrc, sqDst, pc;
	sqSrc = SRC(mv);
	sqDst = DST(mv);
	pc = board[sqDst];
	delPiece(sqDst, pc);
	addPiece(sqSrc, pc);
	if (pcCaptured != 0) {
		addPiece(sqDst, pcCaptured);
	}
}

inline bool Engine::move(int mv) {
	int pcCaptured;
	uint32 dwKey;

	dwKey = zobr.dwKey;
	pcCaptured = putPiece(mv);
	if (checked()) {
		undoPut(mv, pcCaptured);
		return false;
	}
	changeSide();
	mvsList[nMoveNum].set(mv, pcCaptured, checked(), dwKey);
	nMoveNum++;
#if defined(ANDROID_DEBUG)
	if (nMoveNum >= MAX_MOVES) {
		LOG("nMoveNum out of range.");
		exit(nMoveNum);
	}
#endif
	nDistance++;
	return true;
}

inline void Engine::undoMove(void) {
	nDistance--;
	nMoveNum--;
	changeSide();
	undoPut(mvsList[nMoveNum].wmv, mvsList[nMoveNum].ucpcCaptured);
}

inline void Engine::nullMove(void) {
	uint32 dwKey;
	dwKey = zobr.dwKey;
	changeSide();
	mvsList[nMoveNum].set(0, 0, false, dwKey);
	nMoveNum++;
#if defined(ANDROID_DEBUG)
	if (nMoveNum >= MAX_MOVES) {
		LOG("nMoveNum out of range.");
		exit(nMoveNum);
	}
#endif
	nDistance++;
}

inline void Engine::undoNullMove(void) {
	nDistance--;
	nMoveNum--;
	changeSide();
}

inline bool Engine::allowNullMove(void) const {
	return (player == 0 ? vlWhite : vlBlack) > NULL_MARGIN;
}

inline int Engine::getRepeatValue(int nRepStatus) const {
	int vlReturn;
	vlReturn = ((nRepStatus & 2) == 0 ? 0 : nDistance - BAN_VALUE)
			+ ((nRepStatus & 4) == 0 ? 0 : BAN_VALUE - nDistance);
	return vlReturn == 0 ? getDrawValue() : vlReturn;
}

inline int Engine::getDrawValue(void) const {
	return (nDistance & 1) == 0 ? -DRAW_VALUE : DRAW_VALUE;
}

inline int Engine::getMoveCount() const {
	return (int)history.size();
}

inline bool Engine::getLastMove(int& fromX, int& fromY, int& toX, int& toY) {
	if (history.size() <= 0)
		return false;
	uint32 mv = history.back().wmv;
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

inline int Engine::getPlayer() const {
	return player;
}



} // End namespace.


#endif /* ENGINE_H_ */
