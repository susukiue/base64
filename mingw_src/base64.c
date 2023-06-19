#include <stddef.h>
#include "base64.h"

const unsigned char _base64_map_table[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/',
	'='
};

void base64_init_zero(char* str, int size){
	for(int i = 0; i < size; i++) if(str[i]) str[i] = 0;
}

int base64_overlen(const char* str, int index){
	if(!str) return 0;
	int so = index;
	while(str[so]) so++;
	return so - index;
}

char base64_map_of(const char* map_table, int ch){
	return map_table ? map_table[ch] : BASE64_MAP_TABLE[ch];
}

char base64_of_map(const char* map_table, int ch){
	for(int i = 0; i < 64; i++){
		if(map_table ? map_table[i] : BASE64_MAP_TABLE[i] == ch){
			return i;
		}
	}
	return 0;
}

int base64_encode(const char* tc, char* entc, int size, int index, const char* map_table){
	if(tc && entc && size > index){
		if(BASE64_FLOW(0)) entc[index + 0] = base64_map_of(map_table, tc[0] >> 2 & 0x3F);
		if(BASE64_FLOW(1)) entc[index + 1] = base64_map_of(map_table, tc[1] ? (tc[0] << 4 & 0x30) | (tc[1] >> 4 & 0x0F) : tc[0] << 4 & 0x30);
		if(BASE64_FLOW(2)) entc[index + 2] = base64_map_of(map_table, tc[1] ? tc[2] ? (tc[1] << 2 & 0x3C) | (tc[2] >> 6 & 0x03) : tc[1] << 2 & 0x3C : 0x40);
		if(BASE64_FLOW(3)) entc[index + 3] = base64_map_of(map_table, tc[1] && tc[2] ? tc[2] & 0x3F : 0x40);
	}
	return base64_overlen(entc, index);
}

int base64_decode(const char* fc, char* defc, int size, int index, const char* map_table){
	if(fc && defc && size > index){
		char t[4] = { base64_of_map(map_table, fc[0]), base64_of_map(map_table, fc[1]), base64_of_map(map_table, fc[2]), base64_of_map(map_table, fc[3]) };
		if(BASE64_FLOW(0)) defc[index + 0] = (t[0] << 2 & 0xFC) | (t[1] >> 4 & 0x03);
		if(BASE64_FLOW(1)) defc[index + 1] = BASE64_FILL(t, 2) ? (t[1] << 4 & 0xF0) | (t[2] >> 2 & 0x0F) : 0x00;
		if(BASE64_FLOW(2)) defc[index + 2] = BASE64_FILL(t, 3) ? (t[2] << 6 & 0xC0) | (t[3] & 0x3F) : 0x00;
	}
	return base64_overlen(defc, index);
}

char* _base64_comple(char mode, const char* str, void* (*create_memory)(void*, int), void* another){
	int len = base64_overlen(str, 0), ispan = 3, jspan = 4;
	if(mode) BASE64_SWAP(ispan, jspan);
	int size = (BASE64_CEIL(len / (float)ispan) * jspan) + (mode ? 0 : 1);
	char* result = create_memory(another, size);
	base64_init_zero(result, size);
	for(int i = 0, j = 0; i < len; i += ispan, j += jspan){
		switch(mode){
			case 0:
				base64_encode(&str[i], result, size, j, 0); break;
			case 1:
				base64_decode(&str[i], result, size, j, 0); break;
			default:
				i = len;
		}
	}
	return result;
}

char* base64_comple(const char* str, void* (*create_memory)(void*, int), char mode){
	return _base64_comple(mode, str, create_memory, NULL);
}
