/************************************************************************************
* cjson.h : header file
*
* A lite json lib Definition header
*
* AUTHOR	:	Sean Feng <SeanFeng2006@hotmail.com>
* DATE		:	Nov. 7, 2024
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

#if !defined(__CLITEJSON_H__)
#define __CLITEJSON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define _T(x)					x

typedef char					tchar_t;
//typedef long long				int64_t;
//typedef unsigned long long	uint64_t;

/********************************************************************
*        Macros
*********************************************************************/
#define CJSON_KEY_BUF_LEN				32 // bytes

#define CJSON_KEY_LEN_MAX				(CJSON_KEY_BUF_LEN - 2 - 1) // 2 of quotation mark & 1 of \0

#define _token_stack_buf_size_			64 // tchars

#define my_malloc(s)					malloc((s))
#define my_free(p)						free((p))

//==================================================================

/********************************************************************
*        Data Types 
*********************************************************************/
/*
string: "some-value"
number: 123456 / 1234.56
boolean: true / false
array: [element1, .. elementN]
object: {}
null
*/

/*
{
	"name": "John Doe",
	"age": 30,
	"is_active": true,
	"hobbies": ["reading", "music", "sports"],
	"address": {
		"street": "123 Main St",
		"city": "New York",
		"zip": null
	}
}
*/

typedef struct {}				*position_t;

typedef tchar_t					cjson_key_t[CJSON_KEY_BUF_LEN];
typedef struct _cjson_array_t	cjson_array_t;
typedef struct _cjson_kv_t		cjson_kv_t;
typedef struct _cjson_object_t	cjson_object_t;
typedef struct _cjson_t			cjson_t;

// number value types
enum _cjson_valuetype_e {
	_cjson_value_unknown_ = 0,
	_cjson_value_null_,
	_cjson_value_string_,
	_cjson_value_number_,
	_cjson_value_bool_,
	_cjson_value_array_,
	_cjson_value_object_,

	_cjson_value_end_
};
typedef enum _cjson_valuetype_e	cjson_valuetype_e;

// string
struct _cjson_string_t {
	unsigned short	capacity;
	unsigned short	len;
	tchar_t			s[0];
};
typedef struct _cjson_string_t	cjson_string_t;

// number type
struct _cjson_number_t {
	int64_t			number; // 1234567890
	int64_t			divisor; // divisor == 1000,  ==> 1234567.890
};
typedef struct _cjson_number_t	cjson_number_t;

// value
struct _cjson_value_t {
	struct _cjson_value_t	*next;
	// value
	union {
		int					boolval; // boolean value
		cjson_string_t		*strval; // string value
		cjson_number_t		*numval; // number value
		cjson_array_t		*arrval; // array value
		cjson_object_t		*objval; // object value
		void				*_valptr; // for common data type use
	} __value;
	cjson_valuetype_e		value_type;

#define cjson_boolval		__value.boolval
#define cjson_strval		__value.strval
#define cjson_numval		__value.numval
#define cjson_arrval		__value.arrval
#define cjson_objval		__value.objval
#define cjson_valptr		__value._valptr
};
typedef struct _cjson_value_t	cjson_value_t;

// array
struct _cjson_array_t {
	cjson_value_t			*elem;
	int						count;
	cjson_valuetype_e		value_type;
};

// key - value
struct _cjson_kv_t {
	cjson_string_t		*key;
	cjson_value_t		value;
	struct _cjson_kv_t	*next;
};

// object
struct _cjson_object_t {
	cjson_kv_t			*kvs;
	int					count;
};

struct _cjson_t {
	cjson_object_t		*object;
};

/********************************************************************
*        Functions
*********************************************************************/
// jsxon text => data
int cjson_decode(const tchar_t *json_text, cjson_t *data);
// data => jsxon text
int cjson_encode(const cjson_t *json, tchar_t *buf, int buflen);

// array
int cjson_array_add(cjson_array_t *data, cjson_value_t *elem);
int cjson_array_free(cjson_array_t *val);
cjson_value_t* cjson_array_first(cjson_array_t *data, position_t *pos);
cjson_value_t* cjson_array_next(cjson_array_t *data, position_t *pos);

// object
int cjson_object_addkv(cjson_object_t *data, cjson_kv_t *kv);
cjson_kv_t* cjson_object_first(cjson_object_t *data, position_t *pos);
cjson_kv_t* cjson_object_next(cjson_object_t *data, position_t *pos);
int cjson_kv_free(cjson_kv_t *kv);
int cjson_object_free(cjson_object_t *data);

// value
int cjson_value_free(cjson_value_t *val);

#if defined(__cplusplus)
}
#endif

#endif /*__CLITEJSON_H__*/
