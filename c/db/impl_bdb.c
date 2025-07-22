/************************************************************************************
* impl_bdb.c: Implementation File
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

typedef struct _bdb_mdl_t bdb_mdl_t;

struct _bdb_mdl_t {
    DB      *dbp;
    DB_TXN  *txn;
    DBC     *cursor;
};

int cdb_init_bdbe(kvdbe_t *kvdbe)
{
    if (kvdbe->engine) {
        return -1;
    }

    // kvbde function pointers
    kvdbe->create = _bdbe_create;
    kvdbe->destroy = _bdbe_destroy;
    kvdbe->remove = _bdbe_remove;
    // kvdb function pointers
    kvdbe->opendb = _bdbe_opendb;
    kvdbe->removedb = _bdbe_removedb;
    kvdbe->closedb = _bdbe_closedb;

    //kvdbe->engine = NULL;
    //kvdbe->path = NULL;
    //kvdbe->db_type = _cdb_bdb_;

    return 0;
}

// engine
int _bdbe_create(kvdbe_t *dbe, const char *dir, int oflags)
{
    int ret;
    DB_ENV *dbenv = NULL;

    // dbe init already?
    if (dbe->engine) {
        return -1;
    }

    ret = db_env_create(&dbenv, 0);
    if (ret != 0) {
        return ret;
    }

    ret = dbenv->open(dbenv, dir, oflags, 0);
    if (ret != 0) {
        dbenv->close(dbenv, 0);
        return ret;
    }

    dbe->engine = dbenv;
    return 0;
}

int _bdbe_destroy(kvdbe_t *dbe)
{
    DB_ENV *dbenv = dbe->engine;
    if (dbenv != NULL) {
        dbenv->close(dbenv, 0);
        dbe->engine = NULL;
    }
    return 0;
}

int _bdbe_remove(kvdbe_t *dbe)
{
    return 0;
}

// db
int _bdbe_opendb(kvdbe_t *dbe, kvdb_t *db, const char *db_name, int oflags)
{
    int ret;
    DB *dbp = NULL;
    DB_ENV *dbenv = (DB_ENV*)dbe->engine;
    bdb_mdl_t *mdl = NULL;

    mdl = (bdb_mdl_t*)malloc(sizeof(bdb_mdl_t));
    if (mdl == NULL) {
        dbp->close(dbp, 0);
        return -1;
    }

    ret = db_create(&dbp, dbenv, 0);
    if (ret != 0) {
        return ret;
    }

    ret = dbp->open(dbp, NULL, db_name, NULL, DB_BTREE, oflags, 0);
    if (ret != 0) {
        dbp->close(dbp, 0);
        return ret;
    }

    mdl->dbp = dbp;

    db->mdl = mdl;
    db->name = db_name;
    db->get = _bdbe_get;
    db->put = _bdbe_put;
    db->del = _bdbe_del;
    db->reset_cursor = _bdbe_reset_cursor;
    db->traverse = _bdbe_traverse;

    return 0;
}

int _bdbe_removedb(kvdbe_t *dbe, const char *db_name)
{

}

int _bdbe_closedb(kvdbe_t *dbe, kvdb_t *db)
{

}
