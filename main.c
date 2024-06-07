#include "db_searcher.h"
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char* dbPath = "/Users/liucong/Downloads/offline_db.czdb";
    char* key = "3fEhuZUEvDzRjKv9qvAzTQ==";
    int opt;

    while ((opt = getopt(argc, argv, "p:k:")) != -1) {
        switch (opt) {
        case 'p':
            dbPath = optarg;
            break;
        case 'k':
            key = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s [-p dbPath] [-k key]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    DBSearcher* dbSearcher = initDBSearcher(dbPath, key, BTREE);

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