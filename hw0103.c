#include "mymixed.h"
#include <stdio.h>
#include <string.h>

// 將混合分數轉成字串格式 (供測試使用)
int32_t mixed_to_string(const sMixedNumber *pNumber, char *buffer, size_t size) {
    if (!pNumber || !buffer || size == 0) return -1;

    int len = 0;
    if (pNumber->sign == -1) len += snprintf(buffer + len, size - len, "-");

    if (pNumber->integer > 0 && pNumber->numerator > 0)
        len += snprintf(buffer + len, size - len, "%u\\frac{%u}{%u}",
                        pNumber->integer, pNumber->numerator, pNumber->denominator);
    else if (pNumber->integer > 0)
        len += snprintf(buffer + len, size - len, "%u", pNumber->integer);
    else if (pNumber->numerator > 0)
        len += snprintf(buffer + len, size - len, "\\frac{%u}{%u}",
                        pNumber->numerator, pNumber->denominator);
    else
        len += snprintf(buffer + len, size - len, "0");

    return 0;
}

// 測試測資 1: 加法
void test_mixed_add() {
    sMixedNumber num1, num2, result;
    mixed_input(&num1, "1\\frac{2}{3}");
    mixed_input(&num2, "-\\frac{1}{2}");
    mixed_add(&result, num1, num2);

    char expected[] = "1\\frac{1}{6}";
    char actual[32];
    mixed_to_string(&result, actual, 32);
    printf("測資1: %s (預期: %s) -> %s\n",
           actual, expected, strcmp(actual, expected) == 0 ? "通過" : "失敗");
}

// 測試測資 2: 乘法
void test_mixed_mul() {
    sMixedNumber num1, num2, result;
    mixed_input(&num1, "-\\frac{3}{4}");
    mixed_input(&num2, "2\\frac{1}{2}");
    mixed_mul(&result, num1, num2);

    char expected[] = "-1\\frac{7}{8}";
    char actual[32];
    mixed_to_string(&result, actual, 32);
    printf("測資2: %s (預期: %s) -> %s\n",
           actual, expected, strcmp(actual, expected) == 0 ? "通過" : "失敗");
}

// 測試測資 3: 除法
void test_mixed_div() {
    sMixedNumber num1, num2, result;
    mixed_input(&num1, "\\frac{-1}{2}");
    mixed_input(&num2, "\\frac{-1}{3}");
    mixed_div(&result, num1, num2);

    char expected[] = "1\\frac{1}{2}";
    char actual[32];
    mixed_to_string(&result, actual, 32);
    printf("測資3: %s (預期: %s) -> %s\n",
           actual, expected, strcmp(actual, expected) == 0 ? "通過" : "失敗");
}

// 測試測資 4: 比較
void test_mixed_compare() {
    sMixedNumber num1, num2;
    mixed_input(&num1, "1\\frac{1}{2}");
    mixed_input(&num2, "1\\frac{2}{3}");
    int32_t comp = mixed_compare(num1, num2);

    printf("測資4: %d (預期: -1) -> %s\n", comp, comp == -1 ? "通過" : "失敗");
}

// 測試測資 5: 無效輸入 (分母為0)
void test_invalid_input() {
    sMixedNumber num1;
    int32_t ret = mixed_input(&num1, "1\\frac{2}{0}");

    printf("測資5: %d (預期: -1) -> %s\n", ret, ret == -1 ? "通過" : "失敗");
}

// 測試測資 6: 除以0
void test_div_by_zero() {
    sMixedNumber num1, num2, result;
    mixed_input(&num1, "1\\frac{1}{2}");
    mixed_input(&num2, "0");
    int32_t ret = mixed_div(&result, num1, num2);

    printf("測資6: %d (預期: -1) -> %s\n", ret, ret == -1 ? "通過" : "失敗");
}

// 主程式
int main() {
    test_mixed_add();
    test_mixed_mul();
    test_mixed_div();
    test_mixed_compare();
    test_invalid_input();
    test_div_by_zero();
    return 0;
}
