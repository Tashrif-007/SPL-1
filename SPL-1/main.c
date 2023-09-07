#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"
//#include <openssl/rand.h>
#include "aes.h"
#define key_size 32
#define mx 100

void pad_bytes(unsigned char *byteStream, size_t *len)
{
    size_t padLen = 16-(*len % 16);
    for(size_t i=0; i<padLen; i++)
    {
        byteStream[*len +i] = (unsigned char)padLen;
    }
    *len+=padLen;
}

void remove_padding(unsigned char *byteStream, size_t *len)
{
    if (*len == 0) {
        return;
    }

    unsigned char padLen = byteStream[*len - 1];

    if (padLen <= 16 && padLen > 0) {
        for (size_t i = *len - padLen; i < *len; i++) {
            if (byteStream[i] != padLen) {
                printf("Invalid padding!\n");
                return;
            }
        }
        *len -= padLen;
    } else {
        printf("Invalid padding value!\n");
    }
}

void stateArray(const unsigned char byteStream[], unsigned char state[4][4])
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            int idx = i*4+j;
            state[j][i]=byteStream[idx];
        }
    }
}

size_t read_file_to_byteStream(unsigned char *byteStream, unsigned char state[][4][4], size_t block_size, size_t *block_num)
{
    FILE *file = fopen("input.txt", "rb");
    if(file==NULL)
    {
        printf("Error reading file");
        exit(1);
    }

    size_t len=0;
    *block_num=0;

    while(1)
    {
        size_t read = fread(byteStream+len, 1, block_size, file);
        if(read==0)
            break;

        len+=read;
        (*block_num)++;

        if(*block_num>0)
        {
            for(size_t i=0; i<16; i++)
                state[*block_num-1][i%4][i/4] = byteStream[len-16+i];
        }
    }
    fclose(file);
    return len;
}

void decrypt(unsigned char state[4][4], unsigned char round_keys[240], size_t len, size_t block_num)
{

    printf("Last round (Decryption):\n");
    for(size_t i=0; i<block_num; i++)
        add_round_key(state[i], round_keys, 14);

    for (int round = 13; round >= 1; round--) {
        printf("Round %d (Decryption):\n", round);
        for(size_t i=0; i<block_num; i++)
        {
            inv_shift_row(state[i]);
            inv_substitute(state[i]);
            add_round_key(state[i], round_keys, round);
            invMixCol(state[i]);
        }
    }
    for(size_t i=0; i<block_num; i++)
    {
        inv_shift_row(state[i]);
        inv_substitute(state[i]);
        add_round_key(state[i], round_keys, 0);
    }

}

void encrypt(unsigned char state[4][4], unsigned char round_keys[], size_t block_num)
{
    for(size_t i=0; i<block_num; i++)
        add_round_key(state[i], round_keys, 0);

    for(int round=1; round<14; round++)
    {
        printf("Round %d: \n", round);
        for(size_t i=0; i<block_num; i++)
        {
            substitute(state[i]);
            shift_row(state[i]);
            mixCol(state[i]);
            add_round_key(state[i], round_keys, round);
        }
    }

    printf("Last round:\n");
    for(size_t i=0; i<block_num; i++)
    {
        substitute(state[i]);
        shift_row(state[i]);
        add_round_key(state[i], round_keys, 14);
    }
}

int main()
{
    unsigned char byteStream[256];
    unsigned char state[mx][4][4];
    unsigned char key[32];
    unsigned char round_keys[240];
    size_t block_count=0;

    size_t len = read_file_to_byteStream(byteStream, state, 16, &block_count);

    //pad_bytes(byteStream, &len);

    stateArray(byteStream, state);
    key_generation(key);
    key_expansion(key, round_keys);

    printf("Before encryption: \n");
    for(size_t i=0; i<block_count; i++)
    {
        for(int j=0; j<4; j++)
        {
            for(int k=0; k<4; k++)
                printf("%02x ", state[i][j][k]);
            printf("\n");
        }
        printf("\n\n");
    }
    encrypt(state, round_keys, block_count);

    for(size_t i=0; i<block_count; i++)
    {
        for(int j=0; j<4; j++)
        {
            for(int k=0; k<4; k++)
                printf("%02x ", state[i][j][k]);
            printf("\n");
        }
        printf("\n\n");
    }

    decrypt(state, round_keys, len, block_count);

    for(size_t i=0; i<block_count; i++)
    {
        for(int j=0; j<4; j++)
        {
            for(int k=0; k<4; k++)
                printf("%02x ", state[i][j][k]);
            printf("\n");
        }
        printf("\n\n");
    }
    //huffman();
    return 0;
}
