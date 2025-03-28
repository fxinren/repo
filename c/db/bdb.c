/************************************************************************************
* bdb.c: Implementation File
*
* Berkeley DB
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
#include <db.h>
#include <string.h>

typedef struct _bdb_mdl_t {
    DB_ENV  *dbenv;
    DB      *dbp;
    DB_TXN  *txn;
    DBC     *cursor;
} bdb_mdl_t;

static void handle_error(DB *dbp, int ret, const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, db_strerror(ret));
    if (dbp != NULL) dbp->close(dbp, 0);
    exit(1);
}

//===============================================
static int _bdb_convert_flags(int oflags)
{
    int ret = 0;

    if (oflags & _CDBO_CREATE_) {
        ret |= DB_CREATE;
    }

    if (oflags & _CDBO_EXCL_ ) {
        ret |= DB_EXCL;
    }

    if (oflags & _CDBO_RDONLY_) {
        ret |= DB_RDONLY;
    }

    if (oflags & _CDBO_TRUNCATE_) {
        ret |= DB_TRUNCATE;
    }

    return ret;
}

// Berkeley DB
static int _bdb_open(kvdb_t *db, const tchar_t *db_name, int oflags)
{
    int ret = 0;
    DB *dbp = NULL;

    oflags = _bdb_convert_flags(oflags);

    // create db handle
    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        handle_error(NULL, ret, "db_create failed");
    }

    // open db
    ret = dbp->open(dbp,        // DB pointer
                    NULL,       // transaction pointer: DB_ENV
                    db_name,    // db file name
                    NULL,       // sub-database anme
                    DB_BTREE,   // DB type
                    oflags,     // open flags
                    0664);      // file permission

    if (ret != 0) {
        handle_error(dbp, ret, "db->open failed");
    }

    db->db_mdl = (void*)dbp;

    return ret;
}

static int _bdb_remove(const tchar_t *db_name) {
    int ret = 0;
    return ret;
}

static int _bdb_close(kvdb_t *db)
{
    DB *dbp = (DB*)(db->db_mdl);
    dbp->close(dbp, 0);
    return 0;
}

static int _bdb_get(kvdb_t *db, const kvdbt_t *key, kvdbt_t *data)
{
    int ret = 0;
    DB *dbp = (DB*)(db->db_mdl);
    DBT bdb_key;
    DBT bdb_data;

    memset(&bdb_key, 0, sizeof(DBT));
    memset(&bdb_data, 0, sizeof(DBT));

    bdb_key.data = (void*)key->data;
    bdb_key.size = key->size;

    bdb_data.data = (void*)data->data;
    bdb_data.size = data->size;

    ret = dbp->get(dbp, NULL, &bdb_key, &bdb_data, 0);
    if (ret == 0) {
        printf("Found key '%s': value='%s', size=%d\n", 
              (char *)bdb_key.data, (char *)bdb_data.data, bdb_data.size);
    } else if (ret == DB_NOTFOUND) {
        printf("Key '%s' not found in database\n", (char *)bdb_key.data);
    } else {
        handle_error(dbp, ret, "db->get failed");
    }

    return ret;
}

static int _bdb_put(kvdb_t *db, const kvdbt_t *key, const kvdbt_t *data)
{
    int ret = 0;
    DB *dbp = (DB*)(db->db_mdl);
    DBT bdb_key;
    DBT bdb_data;

    memset(&bdb_key, 0, sizeof(DBT));
    memset(&bdb_data, 0, sizeof(DBT));

    bdb_key.data = (void*)key->data;
    bdb_key.size = key->size;

    bdb_data.data = (void*)data->data;
    bdb_data.size = data->size;

    ret = dbp->put(dbp, NULL, &bdb_key, &bdb_data, 0);
    if (ret != 0) {
        handle_error(dbp, ret, "db->put failed");
    }

    return ret;
}

static int _bdb_del(kvdb_t *db, const kvdbt_t *key)
{
    int ret = 0;
    DB *dbp = (DB*)(db->db_mdl);
    DBT bdb_key;
    DBT bdb_data;

    memset(&bdb_key, 0, sizeof(DBT));
    memset(&bdb_data, 0, sizeof(DBT));

    bdb_key.data = (void*)key->data;
    bdb_key.size = key->size;

    ret = dbp->del(dbp, NULL, &bdb_key, 0);
    if (ret != 0 && ret != DB_NOTFOUND) {
        handle_error(dbp, ret, "db->del failed");
    }

    return ret;
}

//===============================================
int cdb_init_bdb(kvdb_t *kvdb)
{
    int ret = 0;

    kvdb->db_mdl = 0;
    kvdb->open = _bdb_open;
    kvdb->remove = _bdb_remove;
    kvdb->close = _bdb_close;
    kvdb->get = _bdb_get;
    kvdb->put = _bdb_put;
    kvdb->del = _bdb_del;
    //kvdb->db_type = _cdb_bdb_;

    return ret;
}