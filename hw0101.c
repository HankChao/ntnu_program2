#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "mystring.h"

// 顯示測試是否通過的輔助函式
void test_result(const char* test_name, int result) {
    printf("%-40s %s\n", test_name, result ? "通過" : "失敗");
}

// 測試 mystrchr 函式
void test_mystrchr() {
    printf("\n=== 測試 mystrchr 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "Hello, World!";
    test_result("尋找存在的字元", mystrchr(str1, 'o') == strchr(str1, 'o'));
    test_result("尋找第一個出現的字元", mystrchr(str1, 'o') == &str1[4]);
    test_result("尋找不存在的字元", mystrchr(str1, 'z') == NULL);
    
    // 特殊案例
    test_result("尋找空字符 (\\0)", mystrchr(str1, '\0') == &str1[13]);
    test_result("在空字串中尋找", mystrchr("", 'a') == NULL);
    test_result("在空字串中尋找空字符", mystrchr("", '\0') == strchr("", '\0'));
}

// 測試 mystrrchr 函式
void test_mystrrchr() {
    printf("\n=== 測試 mystrrchr 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "Hello, World!";
    test_result("尋找存在的字元", mystrrchr(str1, 'o') == strrchr(str1, 'o'));
    test_result("尋找最後一個出現的字元", mystrrchr(str1, 'o') == &str1[8]);
    test_result("尋找不存在的字元", mystrrchr(str1, 'z') == NULL);
    
    // 特殊案例
    test_result("尋找空字符 (\\0)", mystrrchr(str1, '\0') == &str1[13]);
    test_result("字元只出現一次", mystrrchr(str1, '!') == &str1[12]);
    test_result("在空字串中尋找空字符", mystrrchr("", '\0') == strrchr("", '\0'));
}

// 測試 mystrspn 函式
void test_mystrspn() {
    printf("\n=== 測試 mystrspn 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "12345abc789";
    test_result("只匹配開頭部分字元", mystrspn(str1, "0123456789") == 5);
    test_result("匹配所有字元", mystrspn("12345", "0123456789") == 5);
    test_result("無匹配字元", mystrspn("abcde", "0123456789") == 0);
    
    // 特殊案例
    test_result("空字串的匹配", mystrspn("", "0123456789") == 0);
    test_result("空接受集合", mystrspn("12345", "") == 0);
    test_result("與標準函式比較", mystrspn(str1, "0123456789") == strspn(str1, "0123456789"));
}

// 測試 mystrcspn 函式
void test_mystrcspn() {
    printf("\n=== 測試 mystrcspn 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "abcde12345";
    test_result("只排除後面部分字元", mystrcspn(str1, "0123456789") == 5);
    test_result("排除所有字元", mystrcspn("12345", "0123456789") == 0);
    test_result("無排除字元", mystrcspn("abcde", "0123456789") == 5);
    
    // 特殊案例
    test_result("空字串的排除", mystrcspn("", "0123456789") == 0);
    test_result("空排除集合", mystrcspn("12345", "") == 5);
    test_result("與標準函式比較", mystrcspn(str1, "0123456789") == strcspn(str1, "0123456789"));
}

// 測試 mystrpbrk 函式
void test_mystrpbrk() {
    printf("\n=== 測試 mystrpbrk 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "Hello, World!";
    test_result("尋找存在的字元", mystrpbrk(str1, "aeiou") == strpbrk(str1, "aeiou"));
    test_result("尋找第一個匹配字元", mystrpbrk(str1, "aeiou") == &str1[1]);  // 'e' in Hello
    test_result("尋找不存在的字元", mystrpbrk(str1, "xyz") == NULL);
    
    // 特殊案例
    test_result("空字串中尋找", mystrpbrk("", "aeiou") == NULL);
    test_result("空接受集合", mystrpbrk(str1, "") == NULL);
}

// 測試 mystrstr 函式
void test_mystrstr() {
    printf("\n=== 測試 mystrstr 函式 ===\n");
    
    // 一般測試案例
    char str1[] = "Hello, World!";
    test_result("尋找存在的子字串", mystrstr(str1, "World") == strstr(str1, "World"));
    test_result("尋找存在的子字串(位置檢查)", mystrstr(str1, "World") == &str1[7]);
    test_result("尋找不存在的子字串", mystrstr(str1, "Earth") == NULL);
    
    // 特殊案例
    test_result("尋找空子字串", mystrstr(str1, "") == str1);
    test_result("在空字串中尋找", mystrstr("", "World") == NULL);
    test_result("子字串比主字串長", mystrstr("Hi", "Hello") == NULL);
    test_result("完全相同的字串", mystrstr("Hello", "Hello") == strstr("Hello", "Hello"));
    test_result("重疊子字串", mystrstr("ababc", "abc") == &"ababc"[2]);
}

// 測試 mystrtok 函式
void test_mystrtok() {
    printf("\n=== 測試 mystrtok 函式 ===\n");
    
    // 測試基本分割功能
    char str1[] = "Hello,World,Programming";
    char str1_copy[] = "Hello,World,Programming";
    char *token1, *token2;
    
    // 第一次呼叫
    token1 = mystrtok(str1, ",");
    token2 = strtok(str1_copy, ",");
    test_result("第一次呼叫", strcmp(token1, token2) == 0);
    
    // 第二次呼叫
    token1 = mystrtok(NULL, ",");
    token2 = strtok(NULL, ",");
    test_result("第二次呼叫", strcmp(token1, token2) == 0);
    
    // 第三次呼叫
    token1 = mystrtok(NULL, ",");
    token2 = strtok(NULL, ",");
    test_result("第三次呼叫", strcmp(token1, token2) == 0);
    
    // 測試多個分隔符號
    char str2[] = "apple:banana;orange,grape";
    char str2_copy[] = "apple:banana;orange,grape";
    
    token1 = mystrtok(str2, ":;,");
    token2 = strtok(str2_copy, ":;,");
    test_result("多個分隔符號-第一次", strcmp(token1, token2) == 0);
    
    // 測試連續分隔符號
    char str3[] = "abc:::def";
    char str3_copy[] = "abc:::def";
    
    token1 = mystrtok(str3, ":");
    token2 = strtok(str3_copy, ":");
    test_result("連續分隔符號-第一次", strcmp(token1, token2) == 0);
    
    token1 = mystrtok(NULL, ":");
    token2 = strtok(NULL, ":");
    test_result("連續分隔符號-第二次", strcmp(token1, token2) == 0);
}

// 主函式
int main() {
    printf("=== 自訂字串函式庫測試 ===\n");
    
    // 執行各項測試
    test_mystrchr();
    test_mystrrchr();
    test_mystrspn();
    test_mystrcspn();
    test_mystrpbrk();
    test_mystrstr();
    test_mystrtok();
    
    printf("\n所有測試完成！\n");
    return 0;
}
