/************************************************************************************
* rdbe.c: Implementation File
*
* Relationship DB
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

typedef struct _kvcdb_t     kvcdb_t;
typedef struct _kvdb_ops_t  kvdb_ops_t;
typedef struct _rdb_ops_t   rdb_ops_t;
typedef enum _cdb_type_e    cdb_type_e;

enum _cdb_type_e {
    _cdb_bdb_ = 0, // Berkeley DB
    _cdb_redis_,
    _cdb_leveldb_,
    _cdb_mongodb_,
    _cdb_sqlite_,
    _cdb_mysql_,

    _cdb_type_count_
};

struct _kvdb_ops_t {
    int (*open)(cdb_t *db, const char *db_name, int flags);
    int (*remove)(const char *db_name);
    int (*close)(cdb_t *db);
    int (*get)(cdb_t *db, const char *key, void *data, int size);
    int (*put)(cdb_t *db, const char *key, const void *data, int size);
    int (*del)(cdb_t *db, const char *key);
};

struct _rdb_ops_t {
    int (*open)(cdb_t *db, const char *db_name, int flags);
    int (*remove)(const char *db_name);
    int (*close)(cdb_t *db);
    int (*get)(cdb_t *db, const char *where, void *data, int size);
    int (*put)(cdb_t *db, const char *where, const void *data, int size);
    int (*del)(cdb_t *db, const char *where);
};

struct _cdb_t {
    void *db_handle;
    const cdb_type_e db_type;
    union {
        kvdb_ops_t  kvops;
        rdb_ops_t   rops;
        // other db ops
        // ...
    } ops;
#define kvdbops   ops.kvops
#define rdbops    ops.rops
};

struct _kvcdb_t {
    void *db_handle;
    int (*open)(cdb_t *db, const char *db_name, int flags);
    int (*remove)(const char *db_name);
    int (*close)(cdb_t *db);
    int (*get)(cdb_t *db, const char *key, void *data, int size);
    int (*put)(cdb_t *db, const char *key, const void *data, int size);
    int (*del)(cdb_t *db, const char *key);

    const cdb_type_e db_type;
};

struct _bdb_t {
    void *db_handle;
    const cdb_type_e db_type;

};
// Berkeley DB
int bdb_open(cdb_t *db, const char *db_name, int flags)
{
    int ret = 0;
    
    return ret;
}

int bdb_close(cdb_t *db)
{
    int ret = 0;

    return ret;
}

#define _test_db_file_ "test.db"
int main(int argc, char *arg[])
{
    cdb_t db;
    int ret = 0;

    ret = db.kvdbops.open(&db, _test_db_file_, 0);
}
};

struct _rdb_ops_t {
    int (*open)(cdb_t *db, const char *db_name, int flags);
    int (*remove)(const char *db_name);
    int (*close)(cdb_t *db);
    int (*get)(cdb_t *db, const char *where, void *data, int size);
    int (*put)(cdb_t *db, const char *where, const void *data, int size);
    int (*del)(cdb_t *db, const char *where);
};

struct _cdb_t {
    void *db_handle;
    const cdb_type_e db_type;
    union {
        kvdb_ops_t  kvops;
        rdb_ops_t   rops;
        // other db ops
        // ...
    } ops;
#define kvdbops   ops.kvops
#define rdbops    ops.rops
};

struct _kvcdb_t {
    void *db_handle;
    int (*open)(cdb_t *db, const char *db_name, int flags);
    int (*remove)(const char *db_name);
    int (*close)(cdb_t *db);
    int (*get)(cdb_t *db, const char *key, void *data, int size);
    int (*put)(cdb_t *db, const char *key, const void *data, int size);
    int (*del)(cdb_t *db, const char *key);

    const cdb_type_e db_type;
};

struct _bdb_t {
    void *db_handle;
    const cdb_type_e db_type;

};
// Berkeley DB
int bdb_open(cdb_t *db, const char *db_name, int flags)
{
    int ret = 0;
    
    return ret;
}

int bdb_close(cdb_t *db)
{
    int ret = 0;

    return ret;
}

#define _test_db_file_ "test.db"
int main(int argc, char *arg[])
{
    cdb_t db;
    int ret = 0;

    ret = db.kvdbops.open(&db, _test_db_file_, 0);
}