#ifndef CZDB_SEARCHER_HYPER_HEADER_BLOCK_H
#define CZDB_SEARCHER_HYPER_HEADER_BLOCK_H

#include "decrypted_block.h"

#define HEADER_SIZE 12

typedef struct {
    int version;
    int clientId;
    int encryptedBlockSize;
    DecryptedBlock decryptedBlock;
} HyperHeaderBlock;

HyperHeaderBlock* decryptHyperHeaderBlock(FILE *file, const char *key);

int getHyperHeaderBlockSize(HyperHeaderBlock* hyperHeaderBlock);

#endif //CZDB_SEARCHER_HYPER_HEADER_BLOCK_H