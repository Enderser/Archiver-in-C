#define _CRT_SECURE_NO_WARNINGS
#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void quickSort(Node** numbers, int left, int right) {
    Node* pivot;
    int index;
    int l_hold = left;
    int r_hold = right;
    pivot = numbers[left];
    while (left < right)
    {
        while ((numbers[right]->f < pivot->f) && (left < right))
            right--;
        if (left != right) {
            numbers[left] = numbers[right];
            left++;
        }
        while ((numbers[left]->f > pivot->f) && (left < right))
            left++;
        if (left != right)
        {
            numbers[right] = numbers[left];
            right--;
        }
    }
    numbers[left] = pivot;
    index = left;
    left = l_hold;
    right = r_hold;
    if (left < index)
        quickSort(numbers, left, index - 1);
    if (right > index)
        quickSort(numbers, index + 1, right);
}

long int filesize(FILE* fp) {
    long int save_pos, size_of_file;
    save_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_of_file = ftell(fp);
    fseek(fp, save_pos, SEEK_SET);
    return(size_of_file);
}

Node* create_Node(uint8_t symb, long int fr, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node*));
    node->symbol = symb;
    node->f = fr;
    node->left = left;
    node->right = right;
    return node;
}

Node* build_tree(long int* freq, uint8_t cnt) {
    uint8_t ind = 0;
    Node** nodes = malloc(256 * sizeof(Node*));
    for (int i = 0; i < 256; i++)
        if (freq[i]) {
            nodes[ind] = create_Node(i, freq[i], NULL, NULL);
            ind++;
        }
    quickSort(nodes, 0, cnt - 1);
    while (cnt > 1) {
        Node* left = nodes[cnt - 1];
        Node* right = nodes[cnt - 2];
        nodes[cnt - 2] = create_Node('\0', left->f + right->f, left, right);
        cnt--;
        quickSort(nodes, 0, cnt - 1);
    }
    return nodes[0];
}

void code_generation(Node* node, char* str, uint8_t depth, uint8_t* ind, Code* codes) {
    if (node->left == NULL && node->right == NULL) {
        codes[*ind].symbol = node->symbol;
        codes[*ind].code = (char*)malloc((depth + 1) * sizeof(char));
        strncpy(codes[*ind].code, str, depth);
        codes[*ind].code[depth] = '\0';
        (*ind)++;
        return;
    }
    if (node->left != NULL) {
        str[depth] = '0';
        code_generation(node->left, str, depth + 1, ind, codes);
    }
    if (node->right != NULL) {
        str[depth] = '1';
        code_generation(node->right, str, depth + 1, ind, codes);
    }
}

void build_tree_from_code(Node** tree, Code code, uint8_t ind) {
    if (ind == strlen(code.code)) {
        (*tree)->symbol = code.symbol;
        return;
    }
    else if (code.code[ind] == '0') {
        if ((*tree)->left == NULL)
            (*tree)->left = create_Node('\0', 0, NULL, NULL);
        build_tree_from_code(&(*tree)->left, code, ind + 1);
    }
    else if (code.code[ind] == '1') {
        if ((*tree)->right == NULL)
            (*tree)->right = create_Node('\0', 0, NULL, NULL);
        build_tree_from_code(&(*tree)->right, code, ind + 1);
    }
}

void decode(Node* node, long int* i, uint8_t* ans, FILE* out) {
    if (node->left == NULL && node->right == NULL) {
        fwrite(&node->symbol, 1, 1, out);
        return;
    }
    (*i)++;
    if ((((ans[*i / 8] >> (7 - (*i) % 8))) & 1) == 0)
        decode(node->left, i, ans, out);
    else
        decode(node->right, i, ans, out);
}
