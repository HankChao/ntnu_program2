#include "mymixed.h"
#include <stdio.h>
#include <string.h>

// 輔助函數：將混合分數轉成字串格式
int32_t mixed_to_string(const sMixedNumber *pNumber, char *buffer, size_t size) {
    if (!pNumber || !buffer || size == 0) return -1;
    int len = 0;
    if (pNumber->sign == -1) len += snprintf(buffer + len, size - len, "-");
    
    if (pNumber->numerator == 0) {  // 純整數
        len += snprintf(buffer + len, size - len, "%lld", (long long)pNumber->integer);
    } else if (pNumber->integer > 0) {  // 整數加分數
        len += snprintf(buffer + len, size - len, "%lld\\frac{%lld}{%lld}",
                        (long long)pNumber->integer,
                        (long long)pNumber->numerator,
                        (long long)pNumber->denominator);
    } else {  // 純分數
        len += snprintf(buffer + len, size - len, "\\frac{%lld}{%lld}",
                        (long long)pNumber->numerator,
                        (long long)pNumber->denominator);
    }
    return 0;
}

// 輔助函數：進行測試並顯示結果
void test_case(const char *test_name, const char *input1, const char *input2, const char *expected, int32_t (*operation)(sMixedNumber*, const sMixedNumber, const sMixedNumber)) {
    sMixedNumber num1, num2, result;
    mixed_input(&num1, input1);
    mixed_input(&num2, input2);
    int32_t ret = operation(&result, num1, num2);
    
    char actual[64];
    mixed_to_string(&result, actual, sizeof(actual));
    
    printf("測試 %s: %s %s %s = %s (預期: %s) -> %s\n",
           test_name, input1, 
           (operation == mixed_add) ? "+" : 
           (operation == mixed_sub) ? "-" : 
           (operation == mixed_mul) ? "*" : "/", 
           input2, actual, expected, 
           strcmp(actual, expected) == 0 ? "通過" : "失敗");
}

// 測試比較功能
void test_compare(const char *input1, const char *input2, int expected) {
    sMixedNumber num1, num2;
    mixed_input(&num1, input1);
    mixed_input(&num2, input2);
    int32_t comp = mixed_compare(num1, num2);
    printf("比較測試: %s %s %s -> %d (預期: %d) -> %s\n",
           input1, 
           comp == 0 ? "=" : (comp > 0 ? ">" : "<"),
           input2, comp, expected, 
           comp == expected ? "通過" : "失敗");
}

// 測試輸入功能
void test_input(const char *input, const char *expected) {
    sMixedNumber num;
    int32_t ret = mixed_input(&num, input);
    char actual[64];
    mixed_to_string(&num, actual, sizeof(actual));
    printf("輸入測試 '%s': %s (預期: %s) -> %s\n",
           input, actual, expected, 
           strcmp(actual, expected) == 0 ? "通過" : "失敗");
}

int main() {
    printf("=== 基本運算測試 ===\n");
    // 基本加法測試
    test_case("加法1", "1\\frac{3}{1}", "-\\frac{5}{1}", "-1", mixed_add);
    test_case("加法2", "\\frac{1}{3}", "\\frac{1}{6}", "\\frac{1}{2}", mixed_add);
    test_case("加法3", "-\\frac{1}{3}", "-\\frac{1}{6}", "-\\frac{1}{2}", mixed_add);
    test_case("加法3", "\\frac{0}{3}", "-\\frac{1}{6}", "-\\frac{1}{6}", mixed_add);
    test_case("加法3", "-8", "\\frac{10}{2}", "-3", mixed_add);
    test_case("加法3", "0", "0", "0", mixed_add);

    // 基本減法測試
    test_case("減法1", "1\\frac{2}{3}", "\\frac{1}{2}", "1\\frac{1}{6}", mixed_sub);
    test_case("減法2", "\\frac{1}{3}", "\\frac{1}{6}", "\\frac{1}{6}", mixed_sub);
    test_case("減法3", "-\\frac{1}{3}", "\\frac{1}{6}", "-\\frac{1}{2}", mixed_sub);
    
    // 基本乘法測試
    test_case("乘法1", "-\\frac{3}{4}", "2\\frac{1}{2}", "-1\\frac{7}{8}", mixed_mul);
    test_case("乘法2", "\\frac{2}{3}", "\\frac{3}{4}", "\\frac{1}{2}", mixed_mul);
    test_case("乘法3", "-\\frac{2}{3}", "-\\frac{3}{4}", "\\frac{1}{2}", mixed_mul);
    
    // 基本除法測試
    test_case("除法1", "\\frac{-1}{2}", "\\frac{-1}{3}", "1\\frac{1}{2}", mixed_div);
    test_case("除法2", "\\frac{2}{3}", "\\frac{1}{2}", "1\\frac{1}{3}", mixed_div);
    test_case("除法3", "-\\frac{2}{3}", "\\frac{1}{2}", "-1\\frac{1}{3}", mixed_div);
    
    printf("\n=== 邊界情況測試 ===\n");
    // 零值測試
    test_case("零值2", "\\frac{1}{2}", "0", "\\frac{1}{2}", mixed_add);
    test_case("零值3", "0", "0", "0", mixed_mul);
    
    // 大值測試 (接近32位元整數限制)
    test_case("大值1", "1000000", "2000000", "3000000", mixed_add);
    test_case("大值2", "1000000\\frac{1}{2}", "1000000\\frac{1}{2}", "2000001", mixed_add);
    
    // 複雜符號組合測試
    test_input("-\\frac{-1}{2}", "\\frac{1}{2}");
    test_input("\\frac{1}{-2}", "-\\frac{1}{2}");
    test_input("-\\frac{1}{-2}", "\\frac{1}{2}");
    
    printf("\n=== 比較測試 ===\n");
    test_compare("1\\frac{1}{2}", "1\\frac{2}{3}", -1);
    test_compare("2", "1\\frac{2}{3}", 1);
    test_compare("\\frac{2}{4}", "\\frac{1}{2}", 0);
    test_compare("-\\frac{1}{2}", "\\frac{1}{3}", -1);
    
    printf("\n=== 特殊輸入測試 ===\n");
    // 特殊格式測試
    test_input("\\frac{0}{5}", "0");
    test_input("0", "0");
    
    // 非法輸入測試 (應該返回錯誤代碼，但不會導致程式崩潰)
    sMixedNumber num;
    int32_t ret = mixed_input(&num, "1\\frac{2}{0}");
    printf("分母為零測試: 返回值 = %d (預期: -1) -> %s\n", ret, ret == -1 ? "通過" : "失敗");
    
    // 除以零測試
    sMixedNumber num1, num2, result;
    mixed_input(&num1, "1\\frac{1}{2}");
    mixed_input(&num2, "0");
    ret = mixed_div(&result, num1, num2);
    printf("除以零測試: 返回值 = %d (預期: -1) -> %s\n", ret, ret == -1 ? "通過" : "失敗");
    
    return 0;
}