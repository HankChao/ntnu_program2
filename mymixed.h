#pragma once

#include <stdint.h>

// 定義混合分數結構
typedef struct _sMixedNumber {
    int64_t sign;           // 符號: 1 (正), -1 (負)
    int64_t integer;       // 整數部分
    int64_t numerator;     // 分子
    int64_t denominator;   // 分母 (始終為正)
} sMixedNumber;

// 函式原型
int32_t mixed_print(sMixedNumber *pNumber);
int32_t mixed_input(sMixedNumber *pNumber, const char *str);
int32_t mixed_add(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);
int32_t mixed_sub(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);
int32_t mixed_mul(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);
int32_t mixed_div(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);
int32_t mixed_compare(const sMixedNumber r1, const sMixedNumber r2);
