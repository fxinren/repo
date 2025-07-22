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

extern int cdb_init_bdbe(kvdbe_t *kvdbe);
int init_kvdbe(kvdbe_t *kvdbe, cdb_type_e db_type)
{
    int ret = 0;

    switch (db_type) {
        case _cdb_bdb_:
            ret = cdb_init_bdbe(kvdbe);
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
    
        kvdbe->engine = NULL;
        kvdbe->db_type = db_type;
        kvdbe->path = NULL;

        return ret;
}

//gcc -I. -I ~/projects/libs/bdb/include -L ~/projects/libs/bdb/lib -ldb-5 cdb.c bdb.c -o kvdb
#define _test_db_path_ "/Users/sean/tmp/bdb_test"
#define _test_db_name_ "bdb_test.bdb"
int main(int argc, char *arg[])
{
    int ret = 0;
    kvdbe_t kvdbe;
    kvdb_t kvdb;
    kvdbt_t key;
    kvdbt_t data;
    char v[128] = {0};

    init_kvdbe(&kvdbe, _cdb_bdb_);

    ret = kvdbe.create(&kvdbe, _test_db_path_, _CDBEO_CREATE_ | _CDBEO_TRANSICTION_);
    if (ret < 0) {
        // out put db engine creation error
        return 0;
    }

    ret = kvdbe.opendb(&kvdbe, &kvdb, _test_bdb_name_, _CDBO_CREATE_);
    if (ret < 0) {
        // out put db creation error
        return 0;
    }

    memset(&key, 0, sizeof(kvdbt_t));
    memset(&data, 0, sizeof(kvdbt_t));
    key.data = "test-key1";
    key.size = strlen("test-key1") + 1;

    data.data = "test-value1";
    data.size = strlen("test-value1") + 1;
    kvdb.put(&kvdb, &key, &data); // put

    memset(&data, 0, sizeof(kvdbt_t));
    data.data = v;
    data.size = 128;
    kvdb.get(&kvdb, &key, &data); // get, after put

    kvdb.del(&kvdb, &key); // delete
    kvdb.get(&kvdb, &key, &data); // get, after del

    kvdbe.closedb(&kvdbe, &kvdb);
    kvdbe.destroy(&kvdbe);

    printf("%s\n", v);

    return 0;
}