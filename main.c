#include "db_searcher.h"

int main(void) {
    DBSearcher* dbSearcher = initDBSearcher("/Users/liucong/Downloads/offline_db.czdb", "3fEhuZUEvDzRjKv9qvAzTQ==", BTREE);

    info(dbSearcher);

    char region[100];
    search("1.32.240.0", dbSearcher, region, 100);

    printf("Region: %s\n", region);

    closeDBSearcher(dbSearcher);

    return 0;
}
