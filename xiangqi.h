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
    BLACK_CHARIOT, BLACK_HORSE, BLACK_ELEPHANT, BLACK_ADVISOR, BLACK_GENERAL, BLACK_CANNON, BLACK_SOLDIER,
    RED_CHARIOT, RED_HORSE, RED_ELEPHANT, RED_ADVISOR, RED_GENERAL, RED_CANNON, RED_SOLDIER
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