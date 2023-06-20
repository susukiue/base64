#ifndef _BASE64_H_
#define _BASE64_H_

extern const unsigned char _base64_map_table[];

#define BASE64_MAP_TABLE _base64_map_table
#define BASE64_FLOW(n) ((size - index) > n)
#define BASE64_SWAP(x, y) do{ (x) ^= (y); (y) ^= (x); (x) ^= (y); }while(0)
#define BASE64_CEIL(x) (((x) == (int)(x)) ? (int)(x) : (int)(x) + 1)

void base64_init_zero(char* str, int size);
int base64_overlen(const char* str, int index);
char base64_map_of(const char* map_table, int ch);
char base64_of_map(const char* map_table, int ch);
int base64_encode(const char* tc, char* entc, int size, int index, const char* map_table);
int base64_decode(const char* fc, char* defc, int size, int index, const char* map_table);
char* _base64_comple(char mode, const char* str, void* (*create_memory)(void*, int), void* another);
char* base64_comple(const char* str, void* (*create_memory)(void*, int), char mode);

#endif
