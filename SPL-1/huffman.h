#include <stdio.h>
#include <stdlib.h>
#include<string.h>

struct node
{
    char c;
    int freq;
    struct node* left;
    struct node* right;
};

void compress(struct node* root, char code[], int top, char *res[])
{
    if (!root)
        return;

    if (root->left)
    {
        code[top] = '0';
        compress(root->left, code, top + 1, res);
    }

    if (root->right)
    {
        code[top] = '1';
        compress(root->right, code, top + 1, res);
    }

    if (!root->left && !root->right)
    {
        code[top] = '\0';
        int idx = (int)(root->c);
        res[idx] = (char*)malloc(strlen(code)+1);
        strcpy(res[idx], code);
    }
}


void write_file(char filename[], char *res[], size_t *original_size, size_t *compressed_size)
{
    FILE *input = fopen(filename, "r");
    char output[1000];
    strcpy(output, filename);

    fseek(input, 0, SEEK_END);
    *original_size = ftell(input);
    fseek(input, 0, SEEK_SET);
    for(int i=0; i<strlen(filename); i++)
    {
        if(output[i]=='.')
        {
            output[i]='o';
            output[i+1]='u';
            output[i+2]='t';
            output[i+3]='.';
            output[i+4]='t';
            output[i+5]='x';
            output[i+6]='t';
            output[i+7]='\0';
            break;
        }
    }

    FILE *out = fopen(output, "wb"); 
    if (!out)
    {
        printf("Unable to open the output file.\n");
        exit(1);
    }

    char buffer = 0; 
    int buffer_count = 0; 

    char bit;
    while((bit=fgetc(input))!=EOF)
    {

        char *code = res[(int)bit];
        int code_len = strlen(code);
        for (int i = 0; i < code_len; i++)
        {
            if (code[i] == '1')
            {
                buffer |= (1 << (7 - buffer_count)); 
            }
            buffer_count++;

         
            if (buffer_count == 8)
            {
                fwrite(&buffer, sizeof(char), 1, out);
                buffer = 0;
                buffer_count = 0;
            }
        }
    }

    if (buffer_count > 0)
    {
        fwrite(&buffer, sizeof(char), 1, out);
    }
    fseek(out, 0, SEEK_END);
    *compressed_size = ftell(out);

    fclose(input);
    fclose(out);
}
void write_array(char input[], char *res[])
{
    FILE *fp = fopen(input, "wb");
    int row,col;
    for(int i=0; res[i][0]; i++)
        row++;

    for(int i=0; res[0][i]; i++)
        col++;

    fwrite(res, 0, row*col, fp);

    fclose(fp);
}

void decompress(char output[], struct node* root)
{
    FILE *out = fopen(output, "rb");  
    if (!out)
    {
        printf("Unable to open the compressed file.\n");
        exit(1);
    }

    char result[1000];
    strcpy(result, output);

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

    strcpy(result + dot_position, ".decompressed.txt");

    FILE *finall = fopen(result, "w");
    if (!finall)
    {
        printf("Unable to open the output file for decompression.\n");
        exit(1);
    }

    char bit;
    struct node *curr = root;
    int bitsRead = 0;

    while (fread(&bit, sizeof(char), 1, out) == 1)  
    {
        for (int i = 0; i < 8; i++)  
        {
            char mask = 1 << (7 - i);
            if ((bit & mask) == mask)
            {
                curr = curr->right;
            }
            else
            {
                curr = curr->left;
            }

            bitsRead++;

            if (!curr->left && !curr->right)
            {
                fprintf(finall, "%c", curr->c);
                curr = root;
            }
        }
        
        // Check for the end of the file before reading the next byte
        if (feof(out))
            break;
    }

    fclose(out);
    fclose(finall);
}



void b_sort(struct node* arr[], int t)
{
    for (int i = 0; i < t; i++)
    {
        for (int j = 0; j < t; j++)
        {
            if (arr[i]->freq < arr[j]->freq)
            {
                struct node* temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

struct node* build(char c, int freq)
{
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->c = c;
    newNode->freq = freq;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

struct node* make_tree(char input[])
{
    FILE* file = fopen(input, "r");
        if (!file)
        {
            printf("Unable to open the file.\n");
            exit(1);
        }

        int freq[256] = {0};

        char c;
        while ((c = fgetc(file)) != EOF)
        {
            freq[(unsigned char)c]++;
        }

        fclose(file);

        int unique = 0;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
            {
                unique++;
            }
        }

        struct node* arr[unique];

        int idx = 0;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
            {
                arr[idx] = build((char)i, freq[i]);
                idx++;
            }
        }

        b_sort(arr, unique);

        while (unique > 1)
        {
            struct node* huff1 = arr[0];
            struct node* huff2 = arr[1];
            struct node* newHuff = build('@', huff1->freq + huff2->freq);
            newHuff->left = huff1;
            newHuff->right = huff2;

            arr[0] = newHuff;

            for (int i = 1; i < unique - 1; i++)
            {
                arr[i] = arr[i + 1];
            }
            unique--;
            b_sort(arr, unique);
        }
        return arr[0];
}
void init_huffman(char input[], int choice, size_t *original_size, size_t *compressed_size)
{
    if(choice==1)
    {
        struct node* arr=make_tree(input);
        struct node *curr = arr;
        char code[256];
        char *res[256];
        compress(curr, code, 0, res);
        write_file(input, res, original_size, compressed_size);
        //write_array(input, res);
    }
    else if(choice==2)
    {
        char output[1000];
        strcpy(output, input);

        for(int i=0; i<strlen(input); i++)
        {
            if(output[i]=='.')
            {
                output[i-3]='.';
                output[i-2]='t';
                output[i-1]='x';
                output[i]='t';
                output[i+1]='\0';
                break;
            }
        }
        struct node* arr=make_tree(output);
        decompress(input, arr);
    }
}
