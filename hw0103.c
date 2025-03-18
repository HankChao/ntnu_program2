#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mymixed.h"

// 輔助函式：印出測試標題與預期結果，再呼叫 mixed_print 印出實際結果
void test_mixed_input(const char *testName, const char *input, const char *expected) {
    sMixedNumber num;
    int32_t ret = mixed_input(&num, input);
    printf("\x1b[34m=== Test %s ===\x1b[0m\n", testName);
    printf("Input: \"%s\"\n", input);
    printf("Expected output: %s\n", expected);
    if (ret != 0) {
        printf("Result: Error\n");
    } else {
        printf("Actual output: ");
        mixed_print(&num);
        printf("\n");
    }
    printf("\n");
}

// 輔助函式：測試運算（加、減、乘、除）
void test_mixed_arithmetic(const char *testName, const char *input1, const char *input2,
                           const char *op, const char *expected) {
    sMixedNumber r1, r2, result;
    int32_t ret1 = mixed_input(&r1, input1);
    int32_t ret2 = mixed_input(&r2, input2);
    printf("\x1b[34m=== Arithmetic Test %s ===\x1b[0m\n", testName);
    printf("Operand 1: \"%s\"\n", input1);
    printf("Operand 2: \"%s\"\n", input2);
    printf("Operation: %s\n", op);
    if (ret1 != 0 || ret2 != 0) {
        printf("Error in input parsing (ret1=%d, ret2=%d)\n", ret1, ret2);
        printf("\n");
        return;
    }
    int32_t ret = -1;
    if (strcmp(op, "+") == 0) {
        ret = mixed_add(&result, r1, r2);
    } else if (strcmp(op, "-") == 0) {
        ret = mixed_sub(&result, r1, r2);
    } else if (strcmp(op, "*") == 0) {
        ret = mixed_mul(&result, r1, r2);
    } else if (strcmp(op, "/") == 0) {
        ret = mixed_div(&result, r1, r2);
    }
    printf("Expected result: %s\n", expected);
    if (ret != 0) {
        printf("Operation error (ret = %d)\n", ret);
    } else {
        printf("Actual result: ");
        mixed_print(&result);
        printf("\n");
    }
    printf("\n");
}

// 輔助函式：測試混合數比較
void test_mixed_compare(const char *testName, const char *input1, const char *input2, int expected) {
    sMixedNumber r1, r2;
    int32_t ret1 = mixed_input(&r1, input1);
    int32_t ret2 = mixed_input(&r2, input2);
    printf("\x1b[34m=== Compare Test %s ===\x1b[0m\n", testName);
    printf("Operand 1: \"%s\"\n", input1);
    printf("Operand 2: \"%s\"\n", input2);
    if (ret1 != 0 || ret2 != 0) {
        printf("Error in input parsing (ret1=%d, ret2=%d)\n", ret1, ret2);
    } else {
        int cmp = mixed_compare(r1, r2);
        printf("Expected compare result: %d\n", expected);
        printf("Actual compare result: %d\n", cmp);
    }
    printf("\n");
}

int main(void) {
    printf("==== Testing Mixed Fraction Arithmetic Library ====\n\n");

    // ======== Mixed Input Tests ========
    // 1. 正常分數輸入
    test_mixed_input("Normal Fraction", "1\\frac{2}{3}", "1\\frac{2}{3}");

    // 2. 包含負號（整數部分）的分數輸入
    test_mixed_input("Negative Integer Part", "-1\\frac{2}{3}", "-1\\frac{2}{3}");

    // 3. 包含負號（分子）的分數輸入
    test_mixed_input("Negative Numerator", "\\frac{-2}{3}", "-\\frac{2}{3}");

    // 3.5 包含負號（分母）的分數輸入
    test_mixed_input("Negative Denomitor", "\\frac{2}{-3}", "-\\frac{2}{3}");

    // 4. 包含負號但整數位未輸入 (例如 -\frac{2}{3})
    test_mixed_input("No Integer, Negative in Fraction", "-\\frac{2}{3}", "-\\frac{2}{3}");

    // 5. 分子、分母數字很大 (使用 int32_t 極限值)
    test_mixed_input("Large Numbers", "2147483647\\frac{123456789}{987654321}", "2147483647\\frac{13717421}{109739369}"); // 預期結果視化簡後格式而定

    // 6. 輸入格式錯誤 (缺少反斜線)
    test_mixed_input("Malformed Input", "1frac{2}{3}", "Error");

    // 6.5 輸入格式錯誤 (分數後接無關字元)
    test_mixed_input("Malformed Input", "1\\frac{2}{3}nonsence", "Error");

    // 6.6 輸入格式錯誤 (分數後接無關字元)
    test_mixed_input("Malformed Input", "\\frac{2}{3}nonsence", "Error");

    // 7. 輸入字串過長（假設超出規定長度返回錯誤）
    // 這裡用一個非常長的字串模擬，實際上可能依照規範返回 -1
    {
        char longInput[4096];
        memset(longInput, ' ', sizeof(longInput)-1);
        longInput[0] = '1';
        // 模擬一個長字串 (此處僅作示例，實際狀況依實作而定)
        longInput[sizeof(longInput)-2] = '3';
        longInput[sizeof(longInput)-1] = '\0';
        test_mixed_input("Overly Long Input", longInput, "Error");
    }

    // ======== Arithmetic Tests ========
    // 8. 正常分數加減乘除
    // 1\frac{1}{2} = 1 + 1/2, 2\frac{1}{3} = 2 + 1/3
    // 加法: 3/2 + 7/3 = 23/6 = 3\frac{5}{6}
    test_mixed_arithmetic("Addition", "1\\frac{1}{2}", "2\\frac{1}{3}", "+", "3\\frac{5}{6}");
    // 減法: 3/2 - 7/3 = -5/6  → 輸出格式預期："\frac{-5}{6}" (或 "-0\\frac{5}{6}" 視實作而定)
    test_mixed_arithmetic("Subtraction", "1\\frac{1}{2}", "2\\frac{1}{3}", "-", "-\\frac{5}{6}");
    // 乘法: (3/2)*(7/3) = 7/2 = 3\\frac{1}{2}
    test_mixed_arithmetic("Multiplication", "1\\frac{1}{2}", "2\\frac{1}{3}", "*", "3\\frac{1}{2}");
    // 除法: (3/2) / (7/3) = 9/14
    test_mixed_arithmetic("Division", "1\\frac{1}{2}", "2\\frac{1}{3}", "/", "\\frac{9}{14}");

    // 9. 包含負號的分數加減乘除
    test_mixed_arithmetic("Addition with Negative", "-1\\frac{1}{2}", "2\\frac{1}{3}", "+", "\\frac{5}{6}");
    test_mixed_arithmetic("Subtraction with Negative", "2\\frac{1}{3}", "-1\\frac{1}{2}", "-", "3\\frac{5}{6}");
    test_mixed_arithmetic("Multiplication with Negative", "-1\\frac{1}{2}", "2\\frac{1}{3}", "*", "-3\\frac{1}{2}");
    test_mixed_arithmetic("Division with Negative", "-1\\frac{1}{2}", "2\\frac{1}{3}", "/", "-\\frac{9}{14}");

    // 10. 非常大數字的加減乘除 (僅作大數測試，預期結果依化簡後格式)
    test_mixed_arithmetic("Large Addition", "2147483646\\frac{2147483646}{2147483647}", "1", "+", "2147483647\\frac{2147483646}{2147483647}");
    test_mixed_arithmetic("Large Multiplication", "536870912\\frac{1073741824}{2147483647}", "2", "*", "1073741825\\frac{1}{2147483647}");

    // ======== Compare Tests ========
    test_mixed_compare("Compare Equal", "1\\frac{2}{3}", "1\\frac{2}{3}", 0);
    test_mixed_compare("Compare Greater", "2\\frac{1}{3}", "1\\frac{2}{3}", 1);
    test_mixed_compare("Compare Less", "1\\frac{2}{3}", "2\\frac{1}{3}", -1);

    return 0;
}
