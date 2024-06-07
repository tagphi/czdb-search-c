//
// Created by 刘聪 on 2024/6/6.

// I am primarily a Java programmer, so please be understanding of the C code I've written.
// I'm always open to suggestions for improvement!
//
// 我是个写Java的，请多包含。有问题请随时指出！
//

#include <string.h>
#include <arpa/inet.h>
#include "db_searcher.h"
#include "byte_utils.h"

/**
 * Initializes the BtreeModeParam structure.
 *
 * @param fp File pointer to the database file.
 * @param offset Offset to start reading from in the database file.
 * @return Pointer to the initialized BtreeModeParam structure.
 */
BtreeModeParam* initBtreeModeParam(FILE* fp, long offset) {
    fseek(fp, offset, SEEK_SET);
    char superBytes[SUPER_PART_LENGTH];
    fread(superBytes, SUPER_PART_LENGTH, 1, fp);

    fseek(fp, 0, SEEK_END);
    long realFileSize = ftell(fp) - offset;

    long totalHeaderBlockSize = getIntLong(superBytes, HEADER_BLOCK_PTR); // Assuming the totalHeaderBlockSize is at index 1 in superBytes
    long fileSizeInFile = getIntLong(superBytes, FILE_SIZE_PTR);

    if (fileSizeInFile != realFileSize) {
        printf("db file size error, expected [%ld], real [%ld]\n", totalHeaderBlockSize, realFileSize);
        exit(1);
    }

    char* b = (char*)malloc(totalHeaderBlockSize);
    fseek(fp, offset + SUPER_PART_LENGTH, SEEK_SET);
    fread(b, totalHeaderBlockSize, 1, fp);

    int len = (int)totalHeaderBlockSize / HEADER_BLOCK_LENGTH;
    char** HeaderSip = (char**)malloc(len * sizeof(char*));
    int* HeaderPtr = (int*)malloc(len * sizeof(int));

    int idx = 0;
    long dataPtr;
    for (int i = 0; i < totalHeaderBlockSize; i += HEADER_BLOCK_LENGTH) {
        dataPtr = getIntLong(b, i + 16); // Assuming the dataPtr is at index i + 16 in b
        if (dataPtr == 0) {
            break;
        }
        HeaderSip[idx] = (char*)malloc(16);
        memcpy(HeaderSip[idx], &b[i], 16);
        HeaderPtr[idx] = (int)dataPtr;
        idx++;
    }

    BtreeModeParam* param = (BtreeModeParam*)malloc(sizeof(BtreeModeParam));
    param->headerLength = idx;
    param->HeaderPtr = HeaderPtr;
    param->HeaderSip = HeaderSip;

    free(b);

    return param;
}

/**
 * Frees the memory allocated for the BtreeModeParam structure.
 *
 * @param param Pointer to the BtreeModeParam structure to be freed.
 */
void freeBtreeModeParam(BtreeModeParam* param) {
    if (param == NULL) {
        return;
    }

    if (param->HeaderPtr != NULL) {
        free(param->HeaderPtr);
    }

    if (param->HeaderSip != NULL) {
        for (int i = 0; i < param->headerLength; i++) {
            if (param->HeaderSip[i] != NULL) {
                free(param->HeaderSip[i]);
            }
        }
        free(param->HeaderSip);
    }

    free(param);
}

/**
 * Determines the type of the IP address (IPv4 or IPv6).
 *
 * @param ip The IP address as a string.
 * @return The type of the IP address (IPV4 or IPV6).
 */
int getIpType(char* ip) {
    if (strchr(ip, ':') != NULL) {
        return IPV6;
    } else {
        return IPV4;
    }
}

/**
 * Performs a binary search on the database file to find the region associated with the given IP address.
 *
 * @param fp File pointer to the database file.
 * @param ipString The IP address as a string.
 * @param param Pointer to the BtreeModeParam structure.
 * @param region Buffer to store the region associated with the IP address.
 * @param regionLen Length of the region buffer.
 * @param offset Offset to start reading from in the database file.
 * @return 0 if the operation is successful, -1 if the IP address is not found, -2 if the region buffer is too small.
 *        -3 if allocation error occurs.
 */
int bTreeSearch(FILE* fp, char* ipString, BtreeModeParam* param, char* region, int regionLen, long offset) {
    int searchType;
    searchType = getIpType(ipString);
    int ipBytesLength = searchType == IPV4 ? 4 : 16;
    char ip[ipBytesLength];

    getIpBytes(ipString, ip, searchType);

    int l = 0, h = param->headerLength - 1, sptr = 0, eptr = 0;
    while (l <= h) {
        int m = (l + h) / 2;
        int cmp = compareBytes(ip, param->HeaderSip[m], ipBytesLength);
        if (cmp < 0) {
            h = m - 1;
        } else if (cmp > 0) {
            l = m + 1;
        } else {
            sptr = param->HeaderPtr[m > 0 ? m - 1 : m];
            eptr = param->HeaderPtr[m];
            break;
        }
    }

    if (l > h) {
        if (l < param->headerLength) {
            sptr = param->HeaderPtr[l - 1];
            eptr = param->HeaderPtr[l];
        } else if (h >= 0) {
            sptr = param->HeaderPtr[h];
            eptr = param->HeaderPtr[h + 1];
        }
    }

    if (sptr == 0) return -1;

    int blockLen = eptr - sptr, blen = INDEX_LENGTH;
    char* indexBuffer = (char*)malloc((blockLen + blen) * sizeof(char));

    if (indexBuffer == NULL) {
        // handle error
        goto cleanup;
    }

    fseek(fp, sptr + offset, SEEK_SET);
    fread(indexBuffer, blockLen + blen, 1, fp);

    l = 0;
    h = blockLen / blen;
    long dataBlockPtrNSize = 0;
    while (l <= h) {
        int m = (l + h) >> 1;
        int p = m * blen;

        int cmpStart = compareBytes(ip, &indexBuffer[p], 16);
        int cmpEnd = compareBytes(ip, &indexBuffer[p + 16], 16);

        if (cmpStart >= 0 && cmpEnd <= 0) {
            // IP is in this block
            dataBlockPtrNSize = getIntLong(indexBuffer, p + 32);

            break;
        } else if (cmpStart < 0) {
            // IP is less than this block, search in the left half
            h = m - 1;
        } else {
            // IP is greater than this block, search in the right half
            l = m + 1;
        }
    }

    free(indexBuffer);
    if (dataBlockPtrNSize == 0) return -1; // not fount

    int dataLen = (int) ((dataBlockPtrNSize >> 24) & 0xFF);
    if (dataLen > regionLen) {
        // exceeds buffer length
        return -2;
    }

    int dataPtr = (int) ((dataBlockPtrNSize & 0x00FFFFFF));
    fseek(fp, dataPtr + offset, SEEK_SET);
    unsigned char* data = (unsigned char*)malloc(dataLen);

    if (data == NULL) {
        // handle error
        goto cleanup;
    }

    fread(data, dataLen, 1, fp);
    memcpy(region, data, dataLen);
    free(data);

    return 0;

    cleanup:
    if (indexBuffer != NULL) {
        free(indexBuffer);
    }

    return -3;
}

void printIp(char* ipBytes, int ipType) {
    char ipString[INET6_ADDRSTRLEN]; // Buffer to hold the IP string. The size is enough to hold an IPv6 address.

    if (ipType == IPV4) {
        if (inet_ntop(AF_INET, ipBytes, ipString, INET_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
            return;
        }
    } else if (ipType == IPV6) {
        if (inet_ntop(AF_INET6, ipBytes, ipString, INET6_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
            return;
        }
    } else {
        printf("Invalid ipType. It should be either IPV4 or IPV6.\n");
        return;
    }

    printf("%s\n", ipString);
}