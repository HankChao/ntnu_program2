#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "mysplit.h"

int32_t mysplit(char ***pppStrs, const char *pStr, const char *delim) {
    // 檢查輸入是否有效
    if (pStr == NULL || delim == NULL || pppStrs == NULL) {
        return -1;
    }
    size_t len = strlen(delim);
    if (len == 0) {
        return -1; // 分隔符不能為空
    }

    // 初始化動態數組，用於存儲分割後的字符串
    size_t capacity = 10; // 初始容量
    size_t count = 0;    // 當前字符串數量
    char **strs = malloc(capacity * sizeof(char *));
    if (strs == NULL) {
        return -1; // 內存分配失敗
    }

    // 從輸入字符串的開頭開始處理
    const char *current = pStr;
    while (1) {
        // 查找下一個分隔符的位置
        const char *next = strstr(current, delim);

        if (next == current) {
            // 分隔符在當前位置，添加空字符串
            char *empty = malloc(1);
            if (empty == NULL) {
                goto cleanup;
            }
            empty[0] = '\0';
            strs[count++] = empty;
            current += len;
        } else if (next != NULL) {
            // 找到分隔符，提取 current 到 next 之間的子字符串
            size_t substr_len = next - current;
            char *substr = malloc(substr_len + 1);
            if (substr == NULL) {
                goto cleanup;
            }
            strncpy(substr, current, substr_len);
            substr[substr_len] = '\0';
            strs[count++] = substr;
            current = next + len;
        } else {
            // 沒有更多分隔符，提取剩餘部分
            size_t substr_len = strlen(current);
            char *substr = malloc(substr_len + 1);
            if (substr == NULL) {
                goto cleanup;
            }
            strcpy(substr, current);
            strs[count++] = substr;
            break;
        }

        // 如果數組滿了，擴容
        if (count == capacity) {
            capacity *= 2;
            char **new_strs = realloc(strs, capacity * sizeof(char *));
            if (new_strs == NULL) {
                goto cleanup;
            }
            strs = new_strs;
        }
    }

    // 將結果賦值給 pppStrs
    *pppStrs = strs;
    return count;

cleanup:
    // 清理內存並返回錯誤
    for (size_t i = 0; i < count; i++) {
        free(strs[i]);
    }
    free(strs);
    return -1;
}