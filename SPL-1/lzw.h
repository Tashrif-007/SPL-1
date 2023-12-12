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
            fwrite(&currentCode, sizeof(int), 1, outputFile);
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
            
            fwrite(&currentCode, sizeof(int), 1, outputFile);

            dictionary[dictSize].prefix = currentCode;
            dictionary[dictSize].character = nextChar;
            dictSize++;

        
            if (dictSize == MAX_DICT_SIZE) {
                initializeDictionary(dictionary);
                dictSize = 256;
            }

            currentCode = nextChar;
        } else {
        
            currentCode = j;
        }
    }
}

void decompressLZW(FILE *inputFile, FILE *outputFile) {
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    int dictSize = 256; 

  
    for (int i = 0; i < 256; ++i) {
        dictionary[i].prefix = -1;
        dictionary[i].character = (char)i;
    }

    int currentCode, previousCode;

    fread(&previousCode, sizeof(int), 1, inputFile);
    fputc(dictionary[previousCode].character, outputFile);

   
    while (fread(&currentCode, sizeof(int), 1, inputFile) > 0) {
        if (currentCode < dictSize) {
            
            while (currentCode >= 256) {
                fputc(dictionary[currentCode].character, outputFile);
                currentCode = dictionary[currentCode].prefix;
            }
            fputc(currentCode, outputFile);
            
            if (dictSize < MAX_DICT_SIZE) {
                dictionary[dictSize].prefix = previousCode;
                dictionary[dictSize].character = currentCode;
                ++dictSize;
            }
        } else {
        
            char firstChar = dictionary[previousCode].character;
            fputc(firstChar, outputFile);

          
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

    strcpy(result + dot_position, ".compressed.bin");  

    FILE *compressedFile = fopen(result, "wb"); 
    if (compressedFile == NULL) {
        perror("Error opening compressed file for writing");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }


    compressLZW(inputFile, compressedFile);
    fseek(compressedFile, 0, SEEK_END);
    *compressed_size = ftell(compressedFile);
    fclose(inputFile);
    fclose(compressedFile);
}


void lzwDecompress(char* filename) {
    FILE *compressedInputFile = fopen(filename, "rb"); 
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


