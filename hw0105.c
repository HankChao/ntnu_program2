#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <strings.h>

#define MAX_INPUT_SIZE 8192
#define MAX_LINE_SIZE 1024
#define MAX_KEY_SIZE 1024  // 進一步增大
#define MAX_TABLE_NAME 256

// 資料結構定義
typedef enum {
    TYPE_STRING,
    TYPE_NUMBER,
    TYPE_BOOLEAN,
    TYPE_ARRAY,
    TYPE_OBJECT,
    TYPE_NULL
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        char* string_value;
        double number_value;
        bool boolean_value;
        struct {
            struct Value** items;
            int size;
            int capacity;
        } array;
        struct {
            char** keys;
            struct Value** values;
            int size;
            int capacity;
        } object;
    } data;
} Value;

// 函數前向聲明
void free_value(Value* value);
Value* parse_json_value(const char** json);
Value* parse_toml_value(const char** toml);
char* generate_json(Value* value);
char* generate_toml(Value* value);
void generate_toml_object(Value* object, char** buffer, int* size, int* capacity, const char* prefix);
void generate_toml_string_value(Value* value, char** buffer, int* size, int* capacity, bool in_array);

// 工具函式
bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

void skip_whitespace(const char** ptr) {
    while (is_whitespace(**ptr)) {
        (*ptr)++;
    }
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

// 值操作函式
Value* create_string_value(const char* str) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_STRING;
    value->data.string_value = strdup(str);
    return value;
}

Value* create_number_value(double num) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_NUMBER;
    value->data.number_value = num;
    return value;
}

Value* create_boolean_value(bool b) {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_BOOLEAN;
    value->data.boolean_value = b;
    return value;
}

Value* create_null_value() {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_NULL;
    return value;
}

Value* create_array_value() {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_ARRAY;
    value->data.array.items = NULL;
    value->data.array.size = 0;
    value->data.array.capacity = 0;
    return value;
}

Value* create_object_value() {
    Value* value = (Value*)malloc(sizeof(Value));
    value->type = TYPE_OBJECT;
    value->data.object.keys = NULL;
    value->data.object.values = NULL;
    value->data.object.size = 0;
    value->data.object.capacity = 0;
    return value;
}

void array_add(Value* array, Value* item) {
    if (array->data.array.size >= array->data.array.capacity) {
        int new_capacity = (array->data.array.capacity == 0) ? 8 : array->data.array.capacity * 2;
        array->data.array.items = (Value**)realloc(array->data.array.items, new_capacity * sizeof(Value*));
        array->data.array.capacity = new_capacity;
    }
    array->data.array.items[array->data.array.size++] = item;
}

void object_add(Value* object, const char* key, Value* value) {
    if (object->data.object.size >= object->data.object.capacity) {
        int new_capacity = (object->data.object.capacity == 0) ? 8 : object->data.object.capacity * 2;
        object->data.object.keys = (char**)realloc(object->data.object.keys, new_capacity * sizeof(char*));
        object->data.object.values = (Value**)realloc(object->data.object.values, new_capacity * sizeof(Value*));
        object->data.object.capacity = new_capacity;
    }
    object->data.object.keys[object->data.object.size] = strdup(key);
    object->data.object.values[object->data.object.size] = value;
    object->data.object.size++;
}

Value* object_get(Value* object, const char* key) {
    for (int i = 0; i < object->data.object.size; i++) {
        if (strcmp(object->data.object.keys[i], key) == 0) {
            return object->data.object.values[i];
        }
    }
    return NULL;
}

// 釋放資源
void free_value(Value* value) {
    if (value == NULL) return;
    
    if (value->type == TYPE_STRING) {
        free(value->data.string_value);
    } else if (value->type == TYPE_ARRAY) {
        for (int i = 0; i < value->data.array.size; i++) {
            free_value(value->data.array.items[i]);
        }
        free(value->data.array.items);
    } else if (value->type == TYPE_OBJECT) {
        for (int i = 0; i < value->data.object.size; i++) {
            free(value->data.object.keys[i]);
            free_value(value->data.object.values[i]);
        }
        free(value->data.object.keys);
        free(value->data.object.values);
    }
    
    free(value);
}

// JSON 解析函式
char* parse_json_string(const char** json) {
    (*json)++; // 跳過引號
    const char* start = *json;
    while (**json != '"' || (*(*json - 1) == '\\' && *(*json - 2) != '\\')) {
        if (**json == '\0') return NULL; // 字串未結束
        (*json)++;
    }
    
    int len = *json - start;
    char* result = (char*)malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';
    
    // 處理轉義字元
    char* dst = result;
    char* src = result;
    while (*src) {
        if (*src == '\\' && *(src + 1)) {
            src++;
            switch (*src) {
                case 'n': *dst++ = '\n'; break;
                case 'r': *dst++ = '\r'; break;
                case 't': *dst++ = '\t'; break;
                case 'b': *dst++ = '\b'; break;
                case 'f': *dst++ = '\f'; break;
                case '"': *dst++ = '"'; break;
                case '\\': *dst++ = '\\'; break;
                default: *dst++ = *src; break;
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';

    (*json)++; // 跳過結束引號
    return result;
}

Value* parse_json_value(const char** json) {
    skip_whitespace(json);
    
    if (**json == '{') {
        (*json)++;
        Value* obj = create_object_value();
        skip_whitespace(json);
        
        if (**json != '}') {
            while (1) {
                skip_whitespace(json);
                if (**json != '"') {
                    free_value(obj);
                    return NULL;
                }
                
                char* key = parse_json_string(json);
                skip_whitespace(json);
                
                if (**json != ':') {
                    free(key);
                    free_value(obj);
                    return NULL;
                }
                (*json)++;
                
                Value* value = parse_json_value(json);
                if (value == NULL) {
                    free(key);
                    free_value(obj);
                    return NULL;
                }
                
                object_add(obj, key, value);
                free(key);
                
                skip_whitespace(json);
                if (**json == '}') break;
                
                if (**json != ',') {
                    free_value(obj);
                    return NULL;
                }
                (*json)++;
            }
        }
        
        (*json)++;
        return obj;
    } else if (**json == '[') {
        (*json)++;
        Value* arr = create_array_value();
        skip_whitespace(json);
        
        if (**json != ']') {
            while (1) {
                Value* value = parse_json_value(json);
                if (value == NULL) {
                    free_value(arr);
                    return NULL;
                }
                
                array_add(arr, value);
                
                skip_whitespace(json);
                if (**json == ']') break;
                
                if (**json != ',') {
                    free_value(arr);
                    return NULL;
                }
                (*json)++;
                skip_whitespace(json);
            }
        }
        
        (*json)++;
        return arr;
    } else if (**json == '"') {
        char* str = parse_json_string(json);
        Value* value = create_string_value(str);
        free(str);
        return value;
    } else if (is_digit(**json) || **json == '-' || **json == '+') {
        char* end;
        double num = strtod(*json, &end);
        *json = end;
        return create_number_value(num);
    } else if (strncmp(*json, "true", 4) == 0) {
        *json += 4;
        return create_boolean_value(true);
    } else if (strncmp(*json, "false", 5) == 0) {
        *json += 5;
        return create_boolean_value(false);
    } else if (strncmp(*json, "null", 4) == 0) {
        *json += 4;
        return create_null_value();
    } else {
        return NULL;
    }
}

Value* parse_json(const char* json) {
    skip_whitespace(&json);
    Value* value = parse_json_value(&json);
    skip_whitespace(&json);
    if (*json != '\0') {
        free_value(value);
        return NULL;
    }
    return value;
}

// TOML 解析函式
bool is_key_char(char c) {
    return isalnum(c) || c == '-' || c == '_';
}

char* parse_toml_key(const char** toml) {
    const char* start = *toml;
    
    // 檢查是否為引號包裹的鍵
    if (**toml == '"') {
        (*toml)++;
        const char* str_start = *toml;
        while (**toml != '"' || (*(*toml - 1) == '\\' && *(*toml - 2) != '\\')) {
            if (**toml == '\0') return NULL;
            (*toml)++;
        }
        int len = *toml - str_start;
        char* result = (char*)malloc(len + 1);
        strncpy(result, str_start, len);
        result[len] = '\0';
        (*toml)++;
        return result;
    }
    
    // 普通的鍵
    while (is_key_char(**toml)) {
        (*toml)++;
    }
    
    int len = *toml - start;
    if (len == 0) return NULL;
    
    char* result = (char*)malloc(len + 1);
    strncpy(result, start, len);
    result[len] = '\0';
    return result;
}

bool is_toml_date(const char* str) {
    // 簡易判斷是否為日期格式
    return (strlen(str) > 10) && 
           isdigit(str[0]) && isdigit(str[1]) && isdigit(str[2]) && isdigit(str[3]) && 
           str[4] == '-' && 
           isdigit(str[5]) && isdigit(str[6]) && 
           str[7] == '-' && 
           isdigit(str[8]) && isdigit(str[9]) && 
           (str[10] == 'T' || str[10] == ' ');
}

char* parse_toml_string(const char** toml) {
    char quote = **toml;
    (*toml)++;  // 跳過第一個引號
    
    // 檢查是否為多行字串
    bool is_multiline = false;
    if (**toml == quote && *(*toml + 1) == quote) {
        is_multiline = true;
        (*toml) += 2;  // 跳過多出來的兩個引號
        
        // 如果多行字串後面直接是換行符，跳過它
        if (**toml == '\n') {
            (*toml)++;
        }
    }
    
    // 儲存起始位置
    const char* start = *toml;
    const char* end = start;
    
    // 尋找字串結束位置
    if (is_multiline) {
        // 安全地尋找結束的三重引號
        while (*end != '\0') {
            if (*end == quote) {
                // 確保不會越界檢查
                if (*(end + 1) != '\0' && *(end + 1) == quote &&
                    *(end + 2) != '\0' && *(end + 2) == quote) {
                    break;  // 找到結束的三重引號
                }
            }
            end++;
        }
        
        if (*end == '\0') {
            return NULL;  // 字串未正確結束
        }
        
        // 計算長度
        int len = end - start;
        char* result = (char*)malloc(len + 1);
        if (!result) return NULL;  // 記憶體分配失敗
        
        // 複製字串內容
        memcpy(result, start, len);
        result[len] = '\0';
        
        // 更新輸入指針位置
        *toml = end + 3;  // 跳過三個引號
        
        return result;
    } else {
        // 處理普通字串
        bool escaped = false;
        while (*end != '\0') {
            if (*end == '\\') {
                escaped = !escaped;
            } else if (*end == quote && !escaped) {
                break;  // 找到非轉義的結束引號
            } else {
                escaped = false;
            }
            end++;
        }
        
        if (*end == '\0') {
            return NULL;  // 未找到結束引號
        }
        
        // 計算長度
        int len = end - start;
        char* result = (char*)malloc(len + 1);
        if (!result) return NULL;  // 記憶體分配失敗
        
        // 複製字串內容
        memcpy(result, start, len);
        result[len] = '\0';
        
        // 處理轉義字元
        char* dst = result;
        char* src = result;
        
        while (*src) {
            if (*src == '\\' && *(src + 1)) {
                src++;
                switch (*src) {
                    case 'n': *dst++ = '\n'; break;
                    case 'r': *dst++ = '\r'; break;
                    case 't': *dst++ = '\t'; break;
                    case 'b': *dst++ = '\b'; break;
                    case 'f': *dst++ = '\f'; break;
                    case '"': *dst++ = '"'; break;
                    case '\\': *dst++ = '\\'; break;
                    default: *dst++ = *src; break;
                }
            } else {
                *dst++ = *src;
            }
            src++;
        }
        *dst = '\0';
        
        // 更新輸入指針位置
        *toml = end + 1;  // 跳過結束引號
        
        return result;
    }
}

Value* parse_toml_array(const char** toml) {
    (*toml)++; // 跳過 '['
    Value* arr = create_array_value();
    
    skip_whitespace(toml);
    
    if (**toml != ']') {
        while (1) {
            skip_whitespace(toml);
            Value* value = parse_toml_value(toml);
            if (value == NULL) {
                free_value(arr);
                return NULL;
            }
            
            array_add(arr, value);
            
            skip_whitespace(toml);
            if (**toml == ']') break;
            
            if (**toml != ',') {
                free_value(arr);
                return NULL;
            }
            (*toml)++;
            skip_whitespace(toml);
        }
    }
    
    (*toml)++; // 跳過 ']'
    return arr;
}

Value* parse_toml_inline_table(const char** toml) {
    (*toml)++; // 跳過 '{'
    Value* obj = create_object_value();
    
    skip_whitespace(toml);
    
    if (**toml != '}') {
        while (1) {
            skip_whitespace(toml);
            char* key = parse_toml_key(toml);
            if (key == NULL) {
                free_value(obj);
                return NULL;
            }
            
            skip_whitespace(toml);
            if (**toml != '=') {
                free(key);
                free_value(obj);
                return NULL;
            }
            (*toml)++;
            
            skip_whitespace(toml);
            Value* value = parse_toml_value(toml);
            if (value == NULL) {
                free(key);
                free_value(obj);
                return NULL;
            }
            
            object_add(obj, key, value);
            free(key);
            
            skip_whitespace(toml);
            if (**toml == '}') break;
            
            if (**toml != ',') {
                free_value(obj);
                return NULL;
            }
            (*toml)++;
            skip_whitespace(toml);
        }
    }
    
    (*toml)++; // 跳過 '}'
    return obj;
}

Value* parse_toml_value(const char** toml) {
    skip_whitespace(toml);
    
    if (**toml == '[' && *(*toml + 1) != '[') {
        return parse_toml_array(toml);
    } else if (**toml == '{') {
        return parse_toml_inline_table(toml);
    } else if (**toml == '"' || **toml == '\'') {
        char* str = parse_toml_string(toml);
        Value* value = create_string_value(str);
        free(str);
        return value;
    } else if (is_digit(**toml) || **toml == '-' || **toml == '+') {
        const char* start = *toml;
        
        if (**toml == '-' || **toml == '+') {
            (*toml)++;
        }
        
        while (is_digit(**toml) || **toml == '.' || **toml == '_' || 
               **toml == 'e' || **toml == 'E' || 
               (**toml == '-' && (*(*toml - 1) == 'e' || *(*toml - 1) == 'E')) || 
               (**toml == '+' && (*(*toml - 1) == 'e' || *(*toml - 1) == 'E'))) {
            (*toml)++;
        }
        
        int len = *toml - start;
        char* str = (char*)malloc(len + 1);
        strncpy(str, start, len);
        str[len] = '\0';
        
        // 轉換數字前去除下劃線
        char* clean_str = (char*)malloc(len + 1);
        int j = 0;
        for (int i = 0; i < len; i++) {
            if (str[i] != '_') {
                clean_str[j++] = str[i];
            }
        }
        clean_str[j] = '\0';
        
        // 檢查是否為日期
        if (is_toml_date(clean_str)) {
            Value* value = create_string_value(clean_str);
            free(str);
            free(clean_str);
            return value;
        } else {
            char* end;
            double num = strtod(clean_str, &end);
            free(str);
            free(clean_str);
            return create_number_value(num);
        }
    } else if (strncmp(*toml, "true", 4) == 0) {
        *toml += 4;
        return create_boolean_value(true);
    } else if (strncmp(*toml, "false", 5) == 0) {
        *toml += 5;
        return create_boolean_value(false);
    } else {
        return NULL;
    }
}

Value* parse_toml(const char* toml) {
    Value* root = create_object_value();
    if (!root) return NULL;
    
    char current_table[MAX_TABLE_NAME] = "";
    const char* ptr = toml;
    
    while (*ptr != '\0') {
        // 跳過空白
        while (is_whitespace(*ptr)) ptr++;
        
        // 跳過注釋和空行
        if (*ptr == '#' || *ptr == '\0' || *ptr == '\n') {
            // 跳到下一行
            while (*ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        // 處理表格標題 [table]
        if (*ptr == '[') {
            bool is_array_table = (*(ptr + 1) == '[');
            ptr += is_array_table ? 2 : 1;
            
            const char* table_name_start = ptr;
            while (*ptr != '\0' && (*ptr != ']' || (is_array_table && *(ptr + 1) != ']'))) ptr++;
            
            if (*ptr == '\0') {
                free_value(root);
                return NULL; // 無效的表格標題
            }
            
            int len = ptr - table_name_start;
            if (len < MAX_TABLE_NAME - 1) {
                strncpy(current_table, table_name_start, len);
                current_table[len] = '\0';
            } else {
                current_table[0] = '\0'; // 表格名稱過長
            }
            
            // 建立表格路徑
            if (current_table[0] != '\0') {
                char* table_path = strdup(current_table);
                if (!table_path) {
                    free_value(root);
                    return NULL;
                }
                
                char* path_saveptr = NULL;
                char* token = strtok_r(table_path, ".", &path_saveptr);
                Value* current = root;
                
                while (token != NULL) {
                    Value* next = object_get(current, token);
                    if (next == NULL) {
                        next = create_object_value();
                        if (!next) {
                            free(table_path);
                            free_value(root);
                            return NULL;
                        }
                        object_add(current, token, next);
                    }
                    current = next;
                    token = strtok_r(NULL, ".", &path_saveptr);
                }
                
                free(table_path);
            }
            
            // 跳過閉合括號
            ptr += is_array_table ? 2 : 1;
            
            // 跳到下一行
            while (*ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        // 處理鍵值對
        char* key = parse_toml_key(&ptr);
        if (key == NULL) {
            // 跳到下一行
            while (*ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        skip_whitespace(&ptr);
        if (*ptr != '=') {
            free(key);
            // 跳到下一行
            while (*ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        ptr++; // 跳過 '='
        skip_whitespace(&ptr);
        
        // 解析值
        Value* value = parse_toml_value(&ptr);
        if (value == NULL) {
            free(key);
            // 跳到下一行
            while (*ptr != '\0' && *ptr != '\n') ptr++;
            if (*ptr == '\n') ptr++;
            continue;
        }
        
        // 添加到根對象或目前的表格
        if (current_table[0] == '\0') {
            object_add(root, key, value);
        } else {
            // 在嵌套表格中添加
            char* table_path = strdup(current_table);
            if (!table_path) {
                free(key);
                free_value(value);
                // 跳到下一行
                while (*ptr != '\0' && *ptr != '\n') ptr++;
                if (*ptr == '\n') ptr++;
                continue;
            }
            
            char* path_saveptr = NULL;
            char* token = strtok_r(table_path, ".", &path_saveptr);
            Value* current = root;
            
            while (token != NULL) {
                Value* next = object_get(current, token);
                if (next == NULL) {
                    next = create_object_value();
                    if (!next) {
                        free(table_path);
                        free(key);
                        free_value(value);
                        // 跳到下一行
                        while (*ptr != '\0' && *ptr != '\n') ptr++;
                        if (*ptr == '\n') ptr++;
                        goto next_line;
                    }
                    object_add(current, token, next);
                }
                current = next;
                token = strtok_r(NULL, ".", &path_saveptr);
            }
            
            object_add(current, key, value);
            free(table_path);
        }
        
        free(key);
        
        // 跳過此行剩餘部分
        while (*ptr != '\0' && *ptr != '\n') ptr++;
        if (*ptr == '\n') ptr++;
        
    next_line:
        continue;
    }
    
    return root;
}

// 生成函式
void append_string(char** buffer, int* size, int* capacity, const char* str) {
    int len = strlen(str);
    if (*size + len >= *capacity) {
        *capacity = (*capacity == 0) ? 1024 : (*capacity * 2);
        *buffer = (char*)realloc(*buffer, *capacity);
    }
    strcpy(*buffer + *size, str);
    *size += len;
}

void append_char(char** buffer, int* size, int* capacity, char c) {
    if (*size + 1 >= *capacity) {
        *capacity = (*capacity == 0) ? 1024 : (*capacity * 2);
        *buffer = (char*)realloc(*buffer, *capacity);
    }
    (*buffer)[(*size)++] = c;
    (*buffer)[*size] = '\0';
}

void append_escaped_string(char** buffer, int* size, int* capacity, const char* str) {
    append_char(buffer, size, capacity, '"');
    
    while (*str) {
        if (*str == '"' || *str == '\\') {
            append_char(buffer, size, capacity, '\\');
        }
        append_char(buffer, size, capacity, *str);
        str++;
    }
    
    append_char(buffer, size, capacity, '"');
}

void generate_json_string(Value* value, char** buffer, int* size, int* capacity) {
    if (value->type == TYPE_STRING) {
        // 添加開始引號
        append_char(buffer, size, capacity, '"');
        
        // 處理字串內容
        const char* str = value->data.string_value;
        
        while (*str) {
            switch (*str) {
                case '"':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, '"');
                    break;
                case '\\':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, '\\');
                    break;
                case '\n':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, 'n');
                    break;
                case '\r':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, 'r');
                    break;
                case '\t':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, 't');
                    break;
                case '\b':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, 'b');
                    break;
                case '\f':
                    append_char(buffer, size, capacity, '\\');
                    append_char(buffer, size, capacity, 'f');
                    break;
                default:
                    append_char(buffer, size, capacity, *str);
                    break;
            }
            str++;
        }
        
        // 添加結束引號
        append_char(buffer, size, capacity, '"');
    }else if (value->type == TYPE_NUMBER) {
        // 數字處理部分保持不變
        char num_str[64];
        double num = value->data.number_value;
        int int_part = (int)num;
        
        if (num == int_part) {
            sprintf(num_str, "%d", int_part);
        } else {
            sprintf(num_str, "%g", num);
        }
        
        append_string(buffer, size, capacity, num_str);
    } else if (value->type == TYPE_BOOLEAN) {
        // 布林值處理保持不變
        if (value->data.boolean_value) {
            append_string(buffer, size, capacity, "true");
        } else {
            append_string(buffer, size, capacity, "false");
        }
    } else if (value->type == TYPE_NULL) {
        // NULL 處理保持不變
        append_string(buffer, size, capacity, "null");
    } else if (value->type == TYPE_ARRAY) {
        // 陣列處理保持不變
        append_char(buffer, size, capacity, '[');
        
        for (int i = 0; i < value->data.array.size; i++) {
            if (i > 0) {
                append_string(buffer, size, capacity, ", ");
            }
            generate_json_string(value->data.array.items[i], buffer, size, capacity);
        }
        
        append_char(buffer, size, capacity, ']');
    } else if (value->type == TYPE_OBJECT) {
        // 物件處理保持不變
        append_char(buffer, size, capacity, '{');
        
        for (int i = 0; i < value->data.object.size; i++) {
            if (i > 0) {
                append_string(buffer, size, capacity, ", ");
            }
            
            // 處理鍵
            append_escaped_string(buffer, size, capacity, value->data.object.keys[i]);
            append_string(buffer, size, capacity, ": ");
            // 處理值
            generate_json_string(value->data.object.values[i], buffer, size, capacity);
        }
        
        append_char(buffer, size, capacity, '}');
    }
}

char* generate_json(Value* value) {
    char* buffer = NULL;
    int size = 0;
    int capacity = 0;
    
    generate_json_string(value, &buffer, &size, &capacity);
    return buffer;
}

// TOML 生成函式
void generate_toml_string_value(Value* value, char** buffer, int* size, int* capacity, bool in_array) {
    if (value->type == TYPE_STRING) {
        // 檢查是否為複雜字串（含換行符）
        const char* str = value->data.string_value;
        bool needs_multiline = false;
        while (*str) {
            if (*str == '\n') {
                needs_multiline = true;
                break;
            }
            str++;
        }
        
        if (needs_multiline) {
            append_string(buffer, size, capacity, "\"\"\"");
            append_string(buffer, size, capacity, value->data.string_value);
            append_string(buffer, size, capacity, "\"\"\"");
        } else {
            append_escaped_string(buffer, size, capacity, value->data.string_value);
        }
    } else if (value->type == TYPE_NUMBER) {
        char num_str[64];
        double num = value->data.number_value;
        int int_part = (int)num;
        
        if (num == int_part) {
            sprintf(num_str, "%d", int_part);
        } else {
            sprintf(num_str, "%g", num);
        }
        
        append_string(buffer, size, capacity, num_str);
    } else if (value->type == TYPE_BOOLEAN) {
        if (value->data.boolean_value) {
            append_string(buffer, size, capacity, "true");
        } else {
            append_string(buffer, size, capacity, "false");
        }
    } else if (value->type == TYPE_NULL) {
        append_string(buffer, size, capacity, "null");
    } else if (value->type == TYPE_ARRAY) {
        append_char(buffer, size, capacity, '[');
        
        for (int i = 0; i < value->data.array.size; i++) {
            if (i > 0) {
                append_string(buffer, size, capacity, ", ");
            }
            generate_toml_string_value(value->data.array.items[i], buffer, size, capacity, true);
        }
        
        append_char(buffer, size, capacity, ']');
    } else if (value->type == TYPE_OBJECT && in_array) {
        // 陣列中的內聯表格
        append_char(buffer, size, capacity, '{');
        
        for (int i = 0; i < value->data.object.size; i++) {
            if (i > 0) {
                append_string(buffer, size, capacity, ", ");
            }
            
            append_string(buffer, size, capacity, value->data.object.keys[i]);
            append_string(buffer, size, capacity, " = ");
            generate_toml_string_value(value->data.object.values[i], buffer, size, capacity, true);
        }
        
        append_char(buffer, size, capacity, '}');
    }
}

void generate_toml_object(Value* object, char** buffer, int* size, int* capacity, const char* prefix) {
    // 先輸出當前層級的簡單鍵值對
    for (int i = 0; i < object->data.object.size; i++) {
        char* key = object->data.object.keys[i];
        Value* value = object->data.object.values[i];
        
        if (value->type != TYPE_OBJECT) {
            if (prefix[0] == '\0') {
                append_string(buffer, size, capacity, key);
                append_string(buffer, size, capacity, " = ");
                generate_toml_string_value(value, buffer, size, capacity, false);
                append_string(buffer, size, capacity, "\n");
            } else {
                char full_key[MAX_KEY_SIZE];
                strncpy(full_key, prefix, MAX_KEY_SIZE - 1);
                full_key[MAX_KEY_SIZE - 1] = '\0';
                
                size_t prefix_len = strlen(full_key);
                if (prefix_len + 1 < MAX_KEY_SIZE) {
                    full_key[prefix_len] = '.';
                    full_key[prefix_len + 1] = '\0';
                    strncat(full_key, key, MAX_KEY_SIZE - prefix_len - 2);
                }
                
                append_string(buffer, size, capacity, full_key);
                append_string(buffer, size, capacity, " = ");
                generate_toml_string_value(value, buffer, size, capacity, false);
                append_string(buffer, size, capacity, "\n");
            }
        }
    }
    
    // 然後輸出嵌套表格
    for (int i = 0; i < object->data.object.size; i++) {
        char* key = object->data.object.keys[i];
        Value* value = object->data.object.values[i];
        
        if (value->type == TYPE_OBJECT) {
            // 創建表格標題
            char full_key[MAX_KEY_SIZE];
            
            // 使用字符串操作函數代替 snprintf
            full_key[0] = '[';
            full_key[1] = '\0';
            
            if (prefix[0] == '\0') {
                strncat(full_key, key, MAX_KEY_SIZE - 4);
            } else {
                strncat(full_key, prefix, MAX_KEY_SIZE - 4);
                size_t len = strlen(full_key);
                if (len < MAX_KEY_SIZE - 4) {
                    full_key[len] = '.';
                    full_key[len + 1] = '\0';
                    strncat(full_key, key, MAX_KEY_SIZE - len - 4);
                }
            }
            
            // 確保添加閉合括號
            strncat(full_key, "]", MAX_KEY_SIZE - strlen(full_key) - 1);
            
            // 添加空行和表格標題
            append_string(buffer, size, capacity, "\n");
            append_string(buffer, size, capacity, full_key);
            append_string(buffer, size, capacity, "\n");
            
            // 生成新前綴
            char new_prefix[MAX_KEY_SIZE];
            new_prefix[0] = '\0';
            
            if (prefix[0] == '\0') {
                strncat(new_prefix, key, MAX_KEY_SIZE - 1);
            } else {
                strncat(new_prefix, prefix, MAX_KEY_SIZE - 1);
                size_t len = strlen(new_prefix);
                if (len < MAX_KEY_SIZE - 2) {
                    new_prefix[len] = '.';
                    new_prefix[len + 1] = '\0';
                    strncat(new_prefix, key, MAX_KEY_SIZE - len - 2);
                }
            }
            
            // 輸出當前表格的簡單鍵值對
            for (int j = 0; j < value->data.object.size; j++) {
                char* sub_key = value->data.object.keys[j];
                Value* sub_value = value->data.object.values[j];
                
                if (sub_value->type != TYPE_OBJECT) {
                    append_string(buffer, size, capacity, sub_key);
                    append_string(buffer, size, capacity, " = ");
                    generate_toml_string_value(sub_value, buffer, size, capacity, false);
                    append_string(buffer, size, capacity, "\n");
                }
            }
            
            // 處理嵌套表格
            for (int j = 0; j < value->data.object.size; j++) {
                char* sub_key = value->data.object.keys[j];
                Value* sub_value = value->data.object.values[j];
                
                if (sub_value->type == TYPE_OBJECT) {
                    // 創建嵌套表格標題
                    char nested_key[MAX_KEY_SIZE];
                    nested_key[0] = '[';
                    nested_key[1] = '\0';
                    
                    strncat(nested_key, new_prefix, MAX_KEY_SIZE - 4);
                    size_t len = strlen(nested_key);
                    if (len < MAX_KEY_SIZE - 4) {
                        nested_key[len] = '.';
                        nested_key[len + 1] = '\0';
                        strncat(nested_key, sub_key, MAX_KEY_SIZE - len - 4);
                        strncat(nested_key, "]", MAX_KEY_SIZE - strlen(nested_key) - 1);
                    }
                    
                    append_string(buffer, size, capacity, "\n");
                    append_string(buffer, size, capacity, nested_key);
                    append_string(buffer, size, capacity, "\n");
                    
                    // 輸出嵌套表格的鍵值對
                    for (int k = 0; k < sub_value->data.object.size; k++) {
                        char* sub_sub_key = sub_value->data.object.keys[k];
                        Value* sub_sub_value = sub_value->data.object.values[k];
                        
                        if (sub_sub_value->type != TYPE_OBJECT) {
                            append_string(buffer, size, capacity, sub_sub_key);
                            append_string(buffer, size, capacity, " = ");
                            generate_toml_string_value(sub_sub_value, buffer, size, capacity, false);
                            append_string(buffer, size, capacity, "\n");
                        } else {
                            // 處理更深層的嵌套
                            char deeper_prefix[MAX_KEY_SIZE];
                            deeper_prefix[0] = '\0';
                            
                            strncat(deeper_prefix, new_prefix, MAX_KEY_SIZE - 1);
                            size_t len = strlen(deeper_prefix);
                            if (len < MAX_KEY_SIZE - 2) {
                                deeper_prefix[len] = '.';
                                deeper_prefix[len + 1] = '\0';
                                strncat(deeper_prefix, sub_key, MAX_KEY_SIZE - len - 2);
                            }
                            
                            // 遞歸處理深層嵌套
                            generate_toml_object(sub_sub_value, buffer, size, capacity, deeper_prefix);
                        }
                    }
                }
            }
        }
    }
}

char* generate_toml(Value* value) {
    char* buffer = NULL;
    int size = 0;
    int capacity = 0;
    
    if (value->type == TYPE_OBJECT) {
        generate_toml_object(value, &buffer, &size, &capacity, "");
    } else {
        // 頂層必須為對象
        return strdup("Error: TOML top level must be an object");
    }
    
    return buffer;
}

// 主程式
int main() {
    char format[10];
    int lines;
    
    // main函數中的輸入處理部分
    printf("Which language is this: ");
    scanf("%9s", format);

    // 清空輸入緩衝區
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("How many lines: ");
    scanf("%d", &lines);

    // 再次清空輸入緩衝區
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Enter your data:\n");
    
    char input[MAX_INPUT_SIZE] = {0};
    char line[MAX_LINE_SIZE];
    
    for (int i = 0; i < lines; i++) {
        if (fgets(line, MAX_LINE_SIZE, stdin) != NULL) {
            strcat(input, line);
        }
    }
    
    Value* parsed_value = NULL;
    char* output = NULL;
    
    if (strcasecmp(format, "JSON") == 0) {
        parsed_value = parse_json(input);
        if (parsed_value) {
            output = generate_toml(parsed_value);
        }
    } else if (strcasecmp(format, "TOML") == 0) {
        parsed_value = parse_toml(input);
        if (parsed_value) {
            output = generate_json(parsed_value);
        }
    }
    
    if (output) {
        printf("Output:\n%s\n", output);
        free(output);
    } else {
        printf("Error parsing input\n");
    }
    
    if (parsed_value) {
        free_value(parsed_value);
    }
    
    return 0;
}