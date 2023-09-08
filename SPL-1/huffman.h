#include <stdio.h>
#include <stdlib.h>

struct node {
    char c;
    int freq;
    struct node* left;
    struct node* right;
};

void printCodes(struct node* root, char code[], int top) {
    if (!root)
        return;

    if (root->left) {
        code[top] = '0';
        printCodes(root->left, code, top + 1);
    }

    if (root->right) {
        code[top] = '1';
        printCodes(root->right, code, top + 1);
    }

    if (!root->left && !root->right) {
        code[top] = '\0';
        printf("%c: %s\n", root->c, code);
    }
}

void b_sort(struct node* arr[], int t) {
    for (int i = 0; i < t; i++) {
        for (int j = 0; j < t; j++) {
            if (arr[i]->freq < arr[j]->freq) {
                struct node* temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

struct node* build(char c, int freq) {
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->c = c;
    newNode->freq = freq;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void huffman() {

    FILE* file = fopen("input.txt", "r");
    if (!file) {
        printf("Unable to open the file.\n");
        exit(1);
    }

    int freq[256] = {0};

    char c;
    while ((c = fgetc(file)) != EOF) {
        freq[(unsigned char)c]++;
    }

    fclose(file);

    int t = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            t++;
        }
    }

    struct node* arr[t];

    int idx = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            arr[idx] = build((char)i, freq[i]);
            idx++;
        }
    }

    printf("Before Encoding: \n");
    for (int i = 0; i < t; i++)
        printf("%c: %d\n", arr[i]->c, arr[i]->freq);
    printf("\n");

    b_sort(arr, t);

    while (t > 1) {
        struct node* huff1 = arr[0];
        struct node* huff2 = arr[1];
        struct node* newHuff = build('@', huff1->freq + huff2->freq);
        newHuff->left = huff1;
        newHuff->right = huff2;

        arr[0] = newHuff;

        for (int i = 1; i < t - 1; i++) {
            arr[i] = arr[i + 1];
        }
        t--;
        b_sort(arr, t);
    }

    printf("After Encoding: \n");
    char code[100];
    printCodes(arr[0], code, 0);
}
