#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"
#include "blowfish.h"
#include "aes.h"
#include "lzw.h"
#include "filezipper.h"
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

size_t read_fileEncrypt(unsigned char *byteStream, unsigned char state[4][4], size_t block_size, char filename[], unsigned char round_keys[])
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error reading file\n");
        exit(1);
    }

    char result[1000] = "";
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
        printf("Invalid file name.\n");
        exit(1);
    }

    strcpy(result + dot_position, ".temp.txt");

    FILE *encrypted = fopen(result, "wb");
    if (encrypted == NULL)
    {
        printf("Error writing file\n");
        fclose(file);
        exit(1);
    }

    size_t len = 0;
    size_t k = 0;
    size_t block_num = 0;
    unsigned char output[mx];

    while (1)
    {
        size_t read = fread(byteStream + len, 1, block_size, file);
        if (read == 0)
            break;

        len += read;
        block_num++;

        if (block_num > 0)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    state[j][i] = byteStream[k++];
                }
            }
        }
        size_t offset = encrypt(state, round_keys, filename, output);
        fwrite(output, 1, offset, encrypted);
    }

    fclose(encrypted);
    fclose(file);

    if (remove(filename) != 0)
    {
        printf("Error removing the original file\n");
        exit(1);
    }

    if (rename(result, filename) != 0)
    {
        printf("Error renaming the temporary file\n");
        exit(1);
    }

    return len;
}

size_t read_fileDecrypt(unsigned char *byteStream, unsigned char state[4][4], size_t block_size, char filename[], unsigned char round_keys[])
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error reading file\n");
        exit(1);
    }

    char result[1000] = "";
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
        printf("Invalid file name.\n");
        exit(1);
    }

    strcpy(result + dot_position, ".temp.txt");

    FILE *decrypted = fopen(result, "wb");
    if (decrypted == NULL)
    {
        printf("Error writing file\n");
        fclose(file);
        exit(1);
    }

    size_t len = 0;
    size_t k = 0;
    size_t block_num = 0;
    unsigned char output[mx];

    while (1)
    {
        size_t read = fread(byteStream + len, 1, block_size, file);
        if (read == 0)
            break;

        len += read;
        block_num++;

        if (block_num > 0)
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    state[j][i] = byteStream[k++];
                }
            }
        }
        size_t offset = decrypt(state, round_keys, filename, output);
        fwrite(output, 1, strlen(output), decrypted);
    }

    fclose(decrypted);
    fclose(file);

    if (remove(filename) != 0)
    {
        printf("Error removing the original file\n");
        exit(1);
    }

    if (rename(result, filename) != 0)
    {
        printf("Error renaming the temporary file\n");
        exit(1);
    }
    return len;
}

size_t decrypt(unsigned char state[4][4], unsigned char round_keys[240], char filename[], unsigned char output[])
{
    add_round_key(state, round_keys, 14);

    for (int round = 13; round >= 1; round--)
    {
        inv_shift_row(state);
        inv_substitute(state);
        add_round_key(state, round_keys, round);
        invMixCol(state);
    }
    inv_shift_row(state);
    inv_substitute(state);
    add_round_key(state, round_keys, 0);

    size_t offset = 0;
    for (size_t j = 0; j < 4; j++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            output[offset++] = state[k][j];
        }
    }
    return offset;
}

size_t encrypt(unsigned char state[4][4], unsigned char round_keys[], char filename[], unsigned char output[])
{
    add_round_key(state, round_keys, 0);

    for (int round = 1; round < 14; round++)
    {
        substitute(state);
        shift_row(state);
        mixCol(state);
        add_round_key(state, round_keys, round);
    }

    substitute(state);
    shift_row(state);
    add_round_key(state, round_keys, 14);

    size_t offset = 0;
    for (size_t j = 0; j < 4; j++)
    {
        for (size_t k = 0; k < 4; k++)
        {
            output[offset++] = state[k][j];
        }
    }
    return offset;
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

int compressMenu()
{
    int choice;
    printf("Which Algorithm?\n\n");
    printf("1.Huffman\n2.LZW\n");
    scanf("%d", &choice);
    return choice;
}

int main()
{
    unsigned char byteStream[mx];
    unsigned char state[4][4];
    unsigned char key[32];
    unsigned char round_keys[240];
    char filename[mx] = "null";
    size_t block_count = 0, len;
    int choice, key_len, encrypt_choice, compChoice;

    do
    {
        choice = menu();

        switch (choice)
        {
        case 1:
            encrypt_choice = encrypt_menu();

            printf("Enter file path: \n");
            scanf("%s", filename);

            if (encrypt_choice == 1)
            {
                key_len = key_create(key, round_keys, filename);
                len = read_fileEncrypt(byteStream, state, 16, filename, round_keys);
            }

            else if (encrypt_choice == 2)
            {
                blow_main(filename);
            }
            printf("Encryption Done!\n\n");
            system("cls");
            break;

        case 2:
            encrypt_choice = encrypt_menu();

            printf("Enter file path: \n");
            scanf("%s", filename);

            if (encrypt_choice == 1)
            {
                read_key(round_keys, key_len, filename);
                read_fileDecrypt(byteStream, state, 16, filename, round_keys);
            }

            else if (encrypt_choice == 2)
            {
                read_key_files(filename);
                decrypt_file_with_keys(filename);
            }
            printf("Decryption Done!!\n\n");
            system("cls");
            break;

        case 3:
            printf("Enter file path:\n");
            scanf("%s", filename);

            compChoice = compressMenu();

            if (compChoice == 1)
            {
                init_huffman(filename, 1);
            }
            else if (compChoice == 2)
            {
                lzwCompress(filename);
            }
            printf("Compression Done\n");
            system("cls");
            break;

        case 4:
            printf("Enter file path: \n");
            scanf("%s", filename);

            compChoice = compressMenu();

            if (compChoice == 1)
            {
                init_huffman(filename, 2);
            }
            else if (compChoice == 2)
            {
                lzwDecompress(filename);
            }
            printf("Decompression done\n");
            system("cls");
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
