/************************************************************************************
* bdbe.c: Implementation File
*
* KV DB
*
* DESCRIPTION:
*
* AUTHOR    :    Sean Feng <SeanFeng2006@hotmail.com>
* DATE        :    Nov. 24, 2024
*
* Copyright (c) 2024-?. All Rights Reserved.
*
* REMARKS:
*
*
************************************************************************************/
#include <cdb.h>

extern int cdb_init_bdb(kvdb_t *kvdb);

int init_kvdb(kvdb_t *kvdb, cdb_type_e db_type)
{
    int ret = 0;

    switch (db_type) {
    case _cdb_bdb_:
        ret = cdb_init_bdb(kvdb);
        break;
    case _cdb_redis_:
        break;
    case _cdb_leveldb_:
        break;
    case _cdb_mongodb_:
        break;
    case _cdb_sqlite_:
        break;
    case _cdb_mysql_:
        break;
    default:
        ret = -1;
        db_type = 0;
        break;
    }

    kvdb->db_type = db_type;

    return ret;
}

//gcc -I. -I ~/projects/libs/bdb/include -L ~/projects/libs/bdb/lib -ldb-5 cdb.c bdb.c -o kvdb
#define _test_bdb_name_ "bdb_test.bdb"
int main(int argc, char *arg[])
{
    int ret = 0;
    kvdb_t kvdb;
    char v[128] = {0};

    init_kvdb(&kvdb, _cdb_bdb_);

    ret = kvdb.open(&kvdb, _test_bdb_name_, _CDBO_CREATE_);

    kvdb.put(&kvdb, "test-key1", "test-value1", strlen("test-value1") + 1);

    kvdb.get(&kvdb, "test-key1", v, 128);

    kvdb.close(&kvdb);

    printf("%s\n", v);

    return 0;
}