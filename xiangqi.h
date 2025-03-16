#ifndef XIANGQI_H
#define XIANGQI_H

#include <stdint.h>
#include <stdbool.h>

// 定義棋盤大小
#define BOARD_ROWS 10
#define BOARD_COLS 9

// 定義棋子類型
enum Piece {
    EMPTY = 0,
    RED_GENERAL = 1,   // 帥
    RED_ADVISOR = 2,   // 仕
    RED_ELEPHANT = 3,  // 相
    RED_CHARIOT = 4,   // 俥
    RED_HORSE = 5,     // 傌
    RED_CANNON = 6,    // 炮
    RED_SOLDIER = 7,   // 兵
    BLACK_GENERAL = 8,  // 將
    BLACK_ADVISOR = 9,  // 士
    BLACK_ELEPHANT = 10,// 象
    BLACK_CHARIOT = 11, // 車
    BLACK_HORSE = 12,   // 馬
    BLACK_CANNON = 13,  // 包
    BLACK_SOLDIER = 14  // 卒
};

// 定義移動記錄結構
typedef struct {
    uint8_t x, y, new_x, new_y;
} sMoveRecord;

// 定義象棋記錄結構
typedef struct {
    uint8_t board[BOARD_ROWS][BOARD_COLS];
    uint8_t red_captured[16];
    uint8_t black_captured[16];
    uint32_t move_count;
    sMoveRecord* moves;
    bool is_red_turn;
    bool game_over;
} sXiangqiRecord;

// 函數原型
sXiangqiRecord* initXiangqiRecord(void);
void freeXiangqiRecord(sXiangqiRecord* record);
int32_t moveXiangqiRecord(sXiangqiRecord* record, uint8_t x, uint8_t y, uint8_t new_x, uint8_t new_y);
int32_t printXiangqiRecord(sXiangqiRecord* record);
int32_t printXiangqiPlay(sXiangqiRecord* record);

#endif // XIANGQI_H