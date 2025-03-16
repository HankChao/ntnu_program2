#include "xiangqi.h"
#include <stdio.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, ""); // 支持中文字符
    sXiangqiRecord* game = initXiangqiRecord();
    if (!game) {
        fprintf(stderr, "初始化失敗\n");
        return 1;
    }

    printXiangqiRecord(game);

    uint8_t x, y, new_x, new_y;
    while (!game->game_over) {
        printf("輸入移動 (格式: 原x 原y 新x 新y): ");
        if (scanf("%hhu %hhu %hhu %hhu", &x, &y, &new_x, &new_y) != 4) {
            fprintf(stderr, "輸入格式錯誤\n");
            break;
        }

        if (moveXiangqiRecord(game, x, y, new_x, new_y) == -1) {
            fprintf(stderr, "移動無效\n");
            continue;
        }

        printXiangqiRecord(game);
    }

    printf("棋局回放：\n");
    printXiangqiPlay(game);

    freeXiangqiRecord(game);
    return 0;
}