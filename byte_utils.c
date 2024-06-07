//
// Created by 刘聪 on 2024/6/5.
//

#include <arpa/inet.h>
#include "byte_utils.h"

int getIntLong(char *b, int offset) {
    return (int)(
            ((b[offset++] & 0x000000FFL)) |
            ((b[offset++] << 8) & 0x0000FF00L) |
            ((b[offset++] << 16) & 0x00FF0000L) |
            ((b[offset] << 24) & 0xFF000000L)
    );
}

int getInt1(char *b, int offset) {
    return (int)(b[offset] & 0xFF);
}

void printBytesInHex(uint8_t *bytes, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");
}

void printBytesInDecimal(char *bytes, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("%d ", bytes[i]);
    }
    printf("\n");
}

int getIpBytes(char* ip, char* ipBytes, int dbType) {
    int result;
    if (dbType == IPV4) {
        result = inet_pton(AF_INET, ip, ipBytes);
    } else if (dbType == IPV6) {
        result = inet_pton(AF_INET6, ip, ipBytes);
    } else {
        printf("Invalid dbType. It should be either IPV4 or IPV6.\n");
        return -1;
    }

    if (result <= 0) {
        if (result == 0) {
            printf("IP address is not in a valid format.\n");
        } else {
            perror("inet_pton");
        }
        return -1;
    }

    return 0;
}

int compareBytes(const char* bytes1, const char* bytes2, int length) {
    for (int i = 0; i < length; i++) {
        if (bytes1[i] * bytes2[i] > 0) {
            if (bytes1[i] < bytes2[i]) {
                return -1;
            } else if (bytes1[i] > bytes2[i]) {
                return 1;
            }
        } else if (bytes1[i] * bytes2[i] < 0) {
            if (bytes1[i] > 0) {
                return -1;
            } else {
                return 1;
            }
        } else if (bytes1[i] * bytes2[i] == 0 && bytes1[i] + bytes2[i] != 0) {
            if (bytes1[i] == 0) {
                return -1;
            } else {
                return 1;
            }
        }
    }
    return 0;
}

