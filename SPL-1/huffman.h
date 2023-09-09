#include <stdio.h>
#include <stdlib.h>

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

void write_file(char filename[], char *res[])
{
    FILE *input = fopen(filename, "r");
    char output[1000];
    strcpy(output, filename);

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

    FILE *out = fopen(output, "w");
    char bit;
    while((bit=fgetc(input))!=EOF)
    {
        fprintf(out, "%s", res[(int)bit]);
    }
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
    FILE *out = fopen(output, "r");
    char result[1000];
        strcpy(result, output);

        for(int i=0; i<strlen(output); i++)
        {
            if(result[i]=='.')
            {
                result[i]='d';
                result[i+1]='e';
                result[i+2]='c';
                result[i+3]='o';
                result[i+4]='m';
                result[i+5]='.';
                result[i+6]='t';
                result[i+7]='x';
                result[i+8]='t';
                result[i+9]='\0';
                break;
            }
        }
    FILE *finall = fopen(result, "w");
    char bit;
    struct node *curr = root;

    while((bit=fgetc(out))!=EOF)
    {
        if(bit=='0' && curr->left)
            curr = curr->left;
        if(bit=='1' && curr->right)
            curr = curr->right;
        if(!curr->left && !curr->right)
        {
            fprintf(finall, "%c", curr->c);
            curr = root;
        }
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

        int t = 0;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
            {
                t++;
            }
        }

        struct node* arr[t];

        int idx = 0;
        for (int i = 0; i < 256; i++)
        {
            if (freq[i] > 0)
            {
                arr[idx] = build((char)i, freq[i]);
                idx++;
            }
        }

        b_sort(arr, t);

        while (t > 1)
        {
            struct node* huff1 = arr[0];
            struct node* huff2 = arr[1];
            struct node* newHuff = build('@', huff1->freq + huff2->freq);
            newHuff->left = huff1;
            newHuff->right = huff2;

            arr[0] = newHuff;

            for (int i = 1; i < t - 1; i++)
            {
                arr[i] = arr[i + 1];
            }
            t--;
            b_sort(arr, t);
        }
        return arr[0];
}
void init_huffman(char input[], int choice)
{

    if(choice==1)
    {
        struct node* arr=make_tree(input);
        struct node *curr = arr;
        char code[256];
        char *res[256];
        compress(curr, code, 0, res);
        write_file(input, res);
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
