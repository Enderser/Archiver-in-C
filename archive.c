#define _CRT_SECURE_NO_WARNINGS
#include "archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

# define MAGIC_NUMBER 0x1A2B3C4D

int create_archive(const char* filename) {
	FILE* arch = fopen(filename, "wb");
	if (!arch)
		return -1;
	uint32_t value = MAGIC_NUMBER;
	fwrite(&value, sizeof(uint32_t), 1, arch);
	value = 0;
	fwrite(&value, sizeof(uint32_t), 1, arch);
	fclose(arch);
	return 0;
}

int check_in_arch(const char* archive, const char* filename) {
	FILE* arch = fopen(archive, "rb");
	uint32_t cnt;
	uint8_t len;
	fread(&cnt, sizeof(uint32_t), 1, arch);
	if (cnt != MAGIC_NUMBER)
		return -1;
	fread(&cnt, sizeof(uint32_t), 1, arch);
	for (uint32_t i = 0; i < cnt; i++) {
		fread(&len, sizeof(uint8_t), 1, arch);
		char str[256] = { 0 };
		for (uint8_t j = 0; j < len; j++)
			str[j] = getc(arch);
		str[len] = '\0';
		if (strcmp(filename, str) == 0)
			return 0;
	}
	fclose(arch);
	return -2;
}

int compress(const char* archive, const char* filename, const char* comp_filename) {
	if (check_in_arch(archive, comp_filename) != 0) {
		FILE* arch = fopen(archive, "rb+");
		if (!arch) {
			return -1;
		}
		uint32_t cnt;
		fread(&cnt, sizeof(uint32_t), 1, arch);
		if (cnt != MAGIC_NUMBER)
			return -2;
		long int save = ftell(arch);
		fread(&cnt, sizeof(uint32_t), 1, arch);
		cnt++;
		fseek(arch, save, SEEK_SET);
		fwrite(&cnt, sizeof(uint32_t), 1, arch);
		fseek(arch, 0L, SEEK_END);
		uint8_t len_arch = strlen(comp_filename);
		fwrite(&len_arch, sizeof(uint8_t), 1, arch);
		for (uint8_t i = 0; i < len_arch; i++)
			putc(comp_filename[i], arch);
		fclose(arch);
	}
	FILE* inp = fopen(filename, "rb");
	if (!inp) {
		return -3;
	}
	FILE* out = fopen(comp_filename, "wb");
	long int freq[256] = { 0 }, len = filesize(inp);
	long int save = ftell(inp);
	uint8_t symb;
	for (long int i = 0; i < len; i++) {
		fread(&symb, 1, 1, inp);
		freq[symb]++;
	}
	uint8_t node_cnt = 0, ind = 0;;
	for (int i = 0; i < 256; i++)
		if (freq[i]) node_cnt++;
	Node* tree = build_tree(freq, node_cnt);
	Code codes[256] = { {0} };
	char str[256] = { 0 };
	code_generation(tree, str, 0, &ind, codes);
	uint32_t comp_len = 0;
	fseek(inp, save, SEEK_SET);
	for (long int i = 0; i < len; i++) {
		fread(&symb, 1, 1, inp);
		for (uint8_t j = 0; j < ind; j++)
			if (symb == codes[j].symbol) {
				comp_len += strlen(codes[j].code);
				break;
			}
	}
	uint32_t comp_size = (comp_len + 7) / 8;
	uint8_t* ans = malloc(comp_size);
	memset(ans, 0, comp_size);
	uint32_t bit_ind = 0;
	fseek(inp, save, SEEK_SET);

	fwrite(&comp_len, sizeof(uint32_t), 1, out);
	fwrite(&ind, sizeof(uint8_t), 1, out);
	for (uint8_t i = 0; i < ind; i++) {
		uint8_t code_len = strlen(codes[i].code);
		fwrite(&codes[i].symbol, sizeof(uint8_t), 1, out);
		fwrite(&code_len, sizeof(uint8_t), 1, out);
		uint8_t* res = malloc((code_len + 7) / 8);
		memset(res, 0, (code_len + 7) / 8);
		for (uint8_t k = 0; k < code_len; k++)
			if (codes[i].code[k] == '1') res[k / 8] |= (1 << (7 - (k % 8)));
		fwrite(res, (code_len + 7) / 8, 1, out);
	}

	for (long int i = 0; i < len; i++) {
		fread(&symb, 1, 1, inp);
		for (uint8_t j = 0; j < ind; j++)
			if (symb == codes[j].symbol)
				for (uint8_t k = 0; k < strlen(codes[j].code); k++) {
					if (codes[j].code[k] == '1') ans[bit_ind / 8] |= (1 << (7 - (bit_ind % 8)));
					bit_ind++;
				}
	}
	fwrite(ans, comp_size, 1, out);
	fclose(inp);
	fclose(out);
	return 0;
}

int decompress(const char* filename, const char* decomp_filename) {
	FILE* inp = fopen(filename, "rb");
	if (!inp)
		return -1;
	FILE* out = fopen(decomp_filename, "wb");
	uint8_t ind;
	uint32_t comp_len;
	Code codes[256] = { {0} };
	fread(&comp_len, sizeof(uint32_t), 1, inp);
	fread(&ind, sizeof(uint8_t), 1, inp);
	for (uint8_t i = 0; i < ind; i++) {
		uint8_t code_len;
		char str[256] = { 0 };
		fread(&codes[i].symbol, sizeof(uint8_t), 1, inp);
		fread(&code_len, sizeof(uint8_t), 1, inp);
		uint8_t* res = malloc((code_len + 7) / 8);
		memset(res, 0, (code_len + 7) / 8);
		fread(res, (code_len + 7) / 8, 1, inp);
		for (uint8_t k = 0; k < code_len; k++) {
			if ((((res[k / 8] >> (7 - k % 8))) & 1) == 1) str[k] = '1';
			else str[k] = '0';
		}
		str[code_len] = '\0';
		codes[i].code = (char*)malloc(code_len * sizeof(char));
		strcpy(codes[i].code, str);
	}
	uint32_t comp_size = (comp_len + 7) / 8;
	uint8_t* ans = malloc(comp_size);
	memset(ans, 0, comp_size);
	fread(ans, comp_size, 1, inp);
	Node* tree = create_Node('\0', 0, NULL, NULL);
	for (uint8_t i = 0; i < ind; i++)
		build_tree_from_code(&tree, codes[i], 0);
	long int i = -1;
	while (i < ((long int)comp_len) - 2)
		decode(tree, &i, ans, out);
	fclose(inp);
	fclose(out);
	return 0;
}

int list_files(const char* filename) {
	FILE* arch = fopen(filename, "rb");
	if (!arch) {
		return -1;
	}
	uint32_t cnt;
	fread(&cnt, sizeof(uint32_t), 1, arch);
	if (cnt != MAGIC_NUMBER)
		return -2;
	uint8_t len;
	printf("The list of the '%s' archive:\n", filename);
	fread(&cnt, sizeof(uint32_t), 1, arch);
	for (uint32_t i = 0; i < cnt; i++) {
		fread(&len, sizeof(uint8_t), 1, arch);
		char str[256] = { 0 };
		for (uint8_t j = 0; j < len; j++)
			str[j] = getc(arch);
		str[len] = '\0';
		printf("%d. %s\n", i + 1, str);
	}
	fclose(arch);
	return 0;
}
