#ifndef MYMIXED_H
#define MYMIXED_H

#include <stdint.h>

typedef struct _sMixedNumber {
    int64_t sign;         // 1 表示正數，-1 表示負數
    int64_t integer;      // 整數部分
    int64_t numerator;    // 分子
    int64_t denominator;  // 分母
} sMixedNumber;

// 打印混合分數
// 返回值：成功 0，失敗 -1
int32_t mixed_print(sMixedNumber *pNumber);

// 從字符串輸入混合分數
// 返回值：成功 0，失敗 -1
int32_t mixed_input(sMixedNumber *pNumber, const char *str);

// 加法：pNumber = r1 + r2
// 返回值：成功 0，失敗 -1
int32_t mixed_add(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);

// 減法：pNumber = r1 - r2
// 返回值：成功 0，失敗 -1
int32_t mixed_sub(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);

// 乘法：pNumber = r1 * r2
// 返回值：成功 0，失敗 -1
int32_t mixed_mul(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);

// 除法：pNumber = r1 / r2
// 返回值：成功 0，失敗 -1
int32_t mixed_div(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2);

// 比較：r1 與 r2 的大小
// 返回值：r1 > r2 返回 1，r1 = r2 返回 0，r1 < r2 返回 -1，錯誤返回 -2
int32_t mixed_compare(const sMixedNumber r1, const sMixedNumber r2);

#endif /* MYMIXED_H */