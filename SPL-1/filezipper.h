#ifndef FILECRYPTOZIPPER_H
#define FILECRYPTOZIPPER_H
#include<stdlib.h>
void pad_bytes(unsigned char *byteStream, size_t *len);
void remove_padding(unsigned char *byteStream, size_t *len);
void read_key(unsigned char round_keys[], int key_len, char filename[]);
size_t read_fileEncrypt(unsigned char *byteStream, unsigned char state[4][4], size_t block_size, char filename[], unsigned char round_keys[]);
size_t read_fileDecrypt(unsigned char *byteStream, unsigned char state[4][4], size_t block_size, char filename[], unsigned char round_keys[]);
size_t decrypt(unsigned char state[4][4], unsigned char round_keys[240], char filename[], unsigned char output[]);
size_t encrypt(unsigned char state[4][4], unsigned char round_keys[], char filename[], unsigned char output[]);
int key_create(unsigned char key[], unsigned char round_keys[], char filename[]);
int menu();
int encrypt_menu();
int compressMenu();
int main();

#endif // FILECRYPTOZIPPER_H
