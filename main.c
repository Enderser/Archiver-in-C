#define _CRT_SECURE_NO_WARNINGS
#include "archive.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void print_usage(const char* program_name) {
    printf("Usage:\n");
    printf("  %s create <archive_filename>\n", program_name);
    printf("  %s compress <archive_filename> <file_to_comp> <compressed_file>\n", program_name);
    printf("  %s decompress <archive_filename> <file_to_decomp> <decompressed_file>\n", program_name);
    printf("  %s list <archive_filename>\n", program_name);
}

int main(int argc, char** argv) {
    argc = 5;
    argv[1] = "list"; argv[2] = "archive"; argv[3] = "out.txt"; argv[4] = "inp_rez.txt";
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    const char* command = argv[1];
    const char* archive_filename = argv[2];

    if (strcmp(command, "create") == 0) {
        int f = create_archive(archive_filename);
        if (!f) {
            printf("Archive '%s' created successfully.\n", archive_filename);
        }
        else {
            printf("Failed to create archive '%s'.\n", archive_filename);
            return 1;
        }
    }
    else if (strcmp(command, "compress") == 0) {
        if (argc < 5) {
            print_usage(argv[0]);
            return 1;
        }
        const char* file_to_comp = argv[3];
        const char* compressed_file = argv[4];
        int f = compress(archive_filename, file_to_comp, compressed_file);
        if (!f)
            printf("The file %s successfully compressed into %s\n", file_to_comp, compressed_file);
        else if (f == -1) {
            printf("Archive opening error %s\n", archive_filename);
            return 1;
        }
        else if (f == -2) {
            printf("The archive %s is corrupted.\n", archive_filename);
            return 1;
        }
        else if (f == -3)
            printf("File %s opening error.\n", file_to_comp);
    }
    else if (strcmp(command, "decompress") == 0) {
        if (argc < 5) {
            print_usage(argv[0]);
            return 1;
        }
        const char* file_to_decomp = argv[3];
        const char* decompressed_file = argv[4];
        int f = check_in_arch(archive_filename, file_to_decomp);
        if (f == -1) {
            printf("The archive %s is corrupted.\n", archive_filename);
            return 1;
        }
        else if (f == -2)
            printf("The file %s not found among compressed\n", file_to_decomp);
        else if (!f) {
            f = decompress(file_to_decomp, decompressed_file);
            if (f == -1)
                printf("File %s opening error.\n", file_to_decomp);
            else if (!f)
                printf("The file %s successfully decompressed into %s\n", file_to_decomp, decompressed_file);
        }
    }
    else if (strcmp(command, "list") == 0) {
        int f = list_files(archive_filename);
        if (f == -1) {
            printf("Archive opening error %s\n", archive_filename);
            return 1;
        }
        else if (f == -2) {
            printf("The archive %s is corrupted.\n", archive_filename);
            return 1;
        }
    }
    else {
        print_usage(argv[0]);
        return 1;
    }
    return 0;
}
