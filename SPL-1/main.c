#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
//#include "blowfish.h"
#include "aes.h"
#define key_size 32
#define mx 1000

void pad_bytes(unsigned char *byteStream, size_t *len)
{
    size_t padLen = 16 - (*len % 16);
    for (size_t i = 0; i < padLen; i++)
    {
        byteStream[*len + i] = (unsigned char)padLen;
    }
    *len += padLen;
}

void remove_padding(unsigned char *byteStream, size_t *len)
{
    if (*len == 0)
    {
        return;
    }

    unsigned char padLen = byteStream[*len - 1];

    if (padLen <= 16 && padLen > 0)
    {
        for (size_t i = *len - padLen; i < *len; i++)
        {
            if (byteStream[i] != padLen)
            {
                printf("Invalid padding!\n");
                return;
            }
        }
        *len -= padLen;
    }
    else
    {
        printf("Invalid padding value!\n");
    }
}

void read_key(unsigned char round_keys[], int key_len, char filename[])
{
    char keyname[mx];
    strcpy(keyname, filename);

    for (int i = 0; i < strlen(keyname); i++)
    {
        if (keyname[i] == '.')
        {
            keyname[i] = 'k';
            keyname[i + 1] = 'e';
            keyname[i + 2] = 'y';
            keyname[i + 3] = '.';
            keyname[i + 4] = 't';
            keyname[i + 5] = 'x';
            keyname[i + 6] = 't';
            break;
        }
    }
    FILE *fp = fopen(keyname, "rb");

    fread(round_keys, 1, key_len, fp);
    fclose(fp);
    remove(keyname);
}

size_t read_file(unsigned char *byteStream, unsigned char state[][4][4], size_t block_size, size_t *block_num, char filename[])
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error reading file");
        exit(1);
    }

    size_t len = 0;
    size_t k = 0;
    *block_num = 0;

    while (1)
    {
        size_t read = fread(byteStream + len, 1, block_size, file);
        if (read == 0)
            break;

        len += read;
        (*block_num)++;

        if (*block_num > 0)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    state[*block_num - 1][j][i] = byteStream[k++];
                }
            }
        }
    }
    fclose(file);
    return len;
}

void decrypt(unsigned char state[][4][4], unsigned char round_keys[240], size_t len, size_t block_num, char filename[])
{

    for (size_t i = 0; i < block_num; i++)
        add_round_key(state[i], round_keys, 14);

    for (int round = 13; round >= 1; round--)
    {
        for (size_t i = 0; i < block_num; i++)
        {
            inv_shift_row(state[i]);
            inv_substitute(state[i]);
            add_round_key(state[i], round_keys, round);
            invMixCol(state[i]);
        }
    }
    for (size_t i = 0; i < block_num; i++)
    {
        inv_shift_row(state[i]);
        inv_substitute(state[i]);
        add_round_key(state[i], round_keys, 0);
    }
    unsigned char output[mx];

    size_t offset = 0;
    for (size_t i = 0; i < block_num; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            for (size_t k = 0; k < 4; k++)
            {
                output[offset++] = state[i][k][j];
            }
        }
    }

    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("Error writing file\n");
        exit(1);
    }

    fwrite(output, 1, len, fp);
    fclose(fp);
}

void encrypt(unsigned char state[][4][4], unsigned char round_keys[], size_t block_num, size_t len, char filename[])
{
    for (size_t i = 0; i < block_num; i++)
        add_round_key(state[i], round_keys, 0);

    for (int round = 1; round < 14; round++)
    {
        for (size_t i = 0; i < block_num; i++)
        {
            substitute(state[i]);
            shift_row(state[i]);
            mixCol(state[i]);
            add_round_key(state[i], round_keys, round);
        }
    }

    for (size_t i = 0; i < block_num; i++)
    {
        substitute(state[i]);
        shift_row(state[i]);
        add_round_key(state[i], round_keys, 14);
    }
    unsigned char output[mx];

    size_t offset = 0;
    for (size_t i = 0; i < block_num; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            for (size_t k = 0; k < 4; k++)
            {
                output[offset++] = state[i][k][j];
            }
        }
    }

    FILE *encrypted = fopen(filename, "wb");
    if (encrypted == NULL)
    {
        printf("Error writing file\n");
        exit(1);
    }
    fwrite(output, 1, offset, encrypted);
    fclose(encrypted);
}

int key_create(unsigned char key[], unsigned char round_keys[], char filename[])
{
    key_generation(key);
    int key_len = key_expansion(key, round_keys, filename);
    return key_len;
}

int menu()
{
    int choice;
    printf("FILECRYPTOZIPPER\n");
    printf("------------------\n\n");
    printf("1.Encryption\n2.Decryption\n3.Compress\n4.Decompress\n5.Exit\n");
    scanf("%d", &choice);
    return choice;
}

int encrypt_menu()
{
    int choice;
    printf("Which Algorithm?\n\n");
    printf("1.AES\n2.BlowFish\n");
    scanf("%d", &choice);
    return choice;
}

int main()
{
    unsigned char byteStream[mx];
    unsigned char state[mx][4][4];
    unsigned char key[32];
    unsigned char round_keys[240];
    char filename[mx] = "null";
    size_t block_count = 0,len;
    int choice,key_len;

    do
    {
        choice = menu();

        switch (choice)
        {
        case 1:
            printf("Enter file path: \n");
            scanf("%s", filename);

            int encrypt_choice = encrypt_menu();

            if (encrypt_choice == 1)
            {
                len = read_file(byteStream, state, 16, &block_count, filename);
                key_len = key_create(key, round_keys, filename);

                encrypt(state, round_keys, block_count, len, filename);
            }

            else if(encrypt_choice==2)
            {
                
            }
            printf("Encryption Done!\n\n");

            break;

        case 2:
            printf("Enter file path: \n");
            scanf("%s", filename);

            read_file(byteStream, state, 16, &block_count, filename);
            read_key(round_keys, key_len, filename);

            decrypt(state, round_keys, len, block_count, filename);

            printf("Decryption Done!!\n\n");
            break;

        case 3:
            printf("Enter file path:\n");
            scanf("%s", filename);
            init_huffman(filename, 1);
            printf("Compression Done\n");
            break;

        case 4:
            printf("Enter file path: \n");
            scanf("%s", filename);
            init_huffman(filename, 2);
            printf("Decompression done\n");
            break;

        case 5:
            printf("Exiting\n");
            break;

        default:
            printf("Invalid choice\n");
            break;
        }
    } while (choice != 5);

    return 0;
}
