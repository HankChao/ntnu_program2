#include "xiangqi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <locale.h>

//=== 工具函数 ===============================================
static bool is_valid_position(int x, int y) {
    return (x >= 0 && x < BOARD_ROWS) && (y >= 0 && y < BOARD_COLS);
}

static bool is_red_piece(uint8_t piece) {
    return (piece >= RED_GENERAL && piece <= RED_SOLDIER);
}

//=== 九宫格检查 =============================================
static bool is_in_red_palace(int x, int y) {
    return (x >= 0 && x <= 2) && (y >= 3 && y <= 5);  // 紅方九宮格在底部
}

static bool is_in_black_palace(int x, int y) {
    return (x >= 7 && x <= 9) && (y >= 3 && y <= 5);  // 黑方九宮格在頂部
}

//=== 棋子移动规则验证 ========================================
static bool validate_general(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    // 检查目标位置是否在九宫格内
    if (red && !is_in_red_palace(nx, ny)) return false;
    if (!red && !is_in_black_palace(nx, ny)) return false;

    // 移动步长检查（一步）
    int dx = abs(nx - x), dy = abs(ny - y);
    if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) return false;

    // 檢查是否與對方的將對臉
    uint8_t enemy_general = red ? BLACK_GENERAL : RED_GENERAL;
    int ex = -1, ey = -1;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_general) {
                ex = i;
                ey = j;
                break;
            }
        }
        if (ex != -1) break;
    }
    if (ex == -1) return true; // 對方將已被吃，無需檢查對臉

    // 是否在同一列且中間無棋子？
    if (ny == ey) {  // 修正：比較 y 座標是否相同
        int start = (nx < ex) ? nx + 1 : ex + 1;
        int end = (nx < ex) ? ex - 1 : nx - 1;
        for (int i = start; i <= end; i++) {
            if (r->board[i][ny] != EMPTY) return true; // 有棋子阻擋，允許移動
        }
        return false; // 無棋子，對臉非法
    }

    return true;
}

static bool validate_advisor(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    if (red && !is_in_red_palace(nx, ny)) return false;
    if (!red && !is_in_black_palace(nx, ny)) return false;

    int dx = abs(nx - x), dy = abs(ny - y);
    return (dx == 1 && dy == 1);
}

static bool validate_elephant(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    // 紅象：不能過河（x <= 4）
    if (red && nx > 4) return false;
    // 黑象：不能過河（x >= 5）
    if (!red && nx < 5) return false;

    int dx = abs(nx - x), dy = abs(ny - y);
    if (dx != 2 || dy != 2) return false;

    // 檢查象眼位置（中間點必須為整數座標）
    int block_x = x + (nx - x)/2;
    int block_y = y + (ny - y)/2;
    return r->board[block_x][block_y] == EMPTY;
}

static bool validate_chariot(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    if (x != nx && y != ny) return false;

    int step;
    if (x == nx) {
        step = (ny > y) ? 1 : -1;
        for (int j = y + step; j != ny; j += step) {
            if (r->board[x][j] != EMPTY) return false;
        }
    } else {
        step = (nx > x) ? 1 : -1;
        for (int i = x + step; i != nx; i += step) {
            if (r->board[i][y] != EMPTY) return false;
        }
    }
    return true;
}

static bool validate_horse(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    int dx = abs(nx - x), dy = abs(ny - y);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return false;

    // 蹩馬腿檢測修正
    int block_x = x, block_y = y;
    if (dx == 2) {          // 橫向移動兩格
        block_x = x + ((nx > x) ? 1 : -1);
        block_y = y;
    } else {                // 縱向移動兩格
        block_x = x;
        block_y = y + ((ny > y) ? 1 : -1);
    }
    return r->board[block_x][block_y] == EMPTY;
}

static bool validate_cannon(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    if (x != nx && y != ny) return false;

    int count = 0;
    int step;
    if (x == nx) {
        step = (ny > y) ? 1 : -1;
        for (int j = y + step; j != ny; j += step) {
            if (r->board[x][j] != EMPTY) count++;
        }
    } else {
        step = (nx > x) ? 1 : -1;
        for (int i = x + step; i != nx; i += step) {
            if (r->board[i][y] != EMPTY) count++;
        }
    }

    if (r->board[nx][ny] == EMPTY) return (count == 0);
    return (count == 1);
}

static bool validate_soldier(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);
    int dx = nx - x;
    int dy = abs(ny - y);

    // 紅兵
    if (red) {
        if (x <= 4) {       // 未過河（河界在 x=4.5），只能前進（x 增加）
            if (dx != 1 || dy != 0) return false;
        } else {            // 已過河，可橫移或前進
            if (dx != 1 && dx != 0) return false;  // 禁止後退（dx < 0）
            if (dy > 1) return false;
        }
    } 
    // 黑兵
    else {
        if (x >= 5) {       // 未過河，只能前進（x 減少）
            if (dx != -1 || dy != 0) return false;
        } else {            // 已過河，可橫移或前進
            if (dx != -1 && dx != 0) return false;
            if (dy > 1) return false;
        }
    }
    return true;
}

//=== 初始化棋盘 =============================================
sXiangqiRecord* initXiangqiRecord(void) {
    sXiangqiRecord* r = malloc(sizeof(sXiangqiRecord));
    if (!r) return NULL;

    memset(r->board, EMPTY, sizeof(r->board));

    // 紅方布局（底部，x=0~3）
    r->board[0][0] = RED_CHARIOT;
    r->board[0][1] = RED_HORSE;
    r->board[0][2] = RED_ELEPHANT;
    r->board[0][3] = RED_ADVISOR;
    r->board[0][4] = RED_GENERAL;
    r->board[0][5] = RED_ADVISOR;
    r->board[0][6] = RED_ELEPHANT;
    r->board[0][7] = RED_HORSE;
    r->board[0][8] = RED_CHARIOT;

    r->board[2][1] = RED_CANNON;
    r->board[2][7] = RED_CANNON;

    for (int y = 0; y < 9; y += 2) {
        r->board[3][y] = RED_SOLDIER;
    }

    // 黑方布局（頂部，x=7~9）
    r->board[9][0] = BLACK_CHARIOT;
    r->board[9][1] = BLACK_HORSE;
    r->board[9][2] = BLACK_ELEPHANT;
    r->board[9][3] = BLACK_ADVISOR;
    r->board[9][4] = BLACK_GENERAL;
    r->board[9][5] = BLACK_ADVISOR;
    r->board[9][6] = BLACK_ELEPHANT;
    r->board[9][7] = BLACK_HORSE;
    r->board[9][8] = BLACK_CHARIOT;

    r->board[7][1] = BLACK_CANNON;
    r->board[7][7] = BLACK_CANNON;

    for (int y = 0; y < 9; y += 2) {
        r->board[6][y] = BLACK_SOLDIER;
    }

    memset(r->red_captured, 0, sizeof(r->red_captured));
    memset(r->black_captured, 0, sizeof(r->black_captured));
    r->move_count = 0;
    r->moves = NULL;
    r->is_red_turn = true;
    r->game_over = false;
    memset(r->last_error, 0, sizeof(r->last_error));
    return r;
}

void freeXiangqiRecord(sXiangqiRecord* r) {
    if (r) {
        free(r->moves);
        free(r);
    }
}

//=== 移动验证与执行 =========================================
static void check_game_over(sXiangqiRecord* r, bool is_red) {
    uint8_t target_general = is_red ? BLACK_GENERAL : RED_GENERAL;
    bool found = false;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == target_general) {
                found = true;
                break;
            }
        }
        if (found) break;
    }
    if (!found) r->game_over = true;
}

int32_t moveXiangqiRecord(sXiangqiRecord* r, uint8_t x, uint8_t y, uint8_t nx, uint8_t ny) {
    memset(r->last_error, 0, sizeof(r->last_error));

    if (r->game_over) {
        snprintf(r->last_error, sizeof(r->last_error), "遊戲已結束");
        return -1;
    }
    if (!is_valid_position(x, y) || !is_valid_position(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "無效座標");
        return -1;
    }

    uint8_t piece = r->board[x][y];
    if (piece == EMPTY) {
        snprintf(r->last_error, sizeof(r->last_error), "無棋子可移動");
        return -1;
    }

    bool is_red = is_red_piece(piece);
    if (is_red != r->is_red_turn) {
        snprintf(r->last_error, sizeof(r->last_error), "輪到%s方回合", r->is_red_turn?"紅":"黑");
        return -1;
    }

    uint8_t target = r->board[nx][ny];
    if (target != EMPTY) {
        bool same_side = (is_red == is_red_piece(target));
        if (same_side) {
            snprintf(r->last_error, sizeof(r->last_error), "不可吃己方棋子");
            return -1;
        }
    }

    bool valid = false;
    switch (piece) {
        case RED_GENERAL: case BLACK_GENERAL: valid = validate_general(r, x, y, nx, ny); break;
        case RED_ADVISOR: case BLACK_ADVISOR: valid = validate_advisor(r, x, y, nx, ny); break;
        case RED_ELEPHANT: case BLACK_ELEPHANT: valid = validate_elephant(r, x, y, nx, ny); break;
        case RED_CHARIOT: case BLACK_CHARIOT: valid = validate_chariot(r, x, y, nx, ny); break;
        case RED_HORSE: case BLACK_HORSE: valid = validate_horse(r, x, y, nx, ny); break;
        case RED_CANNON: case BLACK_CANNON: valid = validate_cannon(r, x, y, nx, ny); break;
        case RED_SOLDIER: case BLACK_SOLDIER: valid = validate_soldier(r, x, y, nx, ny); break;
        default: snprintf(r->last_error, sizeof(r->last_error), "未知棋子類型"); return -1;
    }

    if (!valid) {
        snprintf(r->last_error, sizeof(r->last_error), "移動不符合規則");
        return -1;
    }

    // 執行移動並處理俘虜
    if (target != EMPTY) {
        // 根據移動方存入對應俘虜列表
        if (is_red) {
            for (int i = 0; i < 16; i++) {
                if (r->red_captured[i] == 0) {
                    r->red_captured[i] = target;
                    break;
                }
            }
        } else {
            for (int i = 0; i < 16; i++) {
                if (r->black_captured[i] == 0) {
                    r->black_captured[i] = target;
                    break;
                }
            }
        }
    }

    r->board[nx][ny] = piece;
    r->board[x][y] = EMPTY;

    // 添加移動記錄
    sMoveRecord* new_moves = realloc(r->moves, (r->move_count + 1) * sizeof(sMoveRecord));
    if (!new_moves) {
        snprintf(r->last_error, sizeof(r->last_error), "記憶體分配失敗");
        return -1;
    }
    r->moves = new_moves;
    r->moves[r->move_count++] = (sMoveRecord){x, y, nx, ny};

    // 檢查是否將死
    check_game_over(r, is_red);

    r->is_red_turn = !r->is_red_turn;
    return 0;
}

//=== 棋盘打印 ===============================================
int32_t printXiangqiRecord(sXiangqiRecord* r) {
    if (!r) return -1;

    const char* pieces[] = {
        "　", "帥", "仕", "相", "俥", "傌", "炮", "兵",
        "將", "士", "象", "車", "馬", "包", "卒"
    };

    printf("\n當前回合: \033[1;33m%s方\033[0m\t\t死亡棋子:\n", r->is_red_turn ? "紅" : "黑");
    printf("紅方死亡: ");
    for (int i = 0; i < 16; i++) {
        if (r->red_captured[i]) printf("\033[31m%s\033[0m ", pieces[r->red_captured[i]]);
    }
    
    printf("\n黑方死亡: ");
    for (int i = 0; i < 16; i++) {
        if (r->black_captured[i]) printf("%s ", pieces[r->black_captured[i]]);
    }
    
    printf("\n\n   ");
    for (int y = 0; y < BOARD_COLS; y++) printf("%d ", y);
    printf("\n  ┌─────────────────────────┐\n");

    for (int x = BOARD_ROWS-1; x >= 0; x--) {
        printf("%2d│", x);
        for (int y = 0; y < BOARD_COLS; y++) {
            uint8_t p = r->board[x][y];
            if (p >= RED_GENERAL && p <= RED_SOLDIER) {
                printf("\033[1;31m%s\033[0m ", pieces[p]);
            } else if (p >= BLACK_GENERAL && p <= BLACK_SOLDIER) {
                printf("%s ", pieces[p]);
            } else {
                printf("%s ", pieces[0]);
            }
        }
        printf("│%d\n", x);
    }

    printf("  └─────────────────────────┘\n   ");
    for (int y = 0; y < BOARD_COLS; y++) printf("%d ", y);
    
    if (r->last_error[0]) {
        printf("\n\033[1;31m錯誤: %s\033[0m", r->last_error);
    }
    
    printf("\n");
    return 0;
}

//=== 棋局回放 ===============================================
int32_t printXiangqiPlay(sXiangqiRecord* r) {
    if (!r) return -1;
    for (uint32_t i = 0; i < r->move_count; i++) {
        printf("第%2d步: (%d,%d)→(%d,%d)\n", i+1, 
               r->moves[i].x, r->moves[i].y,
               r->moves[i].new_x, r->moves[i].new_y);
        printXiangqiRecord(r);
        sleep(2);
    }
    return 0;
}