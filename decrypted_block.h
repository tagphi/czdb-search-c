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

DecryptedBlock decryptEncryptedBytes(const char *key, const uint8_t *encryptedBytes, size_t encryptedSize);

#endif //DECRYPTED_BLOCK_H