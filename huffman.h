#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t symbol;
    char* code;
} Code;

typedef struct Node_t{
    uint8_t symbol;
    long int f;
    struct Node_t* left;
    struct Node_t* right;
} Node;

void quickSort(Node** numbers, int left, int right);
long int filesize(FILE* fp);
Node* create_Node(uint8_t symb, long int fr, Node* left, Node* right);
Node* build_tree(long int* freq, uint8_t cnt);
void code_generation(Node* node, char* str, uint8_t depth, uint8_t* ind, Code* codes);
void build_tree_from_code(Node** tree, Code code, uint8_t ind);
void decode(Node* node, long int* i, uint8_t* ans, FILE* out);

#endif