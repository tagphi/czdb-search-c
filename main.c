#include "db_searcher.h"

int main(void) {
    DBSearcher* dbSearcher = initDBSearcher("/Users/liucong/Downloads/offline_db.czdb", "3fEhuZUEvDzRjKv9qvAzTQ==", BTREE);

    info(dbSearcher);

    closeDBSearcher(dbSearcher);

    return 0;
}
