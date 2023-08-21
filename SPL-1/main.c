#include <stdio.h>
#include <stdlib.h>
#include "aes.h"

void pad_bytes(unsigned char *byteStream, size_t *len)
{
    size_t padLen = 16-(*len % 16);
    for(size_t i=0; i<padLen; i++)
    {
        byteStream[*len +i] = (unsigned char)padLen;
    }
    *len+=padLen;
}

void stateArray(const unsigned char *byteStream, unsigned char state[4][4])
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

int main()
{
    FILE *file = fopen("input.txt", "rb");
    if(file==NULL)
    {
        printf("Error reading file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char byteStream[256];
    size_t len=0;

    while(len<size)
    {
        size_t read = fread(byteStream+len, 1, 16, file);
        len+=read;
    }
    fclose(file);

    pad_bytes(byteStream, &len);

    unsigned char state[4][4];
    stateArray(byteStream, state);
    substitute(state);

    //after s_box substitution
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        printf("%0x ", state[i][j]);
    }

    return 0;
}
