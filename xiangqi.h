#ifndef XIANGQI_H
#define XIANGQI_H

#include <stdint.h>
#include <stdbool.h>

#define BOARD_ROWS 10
#define BOARD_COLS 9

typedef enum {
    EMPTY = 0,
    RED_GENERAL = 1, RED_ADVISOR = 2,
    RED_ELEPHANT = 3, RED_CHARIOT = 4,
    RED_HORSE = 5, RED_CANNON = 6,
    RED_SOLDIER = 7,
    BLACK_GENERAL = 8, BLACK_ADVISOR = 9,
    BLACK_ELEPHANT = 10, BLACK_CHARIOT = 11,
    BLACK_HORSE = 12, BLACK_CANNON = 13,
    BLACK_SOLDIER = 14
} Piece;

typedef struct {
    uint8_t x, y, new_x, new_y;
} sMoveRecord;

typedef struct {
    uint8_t board[BOARD_ROWS][BOARD_COLS];
    uint8_t red_captured[16];
    uint8_t black_captured[16];
    uint32_t move_count;
    sMoveRecord* moves;
    bool is_red_turn;
    bool game_over;
    char last_error[128];
} sXiangqiRecord;

sXiangqiRecord* initXiangqiRecord(void);
void freeXiangqiRecord(sXiangqiRecord* record);
int32_t moveXiangqiRecord(sXiangqiRecord* record, uint8_t x, uint8_t y, uint8_t new_x, uint8_t new_y);
int32_t printXiangqiRecord(sXiangqiRecord* record);
int32_t printXiangqiPlay(sXiangqiRecord* record);

#endif