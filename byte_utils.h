//
// Created by 刘聪 on 2024/6/5.
//

#ifndef DECRYPTEDBLOCKPROJECT_BYTE_UTILS_H
#define DECRYPTEDBLOCKPROJECT_BYTE_UTILS_H

#include <stdint.h>
#include <stdio.h>

#define IPV4 4
#define IPV6 6

int getIntLong(char *b, int offset);
int getInt1(char *b, int offset);
void printBytesInHex(uint8_t *bytes, size_t size);
void printBytesInDecimal(char *bytes, size_t size);
int getIpBytes(char* ip, char* ipBytes, int dbType);
int compareBytes(const char* bytes1, const char* bytes2, int length);

#endif //DECRYPTEDBLOCKPROJECT_BYTE_UTILS_H
