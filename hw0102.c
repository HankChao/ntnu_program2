#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mysplit.h"

// 輔助函式，針對每個測試案例呼叫 mysplit 並印出 token 結果
void test_mysplit(const char *caseName, const char *input, const char *delim) {
    char **tokens = NULL;
    int32_t num = mysplit(&tokens, input, delim);
    printf("%s:\n", caseName);
    printf("Input: \"%s\"\n", input);
    printf("Delimiter: \"%s\"\n", delim);
    if (num < 0) {
        printf("mysplit error\n");
    } else {
        printf("Output: [");
        for (int32_t i = 0; i < num; i++) {
            printf("\"%s\"", tokens[i]);
            if (i < num - 1)
                printf(", ");
            free(tokens[i]);
        }
        printf("]\n");
    }
    free(tokens);
    printf("\n");
}

int main(void) {
    // 範例 1: 原始例子，分隔符是空白
    test_mysplit("Case 1", "The value of pi is 3.14.", " ");

    // 範例 2: 原始例子，分隔符是句點
    test_mysplit("Case 2", "The value of pi is 3.14.", ".");

    // 範例 3: 空字串
    test_mysplit("Case 3", "", ",");

    // 範例 4: 字串全部都是分隔符（例如只有空白）
    test_mysplit("Case 4", "     ", " ");

    // 範例 5: 字串中沒有分隔符
    test_mysplit("Case 5", "HelloWorld", ",");

    // 範例 6: 連續分隔符導致空 token
    test_mysplit("Case 6", "one,,,two,,three,", ",");

    // 範例 7: 多字元分隔符
    test_mysplit("Case 7", "apple--banana--cherry", "--");

    // 範例 8: 分隔符是空字串
    test_mysplit("Case 8", "abcd", "");

    // 範例 9: 分隔符是 NULL
    test_mysplit("Case 9", "abc", NULL);

    return 0;
}
