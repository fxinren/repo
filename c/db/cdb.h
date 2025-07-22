/************************************************************************************
* cdb.h : header file
*
* DB Engine lib Definition header
*
* AUTHOR    :    Sean Feng <SeanFeng2006@hotmail.com>
* DATE      :    Nov. 7, 2024
* Copyright (c) 2024-?. All Rights Reserved.
*
* This code may be used in compiled form in any way you desire. This
* file may be redistributed unmodified by any means PROVIDING it is 
* not sold for profit without the authors written consent, and 
* providing that this notice and the authors name and all copyright 
* notices remains intact. 
*
* An email letting me know how you are using it would be nice as well. 
*
* This file is provided "as is" with no expressed or implied warranty.
* The author accepts no liability for any damage/loss of business that
* this product may cause.
*
************************************************************************************/

#if !defined(__CDB_H__)
#define __CDB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#if defined(__cplusplus)
extern "C" {
#endif

#define _T(x)                   x

typedef char                    tchar_t;

/********************************************************************
*        Macros
*********************************************************************/

/********************************************************************
*        Data Types 
*********************************************************************/

typedef struct _kvdbe_t     kvdbe_t;
typedef struct _kvdb_t      kvdb_t;
typedef enum _cdb_type_e    cdb_type_e;

/* Key/data structure -- a Data-Base Thang. */
typedef struct {
	void	*data;			/* data */
	size_t	 size;			/* data length */
} kvdbt_t;

enum _cdb_type_e {
    _cdb_none_ = 0,
    _cdb_bdb_, // Berkeley DB
    _cdb_redis_,
    _cdb_leveldb_,
    _cdb_mongodb_,
    _cdb_sqlite_,
    _cdb_mysql_,

    _cdb_type_count_
};

// DB engine open flags
#define _CDBEO_CREATE_          0x0001
#define _CDBEO_TRANSICTION_     0x0002

// DB open flags
#define _CDBO_CREATE_           0x0001
#define _CDBO_EXCL_             0x0002
#define _CDBO_RDONLY_           0x0004
#define _CDBO_TRUNCATE_         0x0008


struct _kvdbe_t {
    void        *engine;
    const char  *path;
    // engine
    int (*create)(kvdbe_t *dbe, const char *dir, int oflags);
    int (*destroy)(kvdbe_t *dbe);
    int (*remove)(kvdbe_t *dbe);
    // db
    int (*opendb)(kvdbe_t *dbe, kvdb_t *db, const char *db_name, int oflags);
    int (*removedb)(kvdbe_t *dbe, const char *db_name);
    int (*closedb)(kvdbe_t *dbe, kvdb_t *db);

    cdb_type_e  db_type;
};

struct _kvdb_t {
    kvdbe_t     *dbe;
    void        *mdl;
    const char  *name;

    int (*get)(kvdb_t *db, const kvdbt_t *key, kvdbt_t *value);
    int (*put)(kvdb_t *db, const kvdbt_t *key, const kvdbt_t *value);
    int (*del)(kvdb_t *db, const kvdbt_t *key);

    int (*reset_cursor)(kvdb_t *db);
    int (*traverse)(kvdb_t *db, kvdbt_t *key, kvdbt_t *value);
};

/********************************************************************
*        Functions
*********************************************************************/

int init_kvdbe(kvdbe_t *kvdbe, cdb_type_e db_type);

#if defined(__cplusplus)
}
#endif

#endif /*__CDB_H__*/
