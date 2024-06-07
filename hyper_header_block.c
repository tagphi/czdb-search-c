#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "hyper_header_block.h"
#include "decrypted_block.h"

#define HEADER_SIZE 12

// Function to read data from InputStream and deserialize it into HyperHeaderBlock
HyperHeaderBlock* decryptHyperHeaderBlock(FILE *file, const char *key) {
    // Read header bytes
    uint8_t headerBytes[HEADER_SIZE];
    fread(headerBytes, sizeof(uint8_t), HEADER_SIZE, file);

    // Extract fields from header bytes
    int version = *((int*)headerBytes);
    int clientId = *((int*)(headerBytes + 4));
    int encryptedBlockSize = *((int*)(headerBytes + 8));

    // Read encrypted bytes
    uint8_t *encryptedBytes = (uint8_t*)malloc(sizeof(uint8_t) * encryptedBlockSize);
    fread(encryptedBytes, sizeof(uint8_t), encryptedBlockSize, file);

    // Decrypt encrypted bytes into DecryptedBlock
    DecryptedBlock decryptedBlock = decryptEncryptedBytes(key, encryptedBytes, encryptedBlockSize);

    // Check clientId and expirationDate
    if (decryptedBlock.clientId != clientId) {
        printf("Wrong clientId\n");
        fclose(file);
        free(encryptedBytes);
        return NULL;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int currentDate = (tm.tm_year + 1900 + 1) % 100 * 10000 + (tm.tm_mon + 1) * 100 + tm.tm_mday;

    if (decryptedBlock.expirationDate < currentDate) {
        printf("DB is expired\n");
        fclose(file);
        free(encryptedBytes);
        return NULL;
    }

    // Create HyperHeaderBlock and populate fields
    HyperHeaderBlock *hyperHeaderBlock = (HyperHeaderBlock*)malloc(sizeof(HyperHeaderBlock));
    hyperHeaderBlock->version = version;
    hyperHeaderBlock->clientId = clientId;
    hyperHeaderBlock->encryptedBlockSize = encryptedBlockSize;
    hyperHeaderBlock->decryptedBlock = decryptedBlock;

    fclose(file);
    free(encryptedBytes);

    return hyperHeaderBlock;
}

int getHyperHeaderBlockSize(HyperHeaderBlock* hyperHeaderBlock) {
    return HEADER_SIZE + hyperHeaderBlock->encryptedBlockSize + hyperHeaderBlock->decryptedBlock.randomSize;
}