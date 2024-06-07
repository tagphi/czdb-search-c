#include <stdio.h>
#include "hyper_header_block.h"
#include "decrypted_block.h"
#include "db_searcher.h"

int main(void) {
    FILE *file = fopen("/Users/liucong/Downloads/offline_db.czdb", "rb");
    HyperHeaderBlock* headerBlock = decryptHyperHeaderBlock(file, "3fEhuZUEvDzRjKv9qvAzTQ==");

    if (headerBlock != NULL) {
        printf("Version: %d\n", headerBlock->version);
        printf("Client ID: %d\n", headerBlock->clientId);
        printf("Encrypted Block Size: %d\n", headerBlock->encryptedBlockSize);
        printf("Decrypted Block - Client ID: %d\n", headerBlock->decryptedBlock.clientId);
        printf("Decrypted Block - Expiration Date: %d\n", headerBlock->decryptedBlock.expirationDate);
        printf("Decrypted Block - Random Size: %d\n", headerBlock->decryptedBlock.randomSize);

        BtreeModeParam* btreeModeParam = initBtreeModeParam(file, getHyperHeaderBlockSize(headerBlock));

        char region[100];

        int ret = bTreeSearch(file, "1.32.240.0", btreeModeParam, region, 100, getHyperHeaderBlockSize(headerBlock));

        printf("return code %d, result: %s\n", ret, region);

        freeBtreeModeParam(btreeModeParam);
    } else {
        printf("headerBlock is NULL.\n");
    }
}
