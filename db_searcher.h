//
// Created by 刘聪 on 2024/6/6.
//

#ifndef DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H
#define DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H

#include <stdio.h>
#include <stdlib.h>
#include "hyper_header_block.h"

#define SUPER_PART_LENGTH 17
#define HEADER_BLOCK_LENGTH 20 // Btree index length, 16 bytes for ip, 4 bytes for data ptr points to index data
#define INDEX_LENGTH 36 // index row length, 16 bytes for start ip, 16 for end ip, 4 bytes for data ptr points to region data
#define HEADER_BLOCK_PTR 9
#define FILE_SIZE_PTR 1

// 定义一个枚举类型来表示搜索类型
typedef enum {
    MEMORY,
    BTREE
} SearchType;

typedef struct {
    int headerLength;
    int* HeaderPtr;
    char** HeaderSip;
} BtreeModeParam;

typedef struct {
    FILE* file;
    BtreeModeParam* btreeModeParam;
    SearchType searchType; // 使用枚举类型来定义searchType字段
    HyperHeaderBlock* hyperHeaderBlock;
} DBSearcher;

DBSearcher* initDBSearcher(char* dbFilePath, char* key, SearchType searchType);
int search(char* ipString, DBSearcher* dbSearcher, char* region, int regionLen);
void closeDBSearcher(DBSearcher* dbSearcher);
void info(DBSearcher* dbSearcher);
BtreeModeParam* initBtreeModeParam(FILE* fp, long offset);

void freeBtreeModeParam(BtreeModeParam* param);

int bTreeSearch(FILE* fp, char* ip, BtreeModeParam* param, char* region, int regionLen, long offset);

void printIp(char* ipBytes, int ipType);

#endif //DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H
