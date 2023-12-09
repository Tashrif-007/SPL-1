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
            fprintf(outputFile, "%d ", currentCode);

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
    int dictSize = 256;

    initializeDictionary(dictionary);

    int currentCode, prevCode;
    fscanf(inputFile, "%d", &prevCode);
    fputc(prevCode, outputFile);

    while (fscanf(inputFile, "%d", &currentCode) != EOF) {
        if (currentCode < dictSize) {
            char entry[MAX_DICT_SIZE];
            int i = 0;

            while (currentCode >= 0) {
                entry[i++] = dictionary[currentCode].character;
                currentCode = dictionary[currentCode].prefix;
            }

            for (i--; i >= 0; i--) {
                fputc(entry[i], outputFile);
            }

            dictionary[dictSize].prefix = prevCode;
            dictionary[dictSize].character = entry[0];
            dictSize++;

            if (dictSize == MAX_DICT_SIZE) {
                initializeDictionary(dictionary);
                dictSize = 256;
            }
        } else {
            char entry[MAX_DICT_SIZE];
            int i = 0;

            while (prevCode >= 0) {
                entry[i++] = dictionary[prevCode].character;
                prevCode = dictionary[prevCode].prefix;
            }

            for (i--; i >= 0; i--) {
                fputc(entry[i], outputFile);
            }

            dictionary[dictSize].prefix = prevCode;
            dictionary[dictSize].character = entry[0];
            dictSize++;

            if (dictSize == MAX_DICT_SIZE) {
                initializeDictionary(dictionary);
                dictSize = 256;
            }

            fputc(entry[0], outputFile);
        }

        prevCode = currentCode;
    }
}

void lzwCompress(char* filename) {
    FILE *inputFile = fopen(filename, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    char result[1000];
    strcpy(result, filename);

    int dot_position = -1;
    for (int i = strlen(result) - 1; i >= 0; i--)
    {
        if (result[i] == '.')
        {
            dot_position = i;
            break;
        }
    }

    if (dot_position == -1)
    {
        printf("Invalid compressed file name.\n");
        exit(1);
    }

    strcpy(result + dot_position, ".compressed.lzw");

    FILE *compressedFile = fopen(result, "wb");
    if (compressedFile == NULL) {
        perror("Error opening compressed file for writing");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    // Compress the input file
    compressLZW(inputFile, compressedFile);

    fclose(inputFile);
    fclose(compressedFile);

}

void lzwDecompress(char* filename)
{
    
    FILE *compressedInputFile = fopen(filename, "rb");
    if (compressedInputFile == NULL) {
        perror("Error opening compressed file for reading");
        exit(EXIT_FAILURE);
    }

    char result[1000];
    strcpy(result, filename);

    int dot_position = -1;
    for (int i = 0; i <= strlen(result); i++)
    {
        if (result[i] == '.')
        {
            dot_position = i;
            break;
        }
    }

    if (dot_position == -1)
    {
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

