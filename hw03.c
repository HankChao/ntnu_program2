#include "mymixed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

// 定義 int128_t（假設使用 GCC/Clang 的 __int128）
typedef __int128 int128_t;

// 輔助函數：計算最大公因數 (GCD) for uint32_t
static int128_t gcd(int128_t a, int128_t b) {
    // 確保是正數
    a = (a < 0) ? -a : a;
    b = (b < 0) ? -b : b;
    
    while (b != 0) {
        int128_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// 輔助函數：化簡分數
static void simplify_fraction(int64_t *numer, int64_t *denom) {
    if (*denom == 0) {  // Invalid denominator
        *numer = 0;
        *denom = 1;
        return;
    }
    if (*numer == 0) {  // Numerator is zero, denominator reset to 1
        *denom = 1;
        return;
    }

    int128_t common = gcd(*numer, *denom);
    *numer /= common;
    *denom /= common;
}

// 輔助函數：將混合分數轉為假分數 (int128_t)
static void to_improper_fraction(const sMixedNumber *mn, int128_t *numer, int128_t *denom) {
    *denom = (int128_t)mn->denominator;
    *numer = (int128_t)mn->sign * ((int128_t)mn->integer * mn->denominator + mn->numerator);
}

// 輔助函數：將假分數轉為混合分數
static void from_improper_fraction(sMixedNumber *mn, int128_t numer, int128_t denom) {
    if (denom == 0) {
        mn->sign = 0;
        mn->integer = 0;
        mn->numerator = 0;
        mn->denominator = 1;
        return;
    }

    mn->sign = (numer * denom >= 0) ? 1 : -1;
    numer = (numer < 0) ? -numer : numer;
    denom = (denom < 0) ? -denom : denom;

    int128_t common = gcd(numer, denom);
    numer /= common;
    denom /= common;

    int128_t whole = numer / denom;
    int128_t remainder = numer % denom;

    if (whole > INT64_MAX || remainder > INT64_MAX || denom > INT64_MAX) {
        mn->sign = 0;
        mn->integer = 0;
        mn->numerator = 0;
        mn->denominator = 1;
        return;
    }

    mn->integer = (int64_t)whole;
    mn->numerator = (int64_t)remainder;
    mn->denominator = (int64_t)denom;

    if (mn->numerator == 0) {
        mn->denominator = 1;
    }
}

// 列印混合分數
int32_t mixed_print(sMixedNumber *pNumber) {
    if (pNumber == NULL || pNumber->denominator == 0) return -1;

    if (pNumber->sign == -1) printf("-");

    if (pNumber->numerator == 0) {  // 純整數
        printf("%ld\n", pNumber->integer);
    } else if (pNumber->integer > 0) {  // 整數加分數
        printf("%ld\\frac{%ld}{%ld}\n", 
               pNumber->integer, pNumber->numerator, pNumber->denominator);
    } else {  // 純分數
        printf("\\frac{%ld}{%ld}\n", 
               pNumber->numerator, pNumber->denominator);
    }

    return 0;
}

// 輸入混合分數（支援分母為負）
int32_t mixed_input(sMixedNumber *pNumber, const char *str) {
    if (pNumber == NULL || str == NULL) return -1;

    pNumber->sign = 1;
    pNumber->integer = 0;
    pNumber->numerator = 0;
    pNumber->denominator = 1;

    const char *ptr = str;

    // 處理整體負號
    if (*ptr == '-') {
        pNumber->sign = -1;
        ptr++;
    }

    // 檢查是否有分數部分 ("frac")
    if (strstr(ptr, "frac")) {
        int64_t integer_part = 0;
        int64_t numer = 0, denom = 1;
        int matched = 0;

        // 嘗試解析整數+分數部分
        matched = sscanf(ptr, "%ld\\frac{%ld}{%ld}",
                         &integer_part, &numer, &denom);
        if (matched == 3) {
            // 成功解析「整數+分數」模式
        } else {
            integer_part = 0; // 如果沒有整數部分則設為0
            matched = sscanf(ptr, "\\frac{%ld}{%ld}",
                             &numer, &denom);
            if (matched != 2) return -1; // 無法解析純分數
        }

        if (denom == 0) return -1; // 分母不能為0

        // 處理分子負數（負號轉移）
        if (numer < 0) {
            pNumber->sign *= -1;
            numer = -numer;
        }

        // 處理分母負數（負號轉移）
        if (denom < 0) {
            pNumber->sign *= -1;
            denom = -denom;
        }

        pNumber->integer = integer_part;
        pNumber->numerator = numer;
        pNumber->denominator = denom;

        // 呼叫 simplify_fraction 約分（傳入int64_t版本）
        simplify_fraction(&pNumber->numerator, &pNumber->denominator);

        return 0;
    } else { // 處理純整數模式
        int matched = sscanf(ptr, "%ld", &pNumber->integer);
        if (matched == 1) {
            return 0;
        } else {
            return -1; // 無法解析整數
        }
    }

    return -1; // 其他非預期錯誤
}


// 加法
int32_t mixed_add(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1; // Invalid input
    }
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    int128_t common_denom = denom1 * denom2;
    int128_t sum_numer = numer1 * denom2 + numer2 * denom1;

    from_improper_fraction(pNumber, sum_numer, common_denom);
    simplify_fraction(&pNumber->numerator, &pNumber->denominator);
    return 0;
}

// 減法
int32_t mixed_sub(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1; // Invalid input
    }
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    int128_t common_denom = denom1 * denom2;
    int128_t diff_numer = numer1 * denom2 - numer2 * denom1;

    from_improper_fraction(pNumber, diff_numer, common_denom);
    simplify_fraction(&pNumber->numerator, &pNumber->denominator);
    return 0;
}

// 乘法
int32_t mixed_mul(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1; // Invalid input
    }
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    int128_t prod_numer = numer1 * numer2;
    int128_t prod_denom = denom1 * denom2;

    from_improper_fraction(pNumber, prod_numer, prod_denom);
    simplify_fraction(&pNumber->numerator, &pNumber->denominator);
    return 0;
}

// 除法
int32_t mixed_div(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1; // Invalid input
    }
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    if (numer2 == 0) return -1; // Check division by zero

    int128_t div_numer = numer1 * denom2;
    int128_t div_denom = denom1 * numer2;

    if (div_denom < 0) {
        div_denom = -div_denom;
        div_numer = -div_numer;
    }

    from_improper_fraction(pNumber, div_numer, div_denom);
    simplify_fraction(&pNumber->numerator, &pNumber->denominator);
    return 0;
}

// 比較
int32_t mixed_compare(const sMixedNumber r1, const sMixedNumber r2) {
    if (r1.denominator == 0 || r2.denominator == 0) {
        return -2; // Invalid input
    }
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    int128_t common_denom = denom1 * denom2 / gcd(denom1, denom2);
    int128_t adj_numer1 = numer1 * (common_denom / denom1);
    int128_t adj_numer2 = numer2 * (common_denom / denom2);

    if (adj_numer1 > adj_numer2) return 1;
    if (adj_numer1 < adj_numer2) return -1;
    return 0;
}