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

void compressLZW(char *input) {
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    int dictSize = 256;

    initializeDictionary(dictionary);

    FILE *outputFile = fopen("compressed.lzw", "wb");
    if (outputFile == NULL) {
        perror("Error opening compressed file for writing");
        exit(EXIT_FAILURE);
    }

    int currentCode = input[0];

    for (int i = 1; input[i] != '\0'; i++) {
        int nextChar = input[i];
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
            //printf("%d ", currentCode);
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

    fwrite(&currentCode, sizeof(int), 1, outputFile);
    fclose(outputFile);
}

void decompressLZW() {
    DictionaryEntry dictionary[MAX_DICT_SIZE];
    int dictSize = 256;

    initializeDictionary(dictionary);

    FILE *inputFile = fopen("compressed.lzw", "r");
    if (inputFile == NULL) {
        perror("Error opening compressed file for reading");
        exit(EXIT_FAILURE);
    }

    FILE *outputFile = fopen("decompressed.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening decompressed file for writing");
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

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

    fclose(inputFile);
    fclose(outputFile);
}

int main() {
    char input[] = "ABABABA";
    compressLZW(input);
    //decompressLZW();

    return 0;
}
