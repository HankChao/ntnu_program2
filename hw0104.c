#include "xiangqi.h"
#include <stdio.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");
    sXiangqiRecord* game = initXiangqiRecord();
    if (!game) {
        fprintf(stderr, "初始化失败\n");
        return 1;
    }

    printXiangqiRecord(game);

    uint8_t x, y, new_x, new_y;
    while (!game->game_over) {
        printf("输入移动 (格式: 原x 原y 新x 新y): ");
        if (scanf("%hhu %hhu %hhu %hhu", &x, &y, &new_x, &new_y) != 4) {
            fprintf(stderr, "输入格式错误\n");
            break;
        }

        if (moveXiangqiRecord(game, x, y, new_x, new_y) == -1) {
            fprintf(stderr, "移动无效\n");
            continue;
        }

        printXiangqiRecord(game);
    }

    printf("棋局回放：\n");
    printXiangqiPlay(game);

    freeXiangqiRecord(game);
    return 0;
}