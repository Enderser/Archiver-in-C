#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int create_archive(const char* filename);
int check_in_arch(const char* archive, const char* filename);
int compress(const char* archive, const char* filename, const char* comp_filename);
int decompress(const char* filename, const char* decomp_filename);
int list_files(const char* filename);

#endif