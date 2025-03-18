#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "mysplit.h"

// 輔助函數：檢查字符是否為空白字符
static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

// 輔助函數：按空白字符分割
static int32_t split_by_whitespace(char ***pppStrs, const char *pStr) {
    if (pStr == NULL || pppStrs == NULL) {
        return -1;
    }
    
    size_t capacity = 10;
    size_t count = 0;
    char **strs = malloc(capacity * sizeof(char *));
    if (strs == NULL) {
        return -1;
    }
    
    const char *current = pStr;
    
    // 跳過開頭的空白字符
    while (*current && is_whitespace(*current)) {
        current++;
    }
    
    // 如果字符串為空或只有空白字符
    if (*current == '\0') {
        char *empty = malloc(1);
        if (empty == NULL) {
            free(strs);
            return -1;
        }
        empty[0] = '\0';
        strs[0] = empty;
        *pppStrs = strs;
        return 1;
    }
    
    // 處理字符串
    const char *start = current;
    while (1) {
        if (*current == '\0' || is_whitespace(*current)) {
            // 提取單詞
            size_t len = current - start;
            char *word = malloc(len + 1);
            if (word == NULL) {
                goto cleanup;
            }
            strncpy(word, start, len);
            word[len] = '\0';
            
            // 擴容檢查
            if (count >= capacity) {
                capacity *= 2;
                char **new_strs = realloc(strs, capacity * sizeof(char *));
                if (new_strs == NULL) {
                    free(word);
                    goto cleanup;
                }
                strs = new_strs;
            }
            
            strs[count++] = word;
            
            // 如果到達字符串末尾則結束
            if (*current == '\0') {
                break;
            }
            
            // 跳過連續空白字符
            while (*current && is_whitespace(*current)) {
                current++;
            }
            
            // 如果到達字符串末尾則結束
            if (*current == '\0') {
                break;
            }
            
            start = current;
        } else {
            current++;
        }
    }
    
    *pppStrs = strs;
    return count;
    
cleanup:
    for (size_t i = 0; i < count; i++) {
        free(strs[i]);
    }
    free(strs);
    return -1;
}

int32_t mysplit(char ***pppStrs, const char *pStr, const char *delim) {
    // 檢查輸入是否有效
    if (pStr == NULL || pppStrs == NULL) {
        return -1;
    }
    
    // 當 delim 為 NULL 時，按空白字符分割（類似 Python 的行為）
    if (delim == NULL) {
        return split_by_whitespace(pppStrs, pStr);
    }
    
    // 檢查分隔符是否為空字符串
    size_t delim_len = strlen(delim);
    if (delim_len == 0) {
        return -1; // 分隔符不能為空，與 Python 行為一致
    }
    
    // 初始化動態數組
    size_t capacity = 10;
    size_t count = 0;
    char **strs = malloc(capacity * sizeof(char *));
    if (strs == NULL) {
        return -1;
    }
    
    const char *current = pStr;
    const char *next;
    
    // 處理特殊情況：空字符串
    if (*current == '\0') {
        char *empty = malloc(1);
        if (empty == NULL) {
            free(strs);
            return -1;
        }
        empty[0] = '\0';
        strs[count++] = empty;
        *pppStrs = strs;
        return count;
    }
    
    // 處理整個字符串
    while (1) {
        // 查找下一個分隔符
        next = strstr(current, delim);
        
        if (next == current) {
            // 當前位置是分隔符，添加空字符串
            char *empty = malloc(1);
            if (empty == NULL) {
                goto cleanup;
            }
            empty[0] = '\0';
            
            if (count >= capacity) {
                capacity *= 2;
                char **new_strs = realloc(strs, capacity * sizeof(char *));
                if (new_strs == NULL) {
                    free(empty);
                    goto cleanup;
                }
                strs = new_strs;
            }
            
            strs[count++] = empty;
            current += delim_len;
        } else if (next != NULL) {
            // 找到分隔符，提取子字符串
            size_t substr_len = next - current;
            char *substr = malloc(substr_len + 1);
            if (substr == NULL) {
                goto cleanup;
            }
            
            strncpy(substr, current, substr_len);
            substr[substr_len] = '\0';
            
            if (count >= capacity) {
                capacity *= 2;
                char **new_strs = realloc(strs, capacity * sizeof(char *));
                if (new_strs == NULL) {
                    free(substr);
                    goto cleanup;
                }
                strs = new_strs;
            }
            
            strs[count++] = substr;
            current = next + delim_len;
        } else {
            // 沒有更多分隔符，提取剩餘部分
            if (*current != '\0') {
                size_t substr_len = strlen(current);
                char *substr = malloc(substr_len + 1);
                if (substr == NULL) {
                    goto cleanup;
                }
                
                strcpy(substr, current);
                
                if (count >= capacity) {
                    capacity *= 2;
                    char **new_strs = realloc(strs, capacity * sizeof(char *));
                    if (new_strs == NULL) {
                        free(substr);
                        goto cleanup;
                    }
                    strs = new_strs;
                }
                
                strs[count++] = substr;
            }
            
            // 檢查字符串是否以分隔符結尾
            size_t str_len = strlen(pStr);
            if (str_len >= delim_len) {
                const char *end = pStr + str_len - delim_len;
                if (strcmp(end, delim) == 0) {
                    // 字符串以分隔符結尾，添加一個空字符串
                    char *empty = malloc(1);
                    if (empty == NULL) {
                        goto cleanup;
                    }
                    empty[0] = '\0';
                    
                    if (count >= capacity) {
                        capacity *= 2;
                        char **new_strs = realloc(strs, capacity * sizeof(char *));
                        if (new_strs == NULL) {
                            free(empty);
                            goto cleanup;
                        }
                        strs = new_strs;
                    }
                    
                    strs[count++] = empty;
                }
            }
            
            break;
        }
    }
    
    *pppStrs = strs;
    return count;
    
cleanup:
    for (size_t i = 0; i < count; i++) {
        free(strs[i]);
    }
    free(strs);
    return -1;
}