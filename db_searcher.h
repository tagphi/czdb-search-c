//
// Created by 刘聪 on 2024/6/6.
//

#ifndef DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H
#define DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "hyper_header_block.h"

#define SUPER_PART_LENGTH 17
#define HEADER_BLOCK_LENGTH 20 // Btree index length, 16 bytes for ip, 4 bytes for data ptr points to index data
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
    int startIndexPtr;
    int endIndexPtr;
    char* geoMapData;
    int ipType;
    int ipBytesLength;
    int columnSelection;
    char* dbBin;
    int indexLength;
} DBSearcher;

#ifdef __cplusplus
extern "C" {
#endif

// API functions with DLL export declarations
DB_SEARCHER_API DBSearcher* initDBSearcher(char* dbFilePath, char* key, SearchType searchType);
DB_SEARCHER_API int search(char* ipString, DBSearcher* dbSearcher, char* region, int regionLen);
DB_SEARCHER_API int unpack(char* geoMapData, long columnSelection, unsigned char* region, int regionSize, char* buf, int bufSize);
DB_SEARCHER_API int getActualGeo(char* geoMapData, long columnSelection, int geoPtr, int geoLen, char* buf, int bufSize);
DB_SEARCHER_API void closeDBSearcher(DBSearcher* dbSearcher);
DB_SEARCHER_API void info(DBSearcher* dbSearcher);
DB_SEARCHER_API BtreeModeParam* initBtreeModeParam(FILE* fp, long offset);
DB_SEARCHER_API int loadGeoMapping(DBSearcher* dbSearcher, int offset, char* key);
DB_SEARCHER_API void freeBtreeModeParam(BtreeModeParam* param);
DB_SEARCHER_API int bTreeSearch(char* ip, DBSearcher* dbSearcher, char* region, int regionLen, long offset, int memoryMode);
DB_SEARCHER_API int decrypt(char* encryptedBytes, int size, char* key);
DB_SEARCHER_API void printIp(char* ipBytes, int ipType);

#ifdef __cplusplus
}
#endif

#endif //DECRYPTEDBLOCKPROJECT_DB_SEARCHER_H
