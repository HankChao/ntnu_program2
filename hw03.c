#include "mymixed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 定義 int128_t (使用 GCC/Clang 的 __int128)
typedef __int128 int128_t;

// 最大輸入長度限制
#define MAX_INPUT_LENGTH 1024

// 輔助函數：計算最大公因數 (GCD)
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
    if (*denom == 0) {  // 無效的分母
        *numer = 0;
        *denom = 1;
        return;
    }
    
    if (*numer == 0) {  // 分子為零，將分母設為 1
        *denom = 1;
        return;
    }

    // 取絕對值並找出最大公因數
    int64_t abs_numer = (*numer >= 0) ? *numer : -(*numer);
    int64_t abs_denom = (*denom >= 0) ? *denom : -(*denom);
    int128_t common = gcd(abs_numer, abs_denom);
    
    *numer /= common;
    *denom /= common;
    
    // 確保分母為正
    if (*denom < 0) {
        *numer = -(*numer);
        *denom = -(*denom);
    }
}

// 輔助函數：將混合分數轉為假分數
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

    // 處理符號
    mn->sign = (numer * denom >= 0) ? 1 : -1;
    numer = (numer < 0) ? -numer : numer;
    denom = (denom < 0) ? -denom : denom;

    // 化簡分數
    int128_t common = gcd(numer, denom);
    numer /= common;
    denom /= common;

    // 提取整數部分和餘數
    int128_t whole = numer / denom;
    int128_t remainder = numer % denom;

    // 檢查溢出
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

    // 如果分子為 0，將分母設為 1
    if (mn->numerator == 0) {
        mn->denominator = 1;
    }
}

// 列印混合分數
int32_t mixed_print(sMixedNumber *pNumber) {
    if (pNumber == NULL || pNumber->denominator == 0) {
        return -1;
    }

    // 處理負號
    if (pNumber->sign == -1) {
        printf("-");
    }

    if (pNumber->numerator == 0) {
        // 純整數
        printf("%ld", pNumber->integer);
    } else if (pNumber->integer == 0) {
        // 純分數
        printf("\\frac{%ld}{%ld}", pNumber->numerator, pNumber->denominator);
    } else {
        // 混合分數
        printf("%ld\\frac{%ld}{%ld}", 
               pNumber->integer, pNumber->numerator, pNumber->denominator);
    }

    return 0;
}

// 解析混合分數字符串
int32_t mixed_input(sMixedNumber *pNumber, const char *str) {
    if (pNumber == NULL || str == NULL) {
        return -1;
    }
    
    // 檢查輸入長度
    if (strlen(str) > MAX_INPUT_LENGTH) {
        return -1;
    }

    // 初始化結果
    pNumber->sign = 1;
    pNumber->integer = 0;
    pNumber->numerator = 0;
    pNumber->denominator = 1;

    const char *ptr = str;
    
    // 跳過前導空白
    while (isspace(*ptr)) ptr++;
    
    // 處理負號
    if (*ptr == '-') {
        pNumber->sign = -1;
        ptr++;
    }

    // 確認還有字符可處理
    if (*ptr == '\0') {
        return -1;
    }

    // 檢查是否有分數部分 ("\frac")
    const char *frac_ptr = strstr(ptr, "\\frac");
    if (frac_ptr != NULL) {
        // 解析整數部分（如果存在）
        if (frac_ptr != ptr) {
            char int_part[MAX_INPUT_LENGTH];
            size_t int_len = frac_ptr - ptr;
            if (int_len >= MAX_INPUT_LENGTH) {
                return -1;
            }
            
            strncpy(int_part, ptr, int_len);
            int_part[int_len] = '\0';
            
            // 解析整數
            char *endptr;
            pNumber->integer = strtol(int_part, &endptr, 10);
            
            // 檢查整數後是否有非法字符
            while (*endptr) {
                if (!isspace(*endptr)) {
                    return -1;
                }
                endptr++;
            }
        } else {
            pNumber->integer = 0;
        }

        // 解析分數部分：\frac{分子}{分母}
        const char *num_start = strchr(frac_ptr, '{');
        if (num_start == NULL) return -1;
        num_start++;  // 跳過 '{'
        
        const char *num_end = strchr(num_start, '}');
        if (num_end == NULL) return -1;
        
        const char *denom_start = strchr(num_end, '{');
        if (denom_start == NULL) return -1;
        denom_start++;  // 跳過 '{'
        
        const char *denom_end = strchr(denom_start, '}');
        if (denom_end == NULL) return -1;
        
        // 檢查括號後是否有非法字符
        const char *end_ptr = denom_end + 1;
        while (*end_ptr) {
            if (!isspace(*end_ptr)) {
                return -1;
            }
            end_ptr++;
        }

        // 解析分子和分母
        char num_str[MAX_INPUT_LENGTH], denom_str[MAX_INPUT_LENGTH];
        
        size_t num_len = num_end - num_start;
        size_t denom_len = denom_end - denom_start;
        
        if (num_len >= MAX_INPUT_LENGTH || denom_len >= MAX_INPUT_LENGTH) {
            return -1;
        }
        
        strncpy(num_str, num_start, num_len);
        num_str[num_len] = '\0';
        
        strncpy(denom_str, denom_start, denom_len);
        denom_str[denom_len] = '\0';
        
        // 轉換為數字
        char *endptr_num, *endptr_denom;
        int64_t numerator = strtol(num_str, &endptr_num, 10);
        int64_t denominator = strtol(denom_str, &endptr_denom, 10);
        
        // 檢查分子和分母的格式
        if (*endptr_num != '\0' || *endptr_denom != '\0') {
            return -1;
        }
        
        // 檢查分母不為零
        if (denominator == 0) {
            return -1;
        }
        
        // 處理負數
        if (numerator < 0) {
            pNumber->sign *= -1;
            numerator = -numerator;
        }
        
        if (denominator < 0) {
            pNumber->sign *= -1;
            denominator = -denominator;
        }
        
        pNumber->numerator = numerator;
        pNumber->denominator = denominator;
        
        // 化簡分數
        simplify_fraction(&pNumber->numerator, &pNumber->denominator);
        
        return 0;
    } else {
        // 處理純整數
        char *endptr;
        pNumber->integer = strtol(ptr, &endptr, 10);
        
        // 檢查是否有非法字符
        while (*endptr) {
            if (!isspace(*endptr)) {
                return -1;
            }
            endptr++;
        }
        
        pNumber->numerator = 0;
        pNumber->denominator = 1;
        
        return 0;
    }
}

// 加法
int32_t mixed_add(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1;
    }
    
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    // 通分並相加
    int128_t common_denom = denom1 * denom2;
    int128_t sum_numer = numer1 * denom2 + numer2 * denom1;

    // 轉換回混合分數
    from_improper_fraction(pNumber, sum_numer, common_denom);
    
    return 0;
}

// 減法
int32_t mixed_sub(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1;
    }
    
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    // 通分並相減
    int128_t common_denom = denom1 * denom2;
    int128_t diff_numer = numer1 * denom2 - numer2 * denom1;

    // 轉換回混合分數
    from_improper_fraction(pNumber, diff_numer, common_denom);
    
    return 0;
}

// 乘法
int32_t mixed_mul(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1;
    }
    
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    // 相乘
    int128_t prod_numer = numer1 * numer2;
    int128_t prod_denom = denom1 * denom2;

    // 轉換回混合分數
    from_improper_fraction(pNumber, prod_numer, prod_denom);
    
    return 0;
}

// 除法
int32_t mixed_div(sMixedNumber *pNumber, const sMixedNumber r1, const sMixedNumber r2) {
    if (pNumber == NULL || r1.denominator == 0 || r2.denominator == 0) {
        return -1;
    }
    
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    // 檢查除數不為零
    if (numer2 == 0) {
        return -1;
    }

    // 相除 (a/b ÷ c/d = a*d / b*c)
    int128_t div_numer = numer1 * denom2;
    int128_t div_denom = denom1 * numer2;

    // 確保分母為正
    if (div_denom < 0) {
        div_numer = -div_numer;
        div_denom = -div_denom;
    }

    // 轉換回混合分數
    from_improper_fraction(pNumber, div_numer, div_denom);
    
    return 0;
}

// 比較
int32_t mixed_compare(const sMixedNumber r1, const sMixedNumber r2) {
    if (r1.denominator == 0 || r2.denominator == 0) {
        return -2; // 無效輸入
    }
    
    int128_t numer1, denom1, numer2, denom2;
    to_improper_fraction(&r1, &numer1, &denom1);
    to_improper_fraction(&r2, &numer2, &denom2);

    // 通分後比較
    int128_t common_denom = denom1 * denom2;
    int128_t adj_numer1 = numer1 * denom2;
    int128_t adj_numer2 = numer2 * denom1;

    if (adj_numer1 > adj_numer2) return 1;
    if (adj_numer1 < adj_numer2) return -1;
    return 0;
}