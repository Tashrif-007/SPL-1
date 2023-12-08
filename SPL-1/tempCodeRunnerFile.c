#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_KEY_SIZE 56
#define MAX_TEXT_SIZE 4096

uint32_t P[18];
uint32_t S[4][256];

// Function to calculate the Blowfish "f" function
uint32_t f(uint32_t x) {
    uint32_t h = S[0][x >> 24] + S[1][(x >> 16) & 0xff];
    return (h ^ S[2][(x >> 8) & 0xff]) + S[3][x & 0xff];
}

// Function to perform Blowfish encryption
void blowfish_encrypt(uint32_t *L, uint32_t *R) {
    for (short r = 0; r < 16; r++) {
        *L = *L ^ P[r];
        *R = f(*L) ^ *R;
        // Swap L and R
        *R ^= *L;
        *L ^= *R;
        *R ^= *L;
    }
    // Final swap
    *R ^= *L;
    *L ^= *R;
    *R ^= *L;
    *R = *R ^ P[16];
    *L = *L ^ P[17];
}

// Function to perform Blowfish decryption
void blowfish_decrypt(uint32_t *L, uint32_t *R) {
    for (short r = 17; r > 1; r--) {
        *L = *L ^ P[r];
        *R = f(*L) ^ *R;
        // Swap L and R
        *R ^= *L;
        *L ^= *R;
        *R ^= *L;
    }
    // Final swap
    *R ^= *L;
    *L ^= *R;
    *R ^= *L;
    *R = *R ^ P[1];
    *L = *L ^ P[0];
}

// Function to perform Blowfish key expansion
void blowfish_key_expand(const uint8_t *key, int key_len) {
    uint32_t k;
    for (short i = 0, p = 0; i < 18; i++) {
        k = 0x00;
        for (short j = 0; j < 4; j++) {
            k = (k << 8) | (uint8_t)key[p];
            p = (p + 1) % key_len;
        }
        P[i] ^= k;
    }

    uint32_t l = 0x00, r = 0x00;
    for (short i = 0; i < 18; i += 2) {
        blowfish_encrypt(&l, &r);
        P[i] = l;
        P[i + 1] = r;
    }
    for (short i = 0; i < 4; i++) {
        for (short j = 0; j < 256; j += 2) {
            blowfish_encrypt(&l, &r);
            S[i][j] = l;
            S[i][j + 1] = r;
        }
    }
}

// Function to encrypt a file
void encrypt_file(const char *input_file_path, const char *output_file_path) {
    FILE *input_file = fopen(input_file_path, "rb");
    if (!input_file) {
        perror("Error opening input file");
        return;
    }

    FILE *output_file = fopen(output_file_path, "wb");
    if (!output_file) {
        perror("Error opening output file");
        fclose(input_file);
        return;
    }

    // Process the file in 64-bit chunks
    uint32_t L, R;
    size_t read_size;
    while ((read_size = fread(&L, sizeof(uint32_t), 1, input_file)) == 1) {
        if (fread(&R, sizeof(uint32_t), 1, input_file) != 1) {
            perror("Error reading file");
            break;
        }

        // Encrypt the chunk
        blowfish_encrypt(&L, &R);

        // Write the encrypted chunk to the output file
        fwrite(&L, sizeof(uint32_t), 1, output_file);
        fwrite(&R, sizeof(uint32_t), 1, output_file);
    }

    fclose(input_file);
    fclose(output_file);

    printf("Encryption completed.\n");
}

// Function to decrypt a file
void decrypt_file(const char *input_file_path, const char *output_file_path) {
    FILE *input_file = fopen(input_file_path, "rb");
    if (!input_file) {
        perror("Error opening input file");
        return;
    }

    FILE *output_file = fopen(output_file_path, "wb");
    if (!output_file) {
        perror("Error opening output file");
        fclose(input_file);
        return;
    }

    // Process the file in 64-bit chunks
    uint32_t L, R;
    size_t read_size;
    while ((read_size = fread(&L, sizeof(uint32_t), 1, input_file)) == 1) {
        if (fread(&R, sizeof(uint32_t), 1, input_file) != 1) {
            perror("Error reading file");
            break;
        }

        // Decrypt the chunk
        blowfish_decrypt(&L, &R);

        // Write the decrypted chunk to the output file
        fwrite(&L, sizeof(uint32_t), 1, output_file);
        fwrite(&R, sizeof(uint32_t), 1, output_file);
    }

    fclose(input_file);
    fclose(output_file);

    printf("Decryption completed.\n");
}

int main() {
    // Key initialization
    uint8_t key[MAX_KEY_SIZE] = {0x0f, 0x1e, 0x2d, 0x3c, 0x4b, 0x5a, 0x69, 0x78};
    int key_len = 8;

    // Input and output file paths
    const char *input_file_path = "input.txt";
    const char *encrypted_file_path = "encrypted.txt";
    const char *decrypted_file_path = "decrypted.txt";

    // Key expansion
    blowfish_key_expand(key, key_len);

    // Encrypt the file
    encrypt_file(input_file_path, encrypted_file_path);

    // Decrypt the file
    decrypt_file(encrypted_file_path, decrypted_file_path);

    return 0;
}
