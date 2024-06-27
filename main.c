#include "db_searcher.h"
#include <string.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    char* dbPath = NULL;
    char* key = NULL;
    char* searchMode = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "p:k:m:")) != -1) {
        switch (opt) {
        case 'p':
            dbPath = optarg;
            break;
        case 'k':
            key = optarg;
            break;
        case 'm':
            searchMode = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s -p dbPath -k key -m searchMode\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (dbPath == NULL || key == NULL || searchMode == NULL) {
        fprintf(stderr, "Both -p, -k and -m options are required.\n");
        fprintf(stderr, "Usage: %s -p dbPath -k key -m searchMode\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int mode;
    if (strcmp(searchMode, "memory") == 0) {
        mode = MEMORY;
    } else if (strcmp(searchMode, "btree") == 0) {
        mode = BTREE;
    } else {
        fprintf(stderr, "Invalid search mode. It should be either 'memory' or 'btree'.\n");
        exit(EXIT_FAILURE);
    }

    DBSearcher* dbSearcher = initDBSearcher(dbPath, key, mode);

    info(dbSearcher);

    char region[100];
    char ip[50];

    while (1) {
        printf("Enter IP address (or 'q' to quit): ");
        fgets(ip, 50, stdin);
        // Remove trailing newline
        ip[strcspn(ip, "\n")] = 0;

        if (strcmp(ip, "q") == 0) {
            break;
        }

        if (search(ip, dbSearcher, region, 100) == 0) {
            printf("Region: %s\n", region);
        } else {
            printf("IP not found or an error occurred.\n");
        }
    }

    closeDBSearcher(dbSearcher);

    return 0;
}