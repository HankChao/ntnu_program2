#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mysplit.h"  // 假設 mysplit.h 包含 mysplit 函數的聲明

// 輔助函數：釋放分割後的字符串數組
void free_split_result(char **strs, int32_t count) {
    if (strs != NULL) {
        for (int32_t i = 0; i < count; i++) {
            free(strs[i]);
        }
        free(strs);
    }
}

// 輔助函數：打印分割後的字符串數組
void print_split_result(char **strs, int32_t count) {
    if (count < 0) {
        printf("Error: mysplit returned -1\n");
    } else {
        printf("Split result (count = %d):\n", count);
        for (int32_t i = 0; i < count; i++) {
            printf("[%d] \"%s\"\n", i, strs[i]);
        }
    }
}

// 測試用例 1: 正常分割
void test_normal_split() {
    const char *pStr = "the value of pi is 3.14.";
    const char *delim = " ";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 2: 多字符分隔符
void test_multi_char_delim() {
    const char *pStr = "a..b..c";
    const char *delim = "..";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 3: 開頭和結尾有分隔符
void test_leading_trailing_delim() {
    const char *pStr = "..a..b..";
    const char *delim = "..";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 4: 空字符串
void test_empty_string() {
    const char *pStr = "";
    const char *delim = " ";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 5: 無分隔符
void test_no_delim() {
    const char *pStr = "abc";
    const char *delim = " ";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 6: 連續分隔符
void test_consecutive_delim() {
    const char *pStr = "a,,b,,c";
    const char *delim = ",";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 7: NULL 輸入
void test_null_input() {
    char **strs = NULL;
    int32_t count = mysplit(&strs, NULL, " ");
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 測試用例 8: 空分隔符
void test_empty_delim() {
    const char *pStr = "abc";
    const char *delim = "";
    char **strs = NULL;
    int32_t count = mysplit(&strs, pStr, delim);
    print_split_result(strs, count);
    free_split_result(strs, count);
}

// 主函數：運行所有測試用例
int main() {
    printf("Test 1: Normal split\n");
    test_normal_split();
    printf("\n");

    printf("Test 2: Multi-character delimiter\n");
    test_multi_char_delim();
    printf("\n");

    printf("Test 3: Leading and trailing delimiters\n");
    test_leading_trailing_delim();
    printf("\n");

    printf("Test 4: Empty string\n");
    test_empty_string();
    printf("\n");

    printf("Test 5: No delimiter\n");
    test_no_delim();
    printf("\n");

    printf("Test 6: Consecutive delimiters\n");
    test_consecutive_delim();
    printf("\n");

    printf("Test 7: NULL input\n");
    test_null_input();
    printf("\n");

    printf("Test 8: Empty delimiter\n");
    test_empty_delim();
    printf("\n");

    return 0;
}