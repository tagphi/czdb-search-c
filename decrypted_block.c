#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include "decrypted_block.h"
#include "byte_utils.h"

#define CLIENT_ID_MASK 0xFFF00000
#define EXPIRATION_DATE_MASK 0x000FFFFF
#define RANDOM_SIZE_OFFSET 4

unsigned char* base64_decode(const char* input, int length) {
    BIO *b64, *bmem;
    unsigned char* buffer = (unsigned char*)malloc(length);
    memset(buffer, 0, length);

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf((void*)input, length);
    bmem = BIO_push(b64, bmem);

    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
    int decoded_length = BIO_read(bmem, buffer, length);

    BIO_free_all(bmem);

    return buffer;
}

// Decrypt encrypted data using AES ECB mode
// encrypted_data: Pointer to the encrypted data
// key: AES encryption key
// decrypted_data: Pointer to store the decrypted data
// Returns 1 on success, 0 on failure
int aes_ecb_decrypt(const uint8_t *encrypted_data, const uint8_t *key, uint8_t *decrypted_data) {
    EVP_CIPHER_CTX *ctx;

    // Create and initialize the context
    if (!(ctx = EVP_CIPHER_CTX_new())) {
        return 0;
    }

    // Initialize decryption
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Decrypt data
    int decrypted_len;
    if (1 != EVP_DecryptUpdate(ctx, decrypted_data, &decrypted_len, encrypted_data, AES_BLOCK_SIZE)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Finalize decryption
    int final_len;
    if (1 != EVP_DecryptFinal_ex(ctx, decrypted_data + decrypted_len, &final_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    decrypted_len += final_len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return 1;
}

// Function to decrypt encrypted bytes into DecryptedBlock
DecryptedBlock decryptEncryptedBytes(const char *key, const uint8_t *encryptedBytes, size_t encryptedSize) {
    // Initialize AES key and IV
    AES_KEY aesKey;
    if (AES_set_decrypt_key((const unsigned char *)key, 128, &aesKey) < 0) {
        fprintf(stderr, "Error: AES key initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for decrypted bytes
    uint8_t *decryptedBytes = (uint8_t *)malloc(encryptedSize);
    if (decryptedBytes == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    // Decode Base64 key
    uint8_t *key_bytes = base64_decode(key, strlen(key));
    aes_ecb_decrypt(encryptedBytes, key_bytes, decryptedBytes);

    printBytesInHex(decryptedBytes, encryptedSize);

    // Parse the decrypted bytes
    DecryptedBlock decryptedBlock;
    decryptedBlock.clientId = (int) getIntLong(decryptedBytes, 0) >> 20;
    decryptedBlock.expirationDate = (int) getIntLong(decryptedBytes, 0) & EXPIRATION_DATE_MASK;
    decryptedBlock.randomSize = (int) getIntLong(decryptedBytes, 4);

    // Free allocated memory
    free(decryptedBytes);
    free(key_bytes);

    return decryptedBlock;
}
