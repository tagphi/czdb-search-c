#ifndef DECRYPTED_BLOCK_H
#define DECRYPTED_BLOCK_H

#include <stdint.h>

#define EXPIRATION_DATE_MASK 0xFFFFF
#define RANDOM_SIZE_OFFSET 4

typedef struct {
    int clientId;
    int expirationDate;
    int randomSize;
} DecryptedBlock;
int base64_decode(const char* input, int length, unsigned char* buffer, int buffer_length);
int aes_ecb_decrypt(const uint8_t *encrypted_data, uint8_t encrypted_data_len, const uint8_t *key, uint8_t *decrypted_data);
int decryptEncryptedBytes(const char *key, const uint8_t *encryptedBytes, size_t encryptedSize, DecryptedBlock* decryptedBlock);

#endif //DECRYPTED_BLOCK_H