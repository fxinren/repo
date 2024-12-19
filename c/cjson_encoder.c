/************************************************************************************
* cjson_encoder.c: Implementation File
*
* cjson encoder
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

#include <cjson.h>

// return characters length that writen to buffer
// return 0 for end of token processing
// return -1 for error
typedef int (*_pfn_cjson_encoder_t)(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_unknown(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_null(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_string(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_number(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_bool(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_array(const cjson_value_t *value, tchar_t *buf, int buflen);
static int _encode_object(const cjson_value_t *value, tchar_t *buf, int buflen);
static const _pfn_cjson_encoder_t _encode_handlers[] = {
    _encode_unknown,    // 0,
    _encode_null,       // 1, value null
    _encode_string,     // 2, string
    _encode_number,     // 3, number
    _encode_bool,       // 4, bool
    _encode_array,      // 5, array
    _encode_object,     // 6, object
};

struct __bool_str_entry_t {
    const tchar_t *str;
    int str_len;
};
typedef struct __bool_str_entry_t _bool_str_entry_t;
static const _bool_str_entry_t _bool_str_entries[2] = {
    { _T("false"),  5 },
    { _T("true"),   4 },
};

static int _encode_unknown(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    return -1;
}

static int _encode_null(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    if (buflen > 3) { // null is always 4 chars
        strcpy(buf, _T("null"));
        return 4;
    }

    return -1;
}

static int _encode_string(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    int ret = 0;
    int i = 0;

    if (buflen < value->cjson_strval->len + 2) { // 2 for quotation mark
        return -1;  // buffer is too small
    }

    buf[0] = _T('"');

    while (i < value->cjson_strval->len) {
        buf[i+1] = value->cjson_strval->s[i];
        i++;
    }

    buf[i + 1] = _T('"');

    return i + 2;
}

static int _encode_number(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    int from = 0;
    int buf_idx = 0;
    int i = 0;
    int point_pos = 0;

    int64_t n = value->cjson_numval->number;
    int64_t d = (value->cjson_numval->divisor == 1) ? 0 : value->cjson_numval->divisor;

    if (n < 0){
        if (buf_idx < buflen) {
            buf[0] = _T('-');
            buf_idx++;
        } else {
            return -1;
        }

        n *= -1;
    }

    from = buf_idx;

    // decimal point position
    while (d > 1) {
        point_pos++;
        d /= 10;
    }

    // decimal digits
    i = 0;
    while (i < point_pos) {
        if (buf_idx < buflen) {
            buf[buf_idx] = n % 10 + _T('0');
        } else  {
            return -1;
        }

        n /= 10;
        buf_idx++;
        i++;
    }

    // decimal point
    if (value->cjson_numval->divisor > 1) {
        if (buf_idx < buflen) {
            buf[buf_idx] = _T('.');
            buf_idx++;
        } else {
            return -1;
        }
    }

    // integer digits
    if (n) {
        while (n) {
            if (buf_idx < buflen) {
                buf[buf_idx] = n % 10 + _T('0');
            } else {
                return -1;
            }
            n /= 10;
            buf_idx++;
        }
    } else if (value->cjson_numval->divisor > 1) {
        buf[buf_idx] = _T('0');
        buf_idx++;
    }

    // reverse the string
    // buf_idx + 1 - from : count of chars
    for (i = from; (i - from) < (buf_idx + 1 - from) / 2; i++) {
        tchar_t tmp = buf[i];
        buf[i] = buf[buf_idx - i - 1 + from];
        buf[buf_idx - i - 1 + from] = tmp;
    }

    return buf_idx;
}

static int _encode_bool(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    int idx = 1;

    if (value->cjson_boolval == 0) {
        idx = 0;
    }

    if (buflen < _bool_str_entries[idx].str_len) {
        return -1;
    }

    strcpy(buf, _bool_str_entries[idx].str);

    return _bool_str_entries[idx].str_len;
}

static int _encode_object(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    int ret = 0;

    cjson_kv_t *kv = NULL;
    cjson_value_t dummy;
    position_t pos;
    int i = 0;

    dummy.next = NULL;

    if (i < buflen) {
        buf[0] = _T('{');
        i++;
    }

    // key : value
    kv = cjson_object_first(value->cjson_objval, &pos);
    while (kv) {
        // key
        if (i < buflen) {
            dummy.value_type = _cjson_value_string_;
            dummy.cjson_strval = kv->key;
            ret = _encode_handlers[dummy.value_type](&dummy, buf + i, buflen - i);
            if (ret < 0) {
                break;
            }
            i += ret;
        } else {
            ret = -1;
            break;
        }

        // colon
        buf[i] = _T(':');
        i++;

        // value
        if (i < buflen) {
            ret = _encode_handlers[kv->value.value_type](&(kv->value), buf + i, buflen - i);
            if (ret < 0) {
                break;
            }
            i += ret;
        } else {
            ret = -1;
            break;
        }

        // item end, ','
        if (i < buflen) {
            buf[i] = _T(',');
            i++;
        } else {
            ret = -1;
            break;
        }

        kv = cjson_object_next(value->cjson_objval, &pos);
    }

    if (ret < 0) {
        return -1;
    }

    if (i > 0) { // change the last ',' to '}'
        buf[i - 1] = _T('}');
    }

    return i;
}

static int _encode_array(const cjson_value_t *value, tchar_t *buf, int buflen)
{
    int ret = 0;
    int i = 0;
    cjson_value_t *val = NULL;
    position_t pos;

    if (i < buflen) {
        buf[0] = _T('[');
        i++;
    }

    // key : value
    val = cjson_array_first(value->cjson_arrval, &pos);
    while (val) {
        // value
        if (i < buflen) {
            ret = _encode_handlers[val->value_type](val, buf + i, buflen - i);
            if (ret < 0) {
                break;
            }
            i += ret;
        } else {
            ret = -1;
            break;
        }

        // item end, ','
        if (i < buflen) {
            buf[i] = _T(',');
            i++;
        } else {
            ret = -1;
            break;
        }

        //printf("count: %d\n", value->cjson_arrval->count);
        val = cjson_array_next(value->cjson_arrval, &pos);
    }

    if (ret < 0) {
        return -1;
    }

    if (i > 0) { // change the last ',' to ']'
        buf[i - 1] = _T(']');
    }

    return i;
}

// data => jsxon text
int cjson_encode(const cjson_t *json, tchar_t *buf, int buflen)
{
    cjson_value_t root_data;

    if (json == NULL || json == NULL || json->object == NULL) {
        return -1;
    }

    root_data.next = NULL;

    root_data.value_type = _cjson_value_object_;
    root_data.cjson_objval = json->object;

    return _encode_object(&root_data, buf, buflen);
}
