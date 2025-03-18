#include "xiangqi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <locale.h>

// === 工具函數 ===============================================
static bool is_valid_position(int x, int y) {
    return (x >= 0 && x < BOARD_ROWS) && (y >= 0 && y < BOARD_COLS);
}

static bool is_red_piece(uint8_t piece) {
    return (piece >= RED_GENERAL && piece <= RED_SOLDIER);
}

// === 九宮格檢查 =============================================
static bool is_in_red_palace(int x, int y) {
    return (x >= 0 && x <= 2) && (y >= 3 && y <= 5);
}

static bool is_in_black_palace(int x, int y) {
    return (x >= 7 && x <= 9) && (y >= 3 && y <= 5);
}

// === 棋子移動規則驗證 ========================================
static bool validate_general(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    // 檢查目標位置是否在九宮格內
    if (red && !is_in_red_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "紅帥不能離開九宮格");
        return false;
    }
    if (!red && !is_in_black_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "黑將不能離開九宮格");
        return false;
    }

    // 移動步長檢查（一步）
    int dx = abs(nx - x), dy = abs(ny - y);
    if (!((dx == 1 && dy == 0) || (dx == 0 && dy == 1))) {
        snprintf(r->last_error, sizeof(r->last_error), "將只能直線移動一步");
        return false;
    }

    // 對臉檢查
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
    if (ex == -1) return true; // 對方將已被吃

    // 檢查是否在同一列且中間無棋子
    if (ny == ey) {
        int start = (nx < ex) ? nx + 1 : ex + 1;
        int end = (nx < ex) ? ex - 1 : nx - 1;
        for (int i = start; i <= end; i++) {
            if (r->board[i][ny] != EMPTY)
                return true; // 有棋子阻擋則合法
        }
        snprintf(r->last_error, sizeof(r->last_error), "將軍對臉");
        return false;
    }

    return true;
}

static bool validate_advisor(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    if (red && !is_in_red_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "紅仕不能離開九宮格");
        return false;
    }
    if (!red && !is_in_black_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "黑士不能離開九宮格");
        return false;
    }

    int dx = abs(nx - x), dy = abs(ny - y);
    if (dx != 1 || dy != 1) {
        snprintf(r->last_error, sizeof(r->last_error), "仕必須斜走一步");
        return false;
    }
    return true;
}

static bool validate_elephant(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    if (red && nx > 4) {
        snprintf(r->last_error, sizeof(r->last_error), "紅象不能過河");
        return false;
    }
    if (!red && nx < 5) {
        snprintf(r->last_error, sizeof(r->last_error), "黑象不能過河");
        return false;
    }

    int dx = abs(nx - x), dy = abs(ny - y);
    if (dx != 2 || dy != 2) {
        snprintf(r->last_error, sizeof(r->last_error), "象必須走田字");
        return false;
    }

    // 檢查象眼
    int block_x = x + (nx - x) / 2;
    int block_y = y + (ny - y) / 2;
    if (r->board[block_x][block_y] != EMPTY) {
        snprintf(r->last_error, sizeof(r->last_error), "象眼被擋");
        return false;
    }
    return true;
}

static bool validate_chariot(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    if (x != nx && y != ny) {
        snprintf(r->last_error, sizeof(r->last_error), "車必須直線移動");
        return false;
    }

    int step;
    if (x == nx) {
        step = (ny > y) ? 1 : -1;
        for (int j = y + step; j != ny; j += step) {
            if (r->board[x][j] != EMPTY) {
                snprintf(r->last_error, sizeof(r->last_error), "車移動路徑被擋");
                return false;
            }
        }
    } else {
        step = (nx > x) ? 1 : -1;
        for (int i = x + step; i != nx; i += step) {
            if (r->board[i][y] != EMPTY) {
                snprintf(r->last_error, sizeof(r->last_error), "車移動路徑被擋");
                return false;
            }
        }
    }
    return true;
}

static bool validate_horse(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    int dx = abs(nx - x), dy = abs(ny - y);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        snprintf(r->last_error, sizeof(r->last_error), "馬必須走日字");
        return false;
    }

    // 蹩馬腿檢查
    int block_x = x, block_y = y;
    if (dx == 2) {
        block_x = x + ((nx > x) ? 1 : -1);
    } else {
        block_y = y + ((ny > y) ? 1 : -1);
    }
    if (r->board[block_x][block_y] != EMPTY) {
        snprintf(r->last_error, sizeof(r->last_error), "馬腿被擋");
        return false;
    }
    return true;
}

static bool validate_cannon(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    if (x != nx && y != ny) {
        snprintf(r->last_error, sizeof(r->last_error), "炮必須直線移動");
        return false;
    }

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

    if (r->board[nx][ny] == EMPTY) {
        if (count != 0) {
            snprintf(r->last_error, sizeof(r->last_error), "炮移動需無棋子");
            return false;
        }
    } else {
        if (count != 1) {
            snprintf(r->last_error, sizeof(r->last_error), "炮需隔一子吃子");
            return false;
        }
    }
    return true;
}

static bool validate_soldier(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);
    int dx = nx - x;
    int dy = abs(ny - y);

    if (red) {
        if (x < 5) { // 未過河
            if (dx != 1 || dy != 0) {
                snprintf(r->last_error, sizeof(r->last_error), "紅兵未過河只能前進");
                return false;
            }
        } else { // 已過河
            if (dx < 0) { // 不能後退
                snprintf(r->last_error, sizeof(r->last_error), "紅兵不可後退");
                return false;
            }
            if (dx > 1 || dy > 1 || (dx == 1 && dy == 1)) {
                snprintf(r->last_error, sizeof(r->last_error), "紅兵移動超過一步");
                return false;
            }
        }
    } else { // 黑卒
        if (x > 4) { // 未過河
            if (dx != -1 || dy != 0) {
                snprintf(r->last_error, sizeof(r->last_error), "黑卒未過河只能前進");
                return false;
            }
        } else { // 已過河
            if (dx > 0) { // 不能後退
                snprintf(r->last_error, sizeof(r->last_error), "黑卒不可後退");
                return false;
            }
            if (dx < -1 || dy > 1 || (dx == -1 && dy == 1)) {
                snprintf(r->last_error, sizeof(r->last_error), "黑卒移動超過一步");
                return false;
            }
        }
    }
    return true;
}

// === 初始化棋盤 =============================================
sXiangqiRecord* initXiangqiRecord(void) {
    sXiangqiRecord* r = malloc(sizeof(sXiangqiRecord));
    if (!r) return NULL;

    // 初始化棋盤為空
    memset(r->board, EMPTY, sizeof(r->board));

    // 紅方佈局
    r->board[0][0] = RED_CHARIOT;    // 俥 (0,0)
    r->board[0][1] = RED_HORSE;      // 傌 (0,1)
    r->board[0][2] = RED_ELEPHANT;   // 相 (0,2)
    r->board[0][3] = RED_ADVISOR;    // 仕 (0,3)
    r->board[0][4] = RED_GENERAL;    // 帥 (0,4)
    r->board[0][5] = RED_ADVISOR;    // 仕 (0,5)
    r->board[0][6] = RED_ELEPHANT;   // 相 (0,6)
    r->board[0][7] = RED_HORSE;      // 傌 (0,7)
    r->board[0][8] = RED_CHARIOT;    // 俥 (0,8)

    r->board[2][1] = RED_CANNON;     // 炮 (2,1)
    r->board[2][7] = RED_CANNON;     // 炮 (2,7)

    for (int y = 0; y < BOARD_COLS; y += 2) {
        r->board[3][y] = RED_SOLDIER; // 兵 (3,0), (3,2), ..., (3,8)
    }

    // 黑方佈局
    r->board[9][0] = BLACK_CHARIOT;   // 車 (9,0)
    r->board[9][1] = BLACK_HORSE;     // 馬 (9,1)
    r->board[9][2] = BLACK_ELEPHANT;  // 象 (9,2)
    r->board[9][3] = BLACK_ADVISOR;   // 士 (9,3)
    r->board[9][4] = BLACK_GENERAL;   // 將 (9,4)
    r->board[9][5] = BLACK_ADVISOR;   // 士 (9,5)
    r->board[9][6] = BLACK_ELEPHANT;  // 象 (9,6)
    r->board[9][7] = BLACK_HORSE;     // 馬 (9,7)
    r->board[9][8] = BLACK_CHARIOT;   // 車 (9,8)

    r->board[7][1] = BLACK_CANNON;    // 炮 (7,1)
    r->board[7][7] = BLACK_CANNON;    // 炮 (7,7)

    for (int y = 0; y < BOARD_COLS; y += 2) {
        r->board[6][y] = BLACK_SOLDIER; // 卒 (6,0), (6,2), ..., (6,8)
    }

    // 初始化其他成員
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

// === 移動驗證與執行 =========================================
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
    
    if (!found) {
        r->game_over = true;
    }
}

int32_t moveXiangqiRecord(sXiangqiRecord* r, uint8_t x, uint8_t y, uint8_t nx, uint8_t ny) {
    memset(r->last_error, 0, sizeof(r->last_error));

    if (r->game_over) {
        snprintf(r->last_error, sizeof(r->last_error), "遊戲已結束");
        return -1;
    }
    if (!is_valid_position(x, y) || !is_valid_position(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "無效座標 (%d,%d)→(%d,%d)", x, y, nx, ny);
        return -1;
    }

    uint8_t piece = r->board[x][y];
    if (piece == EMPTY) {
        snprintf(r->last_error, sizeof(r->last_error), "位置 (%d,%d) 無棋子", x, y);
        return -1;
    }

    bool is_red = is_red_piece(piece);
    if (is_red != r->is_red_turn) {
        snprintf(r->last_error, sizeof(r->last_error), "輪到%s方回合", r->is_red_turn ? "紅" : "黑");
        return -1;
    }

    uint8_t target = r->board[nx][ny];
    if (target != EMPTY) {
        // 不允許吃將
        if (target == RED_GENERAL || target == BLACK_GENERAL) {
            snprintf(r->last_error, sizeof(r->last_error), "不能吃將");
            return -1;
        }
        if (is_red_piece(target) == is_red) {
            snprintf(r->last_error, sizeof(r->last_error), "不可吃己方棋子");
            return -1;
        }
    }

    bool valid = false;
    switch (piece) {
        case RED_GENERAL: case BLACK_GENERAL:
            valid = validate_general(r, x, y, nx, ny);
            break;
        case RED_ADVISOR: case BLACK_ADVISOR:
            valid = validate_advisor(r, x, y, nx, ny);
            break;
        case RED_ELEPHANT: case BLACK_ELEPHANT:
            valid = validate_elephant(r, x, y, nx, ny);
            break;
        case RED_CHARIOT: case BLACK_CHARIOT:
            valid = validate_chariot(r, x, y, nx, ny);
            break;
        case RED_HORSE: case BLACK_HORSE:
            valid = validate_horse(r, x, y, nx, ny);
            break;
        case RED_CANNON: case BLACK_CANNON:
            valid = validate_cannon(r, x, y, nx, ny);
            break;
        case RED_SOLDIER: case BLACK_SOLDIER:
            valid = validate_soldier(r, x, y, nx, ny);
            break;
        default:
            snprintf(r->last_error, sizeof(r->last_error), "未知棋子類型");
            return -1;
    }

    if (!valid) return -1;

    // 添加移動記錄
    sMoveRecord* new_moves = realloc(r->moves, (r->move_count + 1) * sizeof(sMoveRecord));
    if (!new_moves) {
        snprintf(r->last_error, sizeof(r->last_error), "記憶體分配失敗");
        return -1;
    }
    r->moves = new_moves;
    r->moves[r->move_count].x = x;
    r->moves[r->move_count].y = y;
    r->moves[r->move_count].new_x = nx;
    r->moves[r->move_count].new_y = ny;
    r->move_count++;

    // 執行移動並處理俘虜（此處不允許吃將）
    if (target != EMPTY) {
        bool target_is_red = is_red_piece(target);
        if (target_is_red) {
            for (int i = 0; i < 16; i++) {
                if (r->black_captured[i] == 0) {
                    r->black_captured[i] = target;
                    break;
                }
            }
        } else {
            for (int i = 0; i < 16; i++) {
                if (r->red_captured[i] == 0) {
                    r->red_captured[i] = target;
                    break;
                }
            }
        }
    }

    // 執行移動
    r->board[nx][ny] = piece;
    r->board[x][y] = EMPTY;

    // 檢查遊戲結束
    check_game_over(r, is_red);

    // 切換回合
    r->is_red_turn = !r->is_red_turn;

    return 0;
}

// === 棋盤列印 ===============================================
int32_t printXiangqiRecord(sXiangqiRecord* r) {
    if (!r) return -1;

    const char* pieces[] = {
        "　", "帥", "仕", "相", "俥", "傌", "炮", "兵",
        "將", "士", "象", "車", "馬", "包", "卒"
    };

    printf("\n當前回合：\033[1;33m%s方\033[0m\t\t\n死亡棋子：\n", r->is_red_turn ? "紅" : "黑");
    printf("黑方死亡：");
    for (int i = 0; i < 16; i++) {
        if (r->red_captured[i])
            printf("\033[1;34m%s\033[0m ", pieces[r->red_captured[i]]);
    }
    printf("\n紅方死亡：");
    for (int i = 0; i < 16; i++) {
        if (r->black_captured[i])
            printf("\033[1;31m%s\033[0m ", pieces[r->black_captured[i]]);
    }

    // Header with adjusted spacing for Chinese characters
    printf("\n\n  0 1 2 3 4 5 6 7 8\n");
    printf(" ┌──────────────────┐\n");

    for (int x = BOARD_ROWS - 1; x >= 0; x--) {
        printf("%d│", x);
        for (int y = 0; y < BOARD_COLS; y++) {
            uint8_t p = r->board[x][y];
            if (p >= RED_GENERAL && p <= RED_SOLDIER) {
                printf("\033[1;31m%s\033[0m", pieces[p]);
            } else if (p >= BLACK_GENERAL && p <= BLACK_SOLDIER) {
                printf("\033[1;34m%s\033[0m", pieces[p]);
            } else {
                printf("%s", pieces[0]);
            }
        }
        printf("│%d\n", x);
    }

    printf(" └──────────────────┘\n");
    printf("  0 1 2 3 4 5 6 7 8\n");

    if (r->last_error[0]) {
        printf("\n\033[1;31m錯誤：%s\033[0m", r->last_error);
    }

    printf("\n");
    return 0;
}

// === 棋局重播 ===============================================
int32_t printXiangqiPlay(sXiangqiRecord* r) {
    if (!r) return -1;
    
    // 改用直接模擬移動而不使用moveXiangqiRecord
    sXiangqiRecord* replay = initXiangqiRecord();
    if (!replay) return -1;
    
    for (uint32_t i = 0; i < r->move_count; i++) {
        sMoveRecord move = r->moves[i];
        printf("第%2d步：(%d,%d)→(%d,%d)\n", i + 1,
               move.x, move.y, move.new_x, move.new_y);
        
        // 直接模擬移動，不進行規則檢查
        uint8_t piece = replay->board[move.x][move.y];
        uint8_t target = replay->board[move.new_x][move.new_y];
        
        // 處理吃子
        if (target != EMPTY) {
            bool target_is_red = is_red_piece(target);
            if (target_is_red) {
                for (int j = 0; j < 16; j++) {
                    if (replay->black_captured[j] == 0) {
                        replay->black_captured[j] = target;
                        break;
                    }
                }
            } else {
                for (int j = 0; j < 16; j++) {
                    if (replay->red_captured[j] == 0) {
                        replay->red_captured[j] = target;
                        break;
                    }
                }
            }
        }
        
        // 移動棋子
        replay->board[move.new_x][move.new_y] = piece;
        replay->board[move.x][move.y] = EMPTY;
        
        // 切換回合
        replay->is_red_turn = !replay->is_red_turn;
        
        // 顯示棋盤
        printXiangqiRecord(replay);
        sleep(2);
    }
    
    freeXiangqiRecord(replay);
    return 0;
}