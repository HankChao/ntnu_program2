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

// 前置聲明，因為check_under_attack需要用到這些函數
static bool is_chariot_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y);
static bool is_cannon_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y);
static bool is_horse_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y);
static bool is_soldier_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y);

// 檢查某個位置是否受到攻擊
static bool check_under_attack(sXiangqiRecord* r, int x, int y, bool by_red) {
    // 檢查是否受到車的攻擊
    uint8_t enemy_chariot = by_red ? RED_CHARIOT : BLACK_CHARIOT;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_chariot && is_chariot_attack(r, i, j, x, y)) {
                return true;
            }
        }
    }
    
    // 檢查是否受到炮的攻擊
    uint8_t enemy_cannon = by_red ? RED_CANNON : BLACK_CANNON;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_cannon && is_cannon_attack(r, i, j, x, y)) {
                return true;
            }
        }
    }
    
    // 檢查是否受到馬的攻擊
    uint8_t enemy_horse = by_red ? RED_HORSE : BLACK_HORSE;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_horse && is_horse_attack(r, i, j, x, y)) {
                return true;
            }
        }
    }
    
    // 檢查是否受到兵/卒的攻擊
    uint8_t enemy_soldier = by_red ? RED_SOLDIER : BLACK_SOLDIER;
    for (int i = 0; i < BOARD_ROWS; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_soldier && is_soldier_attack(r, i, j, x, y)) {
                return true;
            }
        }
    }
    
    return false;
}

// 檢查車是否可以攻擊到某位置
static bool is_chariot_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y) {
    // 車必須在同一行或同一列
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
    // 檢查中間是否有阻擋
    int step;
    if (from_x == to_x) {  // 水平移動
        step = (to_y > from_y) ? 1 : -1;
        for (int j = from_y + step; j != to_y; j += step) {
            if (r->board[from_x][j] != EMPTY) {
                return false;
            }
        }
    } else {  // 垂直移動
        step = (to_x > from_x) ? 1 : -1;
        for (int i = from_x + step; i != to_x; i += step) {
            if (r->board[i][from_y] != EMPTY) {
                return false;
            }
        }
    }
    
    return true;
}

// 檢查炮是否可以攻擊到某位置
static bool is_cannon_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y) {
    // 炮必須在同一行或同一列
    if (from_x != to_x && from_y != to_y) {
        return false;
    }
    
    // 目標位置必須有棋子
    if (r->board[to_x][to_y] == EMPTY) {
        return false;
    }
    
    // 檢查中間棋子數量
    int count = 0;
    int step;
    if (from_x == to_x) {  // 水平方向
        step = (to_y > from_y) ? 1 : -1;
        for (int j = from_y + step; j != to_y; j += step) {
            if (r->board[from_x][j] != EMPTY) {
                count++;
            }
        }
    } else {  // 垂直方向
        step = (to_x > from_x) ? 1 : -1;
        for (int i = from_x + step; i != to_x; i += step) {
            if (r->board[i][from_y] != EMPTY) {
                count++;
            }
        }
    }
    
    // 炮需要一個炮架才能攻擊
    return count == 1;
}

// 檢查馬是否可以攻擊到某位置
static bool is_horse_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y) {
    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);
    
    // 馬走日字
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        return false;
    }
    
    // 檢查蹩馬腿
    int block_x = from_x, block_y = from_y;
    if (dx == 2) {
        block_x = from_x + ((to_x > from_x) ? 1 : -1);
    } else {  // dy == 2
        block_y = from_y + ((to_y > from_y) ? 1 : -1);
    }
    
    return r->board[block_x][block_y] == EMPTY;
}

// 檢查兵/卒是否可以攻擊到某位置
static bool is_soldier_attack(sXiangqiRecord* r, int from_x, int from_y, int to_x, int to_y) {
    uint8_t p = r->board[from_x][from_y];
    bool red = is_red_piece(p);
    int dx = to_x - from_x;  // 注意方向
    int dy = abs(to_y - from_y);
    
    if (red) {  // 紅兵
        if (from_x < 5) {  // 未過河
            return dx == 1 && dy == 0;  // 只能往前一步
        } else {  // 已過河
            return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);  // 可以往前或左右
        }
    } else {  // 黑卒
        if (from_x > 4) {  // 未過河
            return dx == -1 && dy == 0;  // 只能往前（下）一步
        } else {  // 已過河
            return (dx == -1 && dy == 0) || (dx == 0 && dy == 1);  // 可以往前或左右
        }
    }
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

    // 臨時移動並檢查
    uint8_t temp = r->board[nx][ny];
    r->board[nx][ny] = p;
    r->board[x][y] = EMPTY;

    bool valid = true;
    
    // 王見王檢查
    uint8_t enemy_general = red ? BLACK_GENERAL : RED_GENERAL;
    int ex = -1, ey = -1;
    
    // 尋找敵方將/帥位置
    for (int i = 0; i < BOARD_ROWS && ex == -1; i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == enemy_general) {
                ex = i;
                ey = j;
                break;
            }
        }
    }
    
    // 檢查是否王見王
    if (ex != -1 && ny == ey) {
        int start = (nx < ex) ? nx + 1 : ex + 1;
        int end = (nx < ex) ? ex - 1 : nx - 1;
        bool hasBlock = false;
        
        for (int i = start; i <= end; i++) {
            if (r->board[i][ny] != EMPTY) {
                hasBlock = true;
                break;
            }
        }
        
        if (!hasBlock) {
            snprintf(r->last_error, sizeof(r->last_error), "將軍對臉");
            valid = false;
        }
    }
    
    // 檢查自殺：移動後是否會被攻擊
    if (valid && check_under_attack(r, nx, ny, !red)) {
        snprintf(r->last_error, sizeof(r->last_error), "%s自殺", red ? "紅帥" : "黑將");
        valid = false;
    }
    
    // 恢復棋盤
    r->board[x][y] = p;
    r->board[nx][ny] = temp;
    
    return valid;
}

static bool validate_advisor(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    uint8_t p = r->board[x][y];
    bool red = is_red_piece(p);

    // 檢查目標位置是否在九宮格內
    if (red && !is_in_red_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "紅仕不能離開九宮格");
        return false;
    }
    if (!red && !is_in_black_palace(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "黑士不能離開九宮格");
        return false;
    }

    // 仕/士只能斜走一步
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

    // 象/相不能過河
    if (red && nx > 4) {
        snprintf(r->last_error, sizeof(r->last_error), "紅象不能過河");
        return false;
    }
    if (!red && nx < 5) {
        snprintf(r->last_error, sizeof(r->last_error), "黑象不能過河");
        return false;
    }

    // 象/相只能走田字
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
    // 車只能直線移動
    if (x != nx && y != ny) {
        snprintf(r->last_error, sizeof(r->last_error), "車必須直線移動");
        return false;
    }

    // 檢查路徑上是否有阻擋
    int step;
    if (x == nx) {  // 水平移動
        step = (ny > y) ? 1 : -1;
        for (int j = y + step; j != ny; j += step) {
            if (r->board[x][j] != EMPTY) {
                snprintf(r->last_error, sizeof(r->last_error), "車移動路徑被擋");
                return false;
            }
        }
    } else {  // 垂直移動
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
    // 馬只能日字形移動
    int dx = abs(nx - x), dy = abs(ny - y);
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) {
        snprintf(r->last_error, sizeof(r->last_error), "馬必須走日字");
        return false;
    }

    // 檢查蹩馬腿
    int block_x = x, block_y = y;
    if (dx == 2) {
        block_x = x + ((nx > x) ? 1 : -1);
    } else {  // dy == 2
        block_y = y + ((ny > y) ? 1 : -1);
    }
    
    if (r->board[block_x][block_y] != EMPTY) {
        snprintf(r->last_error, sizeof(r->last_error), "馬腿被擋");
        return false;
    }
    return true;
}

static bool validate_cannon(sXiangqiRecord* r, int x, int y, int nx, int ny) {
    // 炮只能直線移動
    if (x != nx && y != ny) {
        snprintf(r->last_error, sizeof(r->last_error), "炮必須直線移動");
        return false;
    }

    // 計算路徑上的棋子數量
    int count = 0;
    int step;
    
    if (x == nx) {  // 水平移動
        step = (ny > y) ? 1 : -1;
        for (int j = y + step; j != ny; j += step) {
            if (r->board[x][j] != EMPTY) {
                count++;
            }
        }
    } else {  // 垂直移動
        step = (nx > x) ? 1 : -1;
        for (int i = x + step; i != nx; i += step) {
            if (r->board[i][y] != EMPTY) {
                count++;
            }
        }
    }

    // 炮移動或吃子的規則檢查
    if (r->board[nx][ny] == EMPTY) {  // 移動
        if (count != 0) {
            snprintf(r->last_error, sizeof(r->last_error), "炮移動需無棋子");
            return false;
        }
    } else {  // 吃子
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
    int dx = nx - x;  // 注意：這裡不取絕對值，因為兵/卒的前進方向有關
    int dy = abs(ny - y);

    if (red) {  // 紅兵
        if (x < 5) {  // 未過河：只能前進
            if (dx != 1 || dy != 0) {
                snprintf(r->last_error, sizeof(r->last_error), "紅兵未過河只能前進");
                return false;
            }
        } else {  // 已過河：可以前進或左右移動，但不能後退
            if (dx < 0) {  // 不能後退
                snprintf(r->last_error, sizeof(r->last_error), "紅兵不可後退");
                return false;
            }
            if ((dx == 1 && dy == 1) || dx > 1 || dy > 1) {  // 不能斜走或前進/左右超過一步
                snprintf(r->last_error, sizeof(r->last_error), "紅兵移動超過一步或斜走");
                return false;
            }
        }
    } else {  // 黑卒
        if (x > 4) {  // 未過河：只能前進（向下）
            if (dx != -1 || dy != 0) {
                snprintf(r->last_error, sizeof(r->last_error), "黑卒未過河只能前進");
                return false;
            }
        } else {  // 已過河：可以前進或左右移動，但不能後退
            if (dx > 0) {  // 不能後退
                snprintf(r->last_error, sizeof(r->last_error), "黑卒不可後退");
                return false;
            }
            if ((dx == -1 && dy == 1) || dx < -1 || dy > 1) {  // 不能斜走或前進/左右超過一步
                snprintf(r->last_error, sizeof(r->last_error), "黑卒移動超過一步或斜走");
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
static void check_game_over(sXiangqiRecord* r) {
    // 檢查將/帥是否仍然存在
    bool red_general_exists = false;
    bool black_general_exists = false;
    
    for (int i = 0; i < BOARD_ROWS && (!red_general_exists || !black_general_exists); i++) {
        for (int j = 0; j < BOARD_COLS; j++) {
            if (r->board[i][j] == RED_GENERAL) {
                red_general_exists = true;
            } else if (r->board[i][j] == BLACK_GENERAL) {
                black_general_exists = true;
            }
            
            if (red_general_exists && black_general_exists) {
                break;
            }
        }
    }
    
    // 如果任一方的將/帥不存在，遊戲結束
    if (!red_general_exists || !black_general_exists) {
        r->game_over = true;
    }
}

int32_t moveXiangqiRecord(sXiangqiRecord* r, uint8_t x, uint8_t y, uint8_t nx, uint8_t ny) {
    if (!r) {
        return -1;
    }

    memset(r->last_error, 0, sizeof(r->last_error));

    if (r->game_over) {
        snprintf(r->last_error, sizeof(r->last_error), "遊戲已結束");
        return -1;
    }
    
    if (!is_valid_position(x, y) || !is_valid_position(nx, ny)) {
        snprintf(r->last_error, sizeof(r->last_error), "無效座標 (%d,%d)→(%d,%d)", x, y, nx, ny);
        return -1;
    }

    if (x == nx && y == ny) {
        snprintf(r->last_error, sizeof(r->last_error), "起點和終點相同");
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
        // 不能吃自己的棋子
        if (is_red_piece(target) == is_red) {
            snprintf(r->last_error, sizeof(r->last_error), "不可吃己方棋子");
            return -1;
        }
        
        // 不允許吃將/帥
        if (target == RED_GENERAL || target == BLACK_GENERAL) {
            snprintf(r->last_error, sizeof(r->last_error), "不能吃將");
            return -1;
        }
    }

    // 根據棋子類型驗證移動規則
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

    if (!valid) {
        return -1;
    }

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

    // 處理俘虜
    if (target != EMPTY) {
        if (is_red) { // 紅方吃子
            for (int i = 0; i < 16; i++) {
                if (r->red_captured[i] == 0) {
                    r->red_captured[i] = target;
                    break;
                }
            }
        } else { // 黑方吃子
            for (int i = 0; i < 16; i++) {
                if (r->black_captured[i] == 0) {
                    r->black_captured[i] = target;
                    break;
                }
            }
        }
    }

    // 執行移動
    r->board[nx][ny] = piece;
    r->board[x][y] = EMPTY;

    // 檢查遊戲結束
    check_game_over(r);

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

    printf("\n當前回合：\033[1;33m%s方\033[0m\n", r->is_red_turn ? "紅" : "黑");
    printf("俘虜棋子：\n");
    
    printf("紅方俘虜：");
    for (int i = 0; i < 16; i++) {
        if (r->red_captured[i])
            printf("\033[1;34m%s\033[0m ", pieces[r->red_captured[i]]);
    }
    
    printf("\n黑方俘虜：");
    for (int i = 0; i < 16; i++) {
        if (r->black_captured[i])
            printf("\033[1;31m%s\033[0m ", pieces[r->black_captured[i]]);
    }

    // 棋盤標題
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
        printf("\n\033[1;31m錯誤：%s\033[0m\n", r->last_error);
    }

    return 0;
}

// === 棋局重播 ===============================================
int32_t printXiangqiPlay(sXiangqiRecord* r) {
    if (!r) return -1;
    
    // 建立新的棋局用於重播
    sXiangqiRecord* replay = initXiangqiRecord();
    if (!replay) return -1;
    
    printf("\n開始重播棋局，共 %d 步\n", r->move_count);
    printXiangqiRecord(replay);  // 顯示初始棋盤
    sleep(1);  // 暫停1秒
    
    for (uint32_t i = 0; i < r->move_count; i++) {
        sMoveRecord move = r->moves[i];
        printf("\n第 %d 步：(%d,%d)→(%d,%d)\n", i + 1,
               move.x, move.y, move.new_x, move.new_y);
        
        // 取得棋子
        uint8_t piece = replay->board[move.x][move.y];
        uint8_t target = replay->board[move.new_x][move.new_y];
        
        // 處理吃子
        if (target != EMPTY) {
            bool is_red = is_red_piece(piece);
            if (is_red) {
                for (int j = 0; j < 16; j++) {
                    if (replay->red_captured[j] == 0) {
                        replay->red_captured[j] = target;
                        break;
                    }
                }
            } else {
                for (int j = 0; j < 16; j++) {
                    if (replay->black_captured[j] == 0) {
                        replay->black_captured[j] = target;
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
        sleep(2);  // 暫停2秒
    }
    
    // 釋放資源
    freeXiangqiRecord(replay);
    return 0;
}