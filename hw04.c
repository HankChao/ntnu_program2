#include "xiangqi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//=== 工具函數 ===============================================

// 檢查座標是否合法
static bool is_valid_position(int x, int y) {
    return (x >= 0 && x < BOARD_ROWS) && (y >= 0 && y < BOARD_COLS);
}

// 判斷是否在九宮格內
static bool is_in_red_palace(int x, int y) {
    return (x >= 0 && x <= 2) && (y >= 3 && y <= 5);
}

static bool is_in_black_palace(int x, int y) {
    return (x >= 7 && x <= 9) && (y >= 3 && y <= 5);
}

// 判斷是否同一方棋子
static bool is_same_side(uint8_t piece1, uint8_t piece2) {
    if (piece1 == EMPTY || piece2 == EMPTY) return false;
    return (piece1 >= RED_CHARIOT) == (piece2 >= RED_CHARIOT);
}

//=== 棋子移動規則驗證 ========================================

// 將/帥
static bool validate_general(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    uint8_t piece = record->board[x][y];
    bool is_red = (piece == RED_GENERAL);

    // 目標是否在九宮格內
    if (is_red && !is_in_red_palace(new_x, new_y)) return false;
    if (!is_red && !is_in_black_palace(new_x, new_y)) return false;

    // 移動一步 (橫或豎)
    int dx = abs(new_x - x), dy = abs(new_y - y);
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

// 士/仕
static bool validate_advisor(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    uint8_t piece = record->board[x][y];
    bool is_red = (piece == RED_ADVISOR);

    // 目標是否在九宮格內
    if (is_red && !is_in_red_palace(new_x, new_y)) return false;
    if (!is_red && !is_in_black_palace(new_x, new_y)) return false;

    // 斜線移動一步
    int dx = abs(new_x - x), dy = abs(new_y - y);
    return (dx == 1 && dy == 1);
}

// 象/相
static bool validate_elephant(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    uint8_t piece = record->board[x][y];
    bool is_red = (piece == RED_ELEPHANT);

    // 不能過河
    if (is_red && new_x < 5) return false;
    if (!is_red && new_x > 4) return false;

    // 田字移動
    int dx = abs(new_x - x), dy = abs(new_y - y);
    if (dx != 2 || dy != 2) return false;

    // 檢查象眼
    int block_x = (x + new_x) / 2;
    int block_y = (y + new_y) / 2;
    return record->board[block_x][block_y] == EMPTY;
}

// 車/俥
static bool validate_chariot(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    if (x != new_x && y != new_y) return false;

    // 檢查路徑是否被阻擋
    if (x == new_x) {
        int step = (new_y > y) ? 1 : -1;
        for (int j = y + step; j != new_y; j += step) {
            if (record->board[x][j] != EMPTY) return false;
        }
    } else {
        int step = (new_x > x) ? 1 : -1;
        for (int i = x + step; i != new_x; i += step) {
            if (record->board[i][y] != EMPTY) return false;
        }
    }
    return true;
}

// 馬/傌
static bool validate_horse(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    int dx = abs(new_x - x), dy = abs(new_y - y);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return false;

    // 檢查蹩腳
    int block_x = x + (new_x - x)/2;
    int block_y = y + (new_y - y)/2;
    if (dx == 2) block_y = y;
    else block_x = x;
    return record->board[block_x][block_y] == EMPTY;
}

// 炮/砲
static bool validate_cannon(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    if (x != new_x && y != new_y) return false;

    int count = 0;
    if (x == new_x) {
        int step = (new_y > y) ? 1 : -1;
        for (int j = y + step; j != new_y; j += step) {
            if (record->board[x][j] != EMPTY) count++;
        }
    } else {
        int step = (new_x > x) ? 1 : -1;
        for (int i = x + step; i != new_x; i += step) {
            if (record->board[i][y] != EMPTY) count++;
        }
    }

    // 吃子需隔一子，移動不隔子
    if (record->board[new_x][new_y] == EMPTY) return (count == 0);
    else return (count == 1);
}

// 兵/卒
static bool validate_soldier(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    uint8_t piece = record->board[x][y];
    bool is_red = (piece == RED_SOLDIER);
    int dx = new_x - x;
    int dy = abs(new_y - y);

    // 紅兵：過河前只能向前，過河後可左右
    if (is_red) {
        if (x >= 5) { // 未過河
            if (dx != -1 || dy != 0) return false;
        } else {     // 已過河
            if (dx != -1 && dx != 0) return false;
            if (dy > 1) return false;
        }
    } else {         // 黑卒
        if (x <= 4) { // 未過河
            if (dx != 1 || dy != 0) return false;
        } else {     // 已過河
            if (dx != 1 && dx != 0) return false;
            if (dy > 1) return false;
        }
    }
    return true;
}

//=== 初始化棋盤 ===============================================

sXiangqiRecord* initXiangqiRecord(void) {
    sXiangqiRecord* record = malloc(sizeof(sXiangqiRecord));
    if (!record) return NULL;

    // 初始化棋盤 (左下角為 (0,0))
    memset(record->board, EMPTY, sizeof(record->board));

    // 紅方布局 (行 0~2)
    record->board[0][0] = RED_CHARIOT;      // (0,0)
    record->board[0][8] = RED_CHARIOT;      // (0,8)
    record->board[0][1] = RED_HORSE;        // (0,1)
    record->board[0][7] = RED_HORSE;        // (0,7)
    record->board[0][2] = RED_ELEPHANT;     // (0,2)
    record->board[0][6] = RED_ELEPHANT;     // (0,6)
    record->board[0][3] = RED_ADVISOR;      // (0,3)
    record->board[0][5] = RED_ADVISOR;      // (0,5)
    record->board[0][4] = RED_GENERAL;      // (0,4)
    record->board[2][1] = RED_CANNON;       // (2,1)
    record->board[2][7] = RED_CANNON;       // (2,7)
    for (int i = 0; i < 5; i++) record->board[3][i*2] = RED_SOLDIER; // (3,0), (3,2), ...

    // 黑方布局 (行 7~9)
    record->board[9][0] = BLACK_CHARIOT;    // (9,0)
    record->board[9][8] = BLACK_CHARIOT;    // (9,8)
    record->board[9][1] = BLACK_HORSE;      // (9,1)
    record->board[9][7] = BLACK_HORSE;      // (9,7)
    record->board[9][2] = BLACK_ELEPHANT;   // (9,2)
    record->board[9][6] = BLACK_ELEPHANT;   // (9,6)
    record->board[9][3] = BLACK_ADVISOR;    // (9,3)
    record->board[9][5] = BLACK_ADVISOR;    // (9,5)
    record->board[9][4] = BLACK_GENERAL;    // (9,4)
    record->board[7][1] = BLACK_CANNON;     // (7,1)
    record->board[7][7] = BLACK_CANNON;     // (7,7)
    for (int i = 0; i < 5; i++) record->board[6][i*2] = BLACK_SOLDIER; // (6,0), (6,2), ...

    // 初始化其他字段
    memset(record->red_captured, 0, sizeof(record->red_captured));
    memset(record->black_captured, 0, sizeof(record->black_captured));
    record->move_count = 0;
    record->moves = NULL;
    record->is_red_turn = true;  // 紅方先手
    record->game_over = false;
    return record;
}

//=== 釋放內存 ===============================================

void freeXiangqiRecord(sXiangqiRecord* record) {
    if (record) {
        free(record->moves);
        free(record);
    }
}

//=== 移動驗證與執行 =========================================

static bool validate_move(sXiangqiRecord* record, int x, int y, int new_x, int new_y) {
    if (record->game_over) return false;
    if (!is_valid_position(x, y) || !is_valid_position(new_x, new_y)) return false;

    uint8_t piece = record->board[x][y];
    if (piece == EMPTY) return false;

    // 輪次檢查
    bool is_red = (piece >= RED_CHARIOT);
    if (is_red != record->is_red_turn) return false;

    // 目標位置檢查
    uint8_t target = record->board[new_x][new_y];
    if (target != EMPTY && is_same_side(piece, target)) return false;

    // 分棋子類型驗證
    switch (piece) {
        case RED_GENERAL:
        case BLACK_GENERAL: return validate_general(record, x, y, new_x, new_y);
        case RED_ADVISOR:
        case BLACK_ADVISOR: return validate_advisor(record, x, y, new_x, new_y);
        case RED_ELEPHANT:
        case BLACK_ELEPHANT: return validate_elephant(record, x, y, new_x, new_y);
        case RED_CHARIOT:
        case BLACK_CHARIOT: return validate_chariot(record, x, y, new_x, new_y);
        case RED_HORSE:
        case BLACK_HORSE: return validate_horse(record, x, y, new_x, new_y);
        case RED_CANNON:
        case BLACK_CANNON: return validate_cannon(record, x, y, new_x, new_y);
        case RED_SOLDIER:
        case BLACK_SOLDIER: return validate_soldier(record, x, y, new_x, new_y);
        default: return false;
    }
}

//=== 核心移動邏輯 =========================================

int32_t moveXiangqiRecord(sXiangqiRecord* record, uint8_t x, uint8_t y, uint8_t new_x, uint8_t new_y) {
    if (!record || !validate_move(record, x, y, new_x, new_y)) return -1;

    // 記錄被吃棋子
    uint8_t target = record->board[new_x][new_y];
    if (target != EMPTY) {
        if (target >= RED_CHARIOT) { // 紅方被吃
            for (int i = 0; i < 16; i++) {
                if (record->red_captured[i] == 0) {
                    record->red_captured[i] = target;
                    break;
                }
            }
        } else { // 黑方被吃
            for (int i = 0; i < 16; i++) {
                if (record->black_captured[i] == 0) {
                    record->black_captured[i] = target;
                    break;
                }
            }
        }
    }

    // 更新棋盤
    record->board[new_x][new_y] = record->board[x][y];
    record->board[x][y] = EMPTY;

    // 更新移動記錄
    sMoveRecord* new_moves = realloc(record->moves, (record->move_count + 1) * sizeof(sMoveRecord));
    if (!new_moves) return -1;
    record->moves = new_moves;
    record->moves[record->move_count++] = (sMoveRecord){x, y, new_x, new_y};

    // 切換回合
    record->is_red_turn = !record->is_red_turn;
    return 0;
}

//=== 列印棋盤 ===============================================

int32_t printXiangqiRecord(sXiangqiRecord* record) {
    if (!record) return -1;

    const char* pieces[] = {"　","車","馬","象","士","將","士","象","馬","車","炮","卒",
                            "俥","傌","相","仕","帥","仕","相","傌","俥","砲","兵"};
    
    // 從下到上列印行 (x: 9~0)
    printf("\n");
    for (int x = BOARD_ROWS-1; x >= 0; x--) {
        printf("%2d ", x);  // 行號 (x)
        for (int y = 0; y < BOARD_COLS; y++) {
            uint8_t p = record->board[x][y];
            if (p >= RED_CHARIOT) printf("\033[31m%s\033[0m ", pieces[p]);
            else printf("%s ", pieces[p]);
        }
        printf("\n");
    }
    
    // 列號 (y: 0~8)
    printf("   ");
    for (int y = 0; y < BOARD_COLS; y++) printf("%d ", y);
    printf("\n");
    return 0;
}

//=== 棋局回放 ===============================================

int32_t printXiangqiPlay(sXiangqiRecord* record) {
    if (!record) return -1;
    for (uint32_t i = 0; i < record->move_count; i++) {
        printf("Step %d: (%d,%d) → (%d,%d)\n", i+1,
               record->moves[i].x, record->moves[i].y,
               record->moves[i].new_x, record->moves[i].new_y);
        printXiangqiRecord(record);
        sleep(2);
    }
    return 0;
}