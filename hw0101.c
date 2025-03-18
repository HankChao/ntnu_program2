#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "mystring.h"

// 定義顏色宏
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

// 測試 mystrchr
void test_mystrchr(void) {
    const char *s = "Hello, world!";
    int c1 = 'l';
    int c2 = 'z';
    int c3 = '\0';

    printf(BLUE "=== Testing mystrchr ===\n" RESET);
    char *std1 = strchr(s, c1);
    char *my1 = mystrchr(s, c1);
    printf(YELLOW "mystrchr: Searching for '%c' in \"%s\"\n" RESET, c1, s);
    printf("  Standard strchr: %s\n", std1 ? std1 : "NULL");
    printf("  My mystrchr   : %s\n", my1 ? my1 : "NULL");

    char *std2 = strchr(s, c2);
    char *my2 = mystrchr(s, c2);
    printf(YELLOW "mystrchr: Searching for '%c' in \"%s\"\n" RESET, c2, s);
    printf("  Standard strchr: %s\n", std2 ? std2 : "NULL");
    printf("  My mystrchr   : %s\n", my2 ? my2 : "NULL");

    char *std3 = strchr(s, c3);
    char *my3 = mystrchr(s, c3);
    printf(YELLOW "mystrchr: Searching for '\\0' in \"%s\"\n" RESET, s);
    printf("  Standard strchr: %s\n", std3 ? std3 : "NULL");
    printf("  My mystrchr   : %s\n", my3 ? my3 : "NULL");

    // 測試非法參數：傳入 NULL 作為字串
    printf(YELLOW "mystrchr: Testing illegal parameter: s = NULL\n" RESET);
    // 以下測試可根據實作決定是否註解，避免真的 crash
    // char *illegal = mystrchr(NULL, 'a');
    // printf("  My mystrchr(NULL, 'a') returned: %s\n", illegal ? illegal : "NULL or error");

    printf("\n");
}

// 測試 mystrrchr
void test_mystrrchr(void) {
    const char *s = "Hello, world!";
    int c1 = 'l';
    int c2 = 'z';
    int c3 = '\0';

    printf(BLUE "=== Testing mystrrchr ===\n" RESET);
    char *std1 = strrchr(s, c1);
    char *my1 = mystrrchr(s, c1);
    printf(YELLOW "mystrrchr: Searching for '%c' in \"%s\"\n" RESET, c1, s);
    printf("  Standard strrchr: %s\n", std1 ? std1 : "NULL");
    printf("  My mystrrchr   : %s\n", my1 ? my1 : "NULL");

    char *std2 = strrchr(s, c2);
    char *my2 = mystrrchr(s, c2);
    printf(YELLOW "mystrrchr: Searching for '%c' in \"%s\"\n" RESET, c2, s);
    printf("  Standard strrchr: %s\n", std2 ? std2 : "NULL");
    printf("  My mystrrchr   : %s\n", my2 ? my2 : "NULL");

    char *std3 = strrchr(s, c3);
    char *my3 = mystrrchr(s, c3);
    printf(YELLOW "mystrrchr: Searching for '\\0' in \"%s\"\n" RESET, s);
    printf("  Standard strrchr: %s\n", std3 ? std3 : "NULL");
    printf("  My mystrrchr   : %s\n", my3 ? my3 : "NULL");

    // 測試非法參數：傳入 NULL 作為字串
    printf(YELLOW "mystrrchr: Testing illegal parameter: s = NULL\n" RESET);
    // char *illegal = mystrrchr(NULL, 'a');
    // printf("  My mystrrchr(NULL, 'a') returned: %s\n", illegal ? illegal : "NULL or error");

    printf("\n");
}

// 測試 mystrspn
void test_mystrspn(void) {
    printf(BLUE "=== Testing mystrspn ===\n" RESET);
    const char *s1 = "abcdefg";
    const char *accept1 = "abc";
    size_t std1 = strspn(s1, accept1);
    size_t my1 = mystrspn(s1, accept1);
    printf(YELLOW "mystrspn: strspn(\"%s\", \"%s\") = %zu\n" RESET, s1, accept1, std1);
    printf("          mystrspn(\"%s\", \"%s\") = %zu\n", s1, accept1, my1);

    const char *s2 = "xyzabc";
    const char *accept2 = "abc";
    size_t std2 = strspn(s2, accept2);
    size_t my2 = mystrspn(s2, accept2);
    printf(YELLOW "mystrspn: strspn(\"%s\", \"%s\") = %zu\n" RESET, s2, accept2, std2);
    printf("          mystrspn(\"%s\", \"%s\") = %zu\n", s2, accept2, my2);

    const char *s3 = "";
    size_t std3 = strspn(s3, accept1);
    size_t my3 = mystrspn(s3, accept1);
    printf(YELLOW "mystrspn: strspn(\"%s\", \"%s\") = %zu\n" RESET, s3, accept1, std3);
    printf("          mystrspn(\"%s\", \"%s\") = %zu\n", s3, accept1, my3);

    // 測試非法參數：傳入 NULL 作為 s 或 accept
    printf(YELLOW "mystrspn: Testing illegal parameter: accept = NULL\n" RESET);
    // 如果你的實作對非法參數有檢查，可以這樣測試：
    // size_t my_illegal = mystrspn(s1, NULL);
    // printf("          mystrspn(\"%s\", NULL) = %zu\n", s1, my_illegal);

    printf("\n");
}

// 測試 mystrcspn
void test_mystrcspn(void) {
    printf(BLUE "=== Testing mystrcspn ===\n" RESET);
    const char *s1 = "abcdefg";
    const char *reject1 = "de";
    size_t std1 = strcspn(s1, reject1);
    size_t my1 = mystrcspn(s1, reject1);
    printf(YELLOW "mystrcspn: strcspn(\"%s\", \"%s\") = %zu\n" RESET, s1, reject1, std1);
    printf("           mystrcspn(\"%s\", \"%s\") = %zu\n", s1, reject1, my1);

    const char *s2 = "abcdefg";
    const char *reject2 = "xyz";
    size_t std2 = strcspn(s2, reject2);
    size_t my2 = mystrcspn(s2, reject2);
    printf(YELLOW "mystrcspn: strcspn(\"%s\", \"%s\") = %zu\n" RESET, s2, reject2, std2);
    printf("           mystrcspn(\"%s\", \"%s\") = %zu\n", s2, reject2, my2);

    const char *reject3 = "";
    size_t std3 = strcspn(s1, reject3);
    size_t my3 = mystrcspn(s1, reject3);
    printf(YELLOW "mystrcspn: strcspn(\"%s\", \"%s\") = %zu\n" RESET, s1, reject3, std3);
    printf("           mystrcspn(\"%s\", \"%s\") = %zu\n", s1, reject3, my3);

    // 測試非法參數
    printf(YELLOW "mystrcspn: Testing illegal parameter: reject = NULL\n" RESET);
    // size_t my_illegal = mystrcspn(s1, NULL);
    // printf("           mystrcspn(\"%s\", NULL) = %zu\n", s1, my_illegal);

    printf("\n");
}

// 測試 mystrpbrk
void test_mystrpbrk(void) {
    printf(BLUE "=== Testing mystrpbrk ===\n" RESET);
    const char *s1 = "Hello, world!";
    const char *accept1 = "aeiou";
    char *std1 = strpbrk(s1, accept1);
    char *my1 = mystrpbrk(s1, accept1);
    printf(YELLOW "mystrpbrk: strpbrk(\"%s\", \"%s\") = %s\n" RESET, s1, accept1, std1 ? std1 : "NULL");
    printf("           mystrpbrk(\"%s\", \"%s\") = %s\n", s1, accept1, my1 ? my1 : "NULL");

    const char *accept2 = "xyz";
    char *std2 = strpbrk(s1, accept2);
    char *my2 = mystrpbrk(s1, accept2);
    printf(YELLOW "mystrpbrk: strpbrk(\"%s\", \"%s\") = %s\n" RESET, s1, accept2, std2 ? std2 : "NULL");
    printf("           mystrpbrk(\"%s\", \"%s\") = %s\n", s1, accept2, my2 ? my2 : "NULL");

    // 測試非法參數
    printf(YELLOW "mystrpbrk: Testing illegal parameter: s = NULL\n" RESET);
    // char *illegal = mystrpbrk(NULL, accept1);
    // printf("           mystrpbrk(NULL, \"%s\") = %s\n", accept1, illegal ? illegal : "NULL or error");
    printf(YELLOW "mystrpbrk: Testing illegal parameter: accept = NULL\n" RESET);
    // illegal = mystrpbrk(s1, NULL);
    // printf("           mystrpbrk(\"%s\", NULL) = %s\n", s1, illegal ? illegal : "NULL or error");

    printf("\n");
}

// 測試 mystrstr
void test_mystrstr(void) {
    printf(BLUE "=== Testing mystrstr ===\n" RESET);
    const char *haystack = "The quick brown fox jumps over the lazy dog.";
    const char *needle1 = "brown";
    const char *needle2 = "cat";
    const char *needle3 = "";

    char *std1 = strstr(haystack, needle1);
    char *my1 = mystrstr(haystack, needle1);
    printf(YELLOW "mystrstr: strstr(\"%s\", \"%s\") = %s\n" RESET, haystack, needle1, std1 ? std1 : "NULL");
    printf("           mystrstr(\"%s\", \"%s\") = %s\n", haystack, needle1, my1 ? my1 : "NULL");

    char *std2 = strstr(haystack, needle2);
    char *my2 = mystrstr(haystack, needle2);
    printf(YELLOW "mystrstr: strstr(\"%s\", \"%s\") = %s\n" RESET, haystack, needle2, std2 ? std2 : "NULL");
    printf("           mystrstr(\"%s\", \"%s\") = %s\n", haystack, needle2, my2 ? my2 : "NULL");

    char *std3 = strstr(haystack, needle3);
    char *my3 = mystrstr(haystack, needle3);
    printf(YELLOW "mystrstr: strstr(\"%s\", \"%s\") = %s\n" RESET, haystack, needle3, std3 ? std3 : "NULL");
    printf("           mystrstr(\"%s\", \"%s\") = %s\n", haystack, needle3, my3 ? my3 : "NULL");

    // 測試非法參數
    printf(YELLOW "mystrstr: Testing illegal parameter: haystack = NULL\n" RESET);
    // char *illegal = mystrstr(NULL, needle1);
    // printf("           mystrstr(NULL, \"%s\") = %s\n", needle1, illegal ? illegal : "NULL or error");
    printf(YELLOW "mystrstr: Testing illegal parameter: needle = NULL\n" RESET);
    // illegal = mystrstr(haystack, NULL);
    // printf("           mystrstr(\"%s\", NULL) = %s\n", haystack, illegal ? illegal : "NULL or error");

    printf("\n");
}

// 測試 mystrtok
void test_mystrtok(void) {
    printf(BLUE "=== Testing mystrtok ===\n" RESET);
    {
        // 測試非法參數：傳入 NULL 作為初始字串
        printf(YELLOW "Test Case Illegal Parameter for mystrtok:\n" RESET);
        printf("Calling mystrtok(NULL, \",\")\n");
        // char *illegal_token = mystrtok(NULL, ",");
        // if (!illegal_token)
        //     printf("  Returned NULL (or error message displayed)\n");
        // else
        //     printf("  Returned token: (ignored)\n");
        printf("\n");
    }
    {
        // 測試非法參數：傳入 NULL 作為分割字串
        char inputIllegal[] = "This,is,a,test,string";
        printf(YELLOW "Test Case Illegal Parameter for mystrtok:\n" RESET);
        printf("Calling mystrtok(%s, NULL)\n", inputIllegal);
        // char *illegal_token = mystrtok(inputIllegal, NULL);
        // if (!illegal_token)
        //     printf("  Returned NULL (or error message displayed)\n");
        // else
        //     printf("  Returned token: (ignored)\n");
        printf("\n");
    }
    {
        // 測試範例1：簡單分隔
        char input1[] = "This,is,a,test,string";
        char copy1_std[sizeof(input1)];
        char copy1_my[sizeof(input1)];
        strcpy(copy1_std, input1);
        strcpy(copy1_my, input1);

        printf(YELLOW "Test Case 1:\n" RESET);
        printf("Input: \"%s\", delim=\",\" \n", input1);
        
        // 使用標準 strtok
        printf(BLUE "Standard strtok:\n" RESET);
        int idx = 0;
        char *token = strtok(copy1_std, ",");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = strtok(NULL, ",");
        }
        
        // 使用自製 mystrtok
        printf(BLUE "My mystrtok:\n" RESET);
        idx = 0;
        token = mystrtok(copy1_my, ",");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = mystrtok(NULL, ",");
        }

        printf("\n");
    }

    {
        // 測試範例2：連續分隔符與空 token
        char input2[] = ";;Hello;;World;;";
        char copy2_std[sizeof(input2)];
        char copy2_my[sizeof(input2)];
        strcpy(copy2_std, input2);
        strcpy(copy2_my, input2);

        printf(YELLOW "Test Case 2:\n" RESET);
        printf("Input: \"%s\", delim=\";\" \n", input2);
        
        printf(BLUE "Standard strtok:\n" RESET);
        int idx = 0;
        char *token = strtok(copy2_std, ";");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = strtok(NULL, ";");
        }
        
        printf(BLUE "My mystrtok:\n" RESET);
        idx = 0;
        token = mystrtok(copy2_my, ";");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = mystrtok(NULL, ";");
        }
        printf("\n");
    }

    {
        // 測試範例3：空字串
        char input3[] = "";
        char copy3_std[sizeof(input3)];
        char copy3_my[sizeof(input3)];
        strcpy(copy3_std, input3);
        strcpy(copy3_my, input3);

        printf(YELLOW "Test Case 3:\n" RESET);
        printf("Input: \"%s\", delim=\",\" \n", input3);
        
        printf(BLUE "Standard strtok:\n" RESET);
        int idx = 0;
        char *token = strtok(copy3_std, ",");
        if (!token) {
            printf("  No tokens (NULL returned)\n");
        } else {
            while (token) {
                printf("  Token %d: \"%s\"\n", idx++, token);
                token = strtok(NULL, ",");
            }
        }
        
        printf(BLUE "My mystrtok:\n" RESET);
        idx = 0;
        token = mystrtok(copy3_my, ",");
        if (!token) {
            printf("  No tokens (NULL returned)\n");
        } else {
            while (token) {
                printf("  Token %d: \"%s\"\n", idx++, token);
                token = mystrtok(NULL, ",");
            }
        }
        printf("\n");
    }

    {
        // 測試範例4：連續分隔符導致空 token
        char input4[] = "one,,,two,,three,";
        char copy4_std[sizeof(input4)];
        char copy4_my[sizeof(input4)];
        strcpy(copy4_std, input4);
        strcpy(copy4_my, input4);

        printf(YELLOW "Test Case 4:\n" RESET);
        printf("Input: \"%s\", delim=\",\" \n", input4);
        
        printf(BLUE "Standard strtok:\n" RESET);
        int idx = 0;
        char *token = strtok(copy4_std, ",");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = strtok(NULL, ",");
        }
        
        printf(BLUE "My mystrtok:\n" RESET);
        idx = 0;
        token = mystrtok(copy4_my, ",");
        while (token) {
            printf("  Token %d: \"%s\"\n", idx++, token);
            token = mystrtok(NULL, ",");
        }
        printf("\n");
    }
}

int main(void) {
    printf(BLUE "==== Testing Standard C String Functions vs. My String Library ====\n\n" RESET);

    test_mystrchr();
    test_mystrrchr();
    test_mystrspn();
    test_mystrcspn();
    test_mystrpbrk();
    test_mystrstr();
    test_mystrtok();

    return 0;
}
