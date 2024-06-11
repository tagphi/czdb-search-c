#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include "decrypted_block.h"
#include "byte_utils.h"

#define CLIENT_ID_MASK 0xFFF00000
#define EXPIRATION_DATE_MASK 0x000FFFFF
#define RANDOM_SIZE_OFFSET 4

int base64_decode(const char* input, int length, unsigned char* buffer, int buffer_length) {
    BIO *b64, *bmem;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new_mem_buf((void*)input, length);
    bmem = BIO_push(b64, bmem);

    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
    int decoded_length = BIO_read(bmem, buffer, buffer_length);

    BIO_free_all(bmem);

    if (decoded_length > buffer_length) {
        fprintf(stderr, "Error: Decoded data is larger than buffer.\n");
        return -1;
    }

    return decoded_length;
}

// Decrypt encrypted data using AES ECB mode
// encrypted_data: Pointer to the encrypted data
// key: AES encryption key
// decrypted_data: Pointer to store the decrypted data
// Returns 1 on success, 0 on failure
int aes_ecb_decrypt(const uint8_t *encrypted_data, uint8_t encrypted_data_len, const uint8_t *key, uint8_t *decrypted_data) {
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
    if (1 != EVP_DecryptUpdate(ctx, decrypted_data, &decrypted_len, encrypted_data, encrypted_data_len)) {
        printf("Error: EVP_DecryptUpdate failed.\n");

        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Finalize decryption
    int final_len;
    if (1 != EVP_DecryptFinal_ex(ctx, decrypted_data + decrypted_len, &final_len)) {
        printf("Error: EVP_DecryptFinal_ex failed.\n");

        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    decrypted_len += final_len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);

    return 1;
}

// Function to decrypt encrypted bytes into DecryptedBlock
int decryptEncryptedBytes(const char *key, const uint8_t *encryptedBytes, size_t encryptedSize, DecryptedBlock* decryptedBlock) {
    // Initialize AES key and IV
    AES_KEY aesKey;
    if (AES_set_decrypt_key((const unsigned char *)key, 128, &aesKey) < 0) {
        fprintf(stderr, "Error: AES key initialization failed.\n");
        return -1;
    }

    // Allocate memory for decrypted bytes
    uint8_t *decryptedBytes = (uint8_t *)malloc(encryptedSize);
    if (decryptedBytes == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return -1;
    }

    // Decode Base64 key
    unsigned char key_bytes[16];
    int ret = base64_decode(key, strlen(key), key_bytes, 16);

    if (ret < 0) {
        fprintf(stderr, "Error: Base64 decoding failed.\n");
        free(decryptedBytes);
        return -1;
    }

    if (!aes_ecb_decrypt(encryptedBytes, encryptedSize, key_bytes, decryptedBytes)) {
        fprintf(stderr, "Error: AES decryption failed.\n");
        free(decryptedBytes);
        return -1;
    }

    // Parse the decrypted bytes
    decryptedBlock->clientId = (int) getIntLong(decryptedBytes, 0) >> 20;
    decryptedBlock->expirationDate = (int) getIntLong(decryptedBytes, 0) & EXPIRATION_DATE_MASK;
    decryptedBlock->randomSize = (int) getIntLong(decryptedBytes, 4);

    // Free allocated memory
    free(decryptedBytes);

    return 1;
}
