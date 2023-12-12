#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DICT_SIZE 4096

typedef struct {
    int prefix;
    char character;
} DictionaryEntry;

void initializeDictionary(DictionaryEntry dictionary[]) {
    for (int i = 0; i < 256; i++) {
        dictionary[i].prefix = -1;
        dictionary[i].character = (char)i;
    }
}

void compressLZW(FILE *inputFile, FILE *outputFile) {
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    int dictSize = 256;

    initializeDictionary(dictionary);

    int currentCode = fgetc(inputFile);

    while (1) {
        int nextChar = fgetc(inputFile);

        if (nextChar == EOF) {
            fprintf(outputFile, "%d\n", currentCode);
            break;
        }

        int code = (currentCode << 8) + nextChar;

        int j;
        for (j = 0; j < dictSize; j++) {
            if (dictionary[j].prefix == -1 || (dictionary[j].prefix << 8 | dictionary[j].character) == code) {
                break;
            }
        }

        if (dictionary[j].prefix == -1) {
            // Entry not found, add it to the dictionary
            fwrite(&currentCode, sizeof(int), 1, outputFile);

            dictionary[dictSize].prefix = currentCode;
            dictionary[dictSize].character = nextChar;
            dictSize++;

            // Check if the dictionary is full
            if (dictSize == MAX_DICT_SIZE) {
                initializeDictionary(dictionary);
                dictSize = 256;
            }

            currentCode = nextChar;
        } else {
            // Entry found in dictionary, continue with the next character
            currentCode = j;
        }
    }
}

void decompressLZW(FILE *inputFile, FILE *outputFile) {
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    int dictSize = 256; // Initial dictionary size for ASCII characters

    // Initialize dictionary with single-character entries
    for (int i = 0; i < 256; ++i) {
        dictionary[i].prefix = -1;
        dictionary[i].character = (char)i;
    }

    int currentCode, previousCode;

    // Read the first code from the file
    fread(&previousCode, sizeof(int), 1, inputFile);
    fputc(dictionary[previousCode].character, outputFile);

    // Main decompression loop
    while (fread(&currentCode, sizeof(int), 1, inputFile) > 0) {
        if (currentCode < dictSize) {
            // Code exists in the dictionary
            while (currentCode >= 256) {
                fputc(dictionary[currentCode].character, outputFile);
                currentCode = dictionary[currentCode].prefix;
            }
            fputc(currentCode, outputFile);
            
            // Add new entry to the dictionary
            if (dictSize < MAX_DICT_SIZE) {
                dictionary[dictSize].prefix = previousCode;
                dictionary[dictSize].character = currentCode;
                ++dictSize;
            }
        } else {
            // Code does not exist in the dictionary (special case)
            char firstChar = dictionary[previousCode].character;
            fputc(firstChar, outputFile);

            // Add new entry to the dictionary
            if (dictSize < MAX_DICT_SIZE) {
                dictionary[dictSize].prefix = previousCode;
                dictionary[dictSize].character = firstChar;
                ++dictSize;
            }
        }
        previousCode = currentCode;
    }
}

void lzwCompress(char* filename, size_t *original_size, size_t *compressed_size) {
    FILE *inputFile = fopen(filename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    fseek(inputFile, 0, SEEK_END);
    *original_size = ftell(inputFile);
    fseek(inputFile, 0 ,SEEK_SET);
    char result[1000];
    strcpy(result, filename);

    int dot_position = -1;
    for (int i = strlen(result) - 1; i >= 0; i--) {
        if (result[i] == '.') {
            dot_position = i;
            break;
        }
    }

    if (dot_position == -1) {
        printf("Invalid compressed file name.\n");
        exit(1);
    }

    strcpy(result + dot_position, ".compressed.bin");  // Use .compressed.bin instead of .compressed.txt

    FILE *compressedFile = fopen(result, "wb");  // Open in binary mode
    if (compressedFile == NULL) {
        perror("Error opening compressed file for writing");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    // Compress the input file
    compressLZW(inputFile, compressedFile);
    fseek(compressedFile, 0, SEEK_END);
    *compressed_size = ftell(compressedFile);
    fclose(inputFile);
    fclose(compressedFile);
}


void lzwDecompress(char* filename) {
    FILE *compressedInputFile = fopen(filename, "rb");  // Open in binary mode
    if (compressedInputFile == NULL) {
        perror("Error opening compressed file for reading");
        exit(EXIT_FAILURE);
    }

    char result[1000];
    strcpy(result, filename);

    int dot_position = -1;
    for (int i = 0; i <= strlen(result); i++) {
        if (result[i] == '.') {
            dot_position = i;
            break;
        }
    }

    if (dot_position == -1) {
        printf("Invalid compressed file name.\n");
        exit(1);
    }

    strcpy(result + dot_position, ".decompressedlzw.txt");

    FILE *decompressedFile = fopen(result, "w");
    if (decompressedFile == NULL) {
        perror("Error opening decompressed file for writing");
        fclose(compressedInputFile);
        exit(EXIT_FAILURE);
    }

    decompressLZW(compressedInputFile, decompressedFile);

    fclose(compressedInputFile);
    fclose(decompressedFile);
}


