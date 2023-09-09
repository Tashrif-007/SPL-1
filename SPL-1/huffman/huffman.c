#include <stdio.h>
#include <stdlib.h>

struct node {
    char c;
    int freq;
    struct node* left;
    struct node* right;
};

void compress(struct node* root, char code[], int top, char *res[]) {
    if (!root)
        return;

    if (root->left) {
        code[top] = '0';
        compress(root->left, code, top + 1, res);
    }

    if (root->right) {
        code[top] = '1';
        compress(root->right, code, top + 1, res);
    }

    if (!root->left && !root->right) {
        code[top] = '\0';
        int idx = (int)(root->c);
        res[idx] = (char*)malloc(strlen(code)+1);
        strcpy(res[idx], code);
        printf("%c : %s\n", root->c, res[idx]);
    }
}

void write_file(char *res[])
{
    FILE *input = fopen("input.txt", "r");
    FILE *output = fopen("output.txt", "w");
    char bit;

    while((bit=fgetc(input))!=EOF)
    {
        fprintf(output, "%s", res[(int)bit]);
    }
    fclose(input);
    fclose(output);
}

void decompress(struct node* root)
{
    FILE *output = fopen("output.txt", "r");
    FILE *result = fopen("decom.txt", "w");
    char bit;
    struct node *curr = root;

    while((bit=fgetc(output))!=EOF)
    {
        printf("%c", bit);
        if(bit=='0' && curr->left)
            curr = curr->left;
        if(bit=='1' && curr->right)
            curr = curr->right;
        if(!curr->left && !curr->right)
        {
            fprintf(result, "%c", curr->c);
            curr = root;
        }
    }
    fclose(output);
    fclose(result);
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

int main() {

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
    struct node *curr = arr[0];
    printf("After Encoding: \n");
    char code[256];
    char *res[256];
    compress(curr, code, 0, res);
    write_file(res);
    curr = arr[0];
    decompress(curr);
    return 0;
}
