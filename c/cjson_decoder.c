/************************************************************************************
* cjson_decoder.c: Implementation File
*
* cjson decoder
*
* DESCRIPTION  :
*
* AUTHOR        :   Sean Feng <SeanFeng2006@hotmail.com>
* DATE          :   Nov. 24, 2024
*
* Copyright (c) 2024-?. All Rights Reserved.
*
* REMARKS:
*
*
************************************************************************************/

#include <cjson.h>

//===========================================================
// cjson array
int cjson_array_add(cjson_array_t *data, cjson_value_t *elem)
{
	int i = 0;
	cjson_value_t dummy;
	cjson_value_t *tail = NULL;

	dummy.next = data->elem;
	tail = &dummy;
	for (i = 0; i < data->count; i++) {
		tail = tail->next;
	}

	tail->next = elem;
	elem->next = NULL;

	data->count++;

	if (data->count == 1) {
		data->elem = elem;
	}

	return 0;
}

cjson_value_t* cjson_array_get(const cjson_array_t *data, int index)
{
	cjson_value_t *ret = NULL;
	int i = 0;
	if (index < 0 || index >= data->count) {
		return NULL;
	}

	do {
		ret = data->elem;
		i++;
	} while (i <= index);
	
	for (i = 1; ; i++) {
		ret = ret->next;
	}

	return ret;
}

int cjson_array_free(cjson_array_t *val)
{
	int i = 0;
	int ret = 0;
	cjson_value_t *tmp = NULL;

	for (i = 0; i < val->count; i++) {
		tmp = val->elem;
		val->elem = val->elem->next;
		cjson_value_free(tmp);
	}

	return ret;
}

//===========================================================
// cjson object
int cjson_object_addkv(cjson_object_t *data, cjson_kv_t *kv)
{
	int i = 0;
	cjson_kv_t dummy;
	cjson_kv_t *tail = NULL;

	//printf("addkv 1: count: %d, value-addr: %p\n", data->count, data->kvs);

	dummy.next = data->kvs;
	tail = &dummy;
	for (i = 0; i < data->count; i++) {
		tail = tail->next;
	}

	tail->next = kv;
	kv->next = NULL;

	data->count++;

	if (data->count == 1) {
		data->kvs = kv;
	}

	//printf("addkv 2: count: %d, value: %s\n", data->count, data->kvs->value.cjson_strval);

	return 0;
}

int cjson_object_free(cjson_object_t *data)
{
	int i = 0;
	cjson_kv_t *tmp = NULL;

	for (i = 0; i < data->count; i++) {
		tmp = data->kvs;
		data->kvs = data->kvs->next;

		cjson_kv_free(tmp);
	}

	my_free(data);

	return 0;
}

cjson_kv_t* cjson_object_first(cjson_object_t *data, position_t *pos)
{
	if (data->count == 0) {
		return NULL;
	}

	*pos = (position_t)(data->kvs);

	return data->kvs;
}

cjson_kv_t* cjson_object_next(cjson_object_t *data, position_t *pos)
{
	cjson_kv_t *kv = NULL;

	if (pos == NULL) {
		return NULL;
	}

	kv = (cjson_kv_t*)(*pos);
	kv = kv->next;

	*pos = (position_t)(kv);

	return kv;
}

//===========================================================
int cjson_value_free(cjson_value_t *val)
{
	if (val->value_type == _cjson_value_object_) {
		cjson_object_free(val->cjson_objval);
	}
	else if (val->value_type == _cjson_value_array_) {
		cjson_array_free(val->cjson_arrval);
	}

	return 0;
}

int cjson_kv_free(cjson_kv_t *kv)
{
	if (kv->key) {
		my_free(kv->key);
	}

	cjson_value_free(&(kv->value));

	return 0;
}

cjson_value_t* cjson_object_get_value(const cjson_object_t *data, const tchar_t *key)
{
	int i = 0;
	cjson_value_t *ret = NULL;
	cjson_kv_t *kv = NULL;

	if (data == NULL) {
		//printf("data is NULL\n");
		return NULL;
	}

	kv = data->kvs;
	for (i = 0; i < data->count; i++) {
		//printf("get value key: %s\n", kv->key);
		if (strcmp(kv->key->s, key) == 0) {
			ret = &(kv->value);
			break;
		}
		kv = kv->next;
	}

	return ret;
}

cjson_value_t* cjson_array_first(cjson_array_t *data, position_t *pos)
{
	if (data->count == 0) {
		return NULL;
	}

	*pos = (position_t)(data->elem);

	return data->elem;
}

cjson_value_t* cjson_array_next(cjson_array_t *data, position_t *pos)
{
	cjson_value_t *val = NULL;

	if (pos == NULL) {
		return NULL;
	}

	val = (cjson_value_t*)(*pos);

	val = val->next;
	*pos = (position_t)(val);

	return val;
}

/********************************************************************
*        Methods
*********************************************************************/

//===============================================================
// '{' '}'
// '[' ']'
// '"' '"'
// ,
// :
// T/t for true
// F/f for false
// N/n for null
// [0 ~ 9] & +/- for number

static const tchar_t _token_fsm_table[128] = {
	// 0 ~ 15
	-1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,
	// 16 ~ 31
	-1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,
	// 32 ~ 47
	//      '"'                                     '+'   ',' '-'
	-1, -1,  0, -1,   -1, -1, -1, -1,   -1, -1, -1,  8,    6,  8, -1, -1,
	// 48 ~ 63  '3'   '4' '5' '6' '7'   '8' '9'':'
	 8,  8,  8,  8,    8,  8,  8,  8,    8,  8, 7, -1,    -1, -1, -1, -1,
	// 64 ~ 79                'F'                                 'N'
	-1, -1, -1, -1,   -1, -1,  9, -1,   -1, -1, -1, -1,   -1, -1, 10, -1,
	// 80 ~ 95        'T'                           '['   '\' ']'
	-1, -1, -1, -1,    9, -1, -1, -1,   -1, -1, -1,  2,    1,  3, -1, -1,
	// 96 ~ 111               'f'                                 'n'
	-1, -1, -1, -1,   -1, -1,  9, -1,   -1, -1, -1, -1,   -1, -1, 10, -1,
	// 112 ~ 127      't'                           '{'       '}'
	-1, -1, -1, -1,    9, -1, -1, -1,   -1, -1, -1,  4,   -1,  5, -1, -1,
};

//==============================================================

#define _token_stack_capacity_			(_token_stack_buf_size_ - 1) // nests depth

// stack top
#define _token_stack_top_max_			(_token_stack_capacity_ - 1)
// stack bottom 
#define _token_stack_bottom_			(-1)
struct __token_stack_t {
	union {
		struct {
			tchar_t	s[_token_stack_capacity_];
			tchar_t	top;
		} __s_un_data;
		tchar_t		__s_un_holder[_token_stack_buf_size_];
	} s_un;
#define stk_data		s_un.__s_un_data.s
#define stk_top			s_un.__s_un_data.top
};

typedef struct __token_stack_t			_stack_t;

#define _stack_top(stk)				(stk)->stk_data[(stk)->stk_top]
#define _stack_peek(stk)			_stack_top(stk)

int _stack_push(_stack_t *stk, tchar_t c)
{
	// stack full
	if (stk->stk_top == _token_stack_top_max_) {
		return -1;
	}
	
	stk->stk_top++;
	stk->stk_data[stk->stk_top] = c;
	//printf("=== push[%d]: %c\n", stk->stk_top,  c);
	return c;
}

int _stack_pop(_stack_t *stk)
{
	int ret = 0;

	// stop empty
	if (stk->stk_top == _token_stack_bottom_) {
		return -1;
	}
	//printf("=== pop[%d] %c\n", stk->stk_top, _stack_peek(stk));
	ret = stk->stk_data[stk->stk_top];
	stk->stk_top--;
	return ret;
}

//==============================================================
enum _decode_object_state_e {
	_object_key_expected_ = 0,
	_object_key_done_,
	_object_colon_done_,
	_object_value_expected_,
	_object_value_done_,
	_object_item_done_, // ',' proessed, key-value pair done
	
	_object_state_amount_
};
typedef enum _decode_object_state_e			decode_object_state_e;

enum _decode_array_state_e {
	_array_element_done_ = 0,
	_array_comma_done_,

	_array_state_amount_
};
typedef enum _decode_array_state_e			decode_array_state_e;

struct _decode_context_t {
	_stack_t					*stack;
	union {
		decode_object_state_e		object_state;
		decode_array_state_e		array_state;
	} s_un;
};

typedef struct _decode_context_t			decode_context_t;

// return characters length that processed
// return 0 for end of token processing
// return -1 for error
typedef int (*_pfn_cjson_decoder_t)(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_string(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_escape(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_array(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_array_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_object(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_object_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_item_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_colon_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_value_number(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_value_bool(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);
static int _decode_value_null(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx);

static const _pfn_cjson_decoder_t _token_handlers[] = {
	_decode_string,			// 0, string
	_decode_escape,			// 1
	_decode_array,			// 2, array
	_decode_array_done,		// 3
	_decode_object,			// 4, object
	_decode_object_done,	// 5
	_decode_item_done,		// 6, sperater between items
	_decode_colon_done,		// 7, sperater between key & value
	_decode_value_number,	// 8, number: 0 ~ 9
	_decode_value_bool,		// 9, true/false, TRUE/FALSE
	_decode_value_null		// 10, null/NULL
};

static int _decode_string(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx)
{
	//===========================================
	// onlye cares about these tokens:
	//		'"' '\'
	//===========================================
	int ret = 0;
	int i = 0;
	int _buf_size_ = CJSON_KEY_BUF_LEN;

	// '"' has been pushed into stack before, 
	// so, it's the second " that we met,
	// what means double-quotation end
	// double-quotation end
	if (_stack_peek(ctx->stack) == _T('"')) { // _stack_peek(ctx->stack) == json_text[i]
		// we done processing the string
		_stack_pop(ctx->stack);
		return 0;

	} else { // double-quotation start
		// it's the first '"' that we met
		// push token into stack

		//printf("=== _decode_string : %c\n", json_text[i+1]);

		ret = _stack_push(ctx->stack, json_text[i]);
		if (ret < 0) {
			return -1; // error
		}
		i++;
	}

	out_value->value_type = _cjson_value_string_;
	out_value->cjson_strval = (cjson_string_t*)my_malloc(sizeof(cjson_string_t) + _buf_size_);
	if (out_value->cjson_strval == NULL) {
		return -1;
	}
	out_value->cjson_strval->capacity = _buf_size_;
	out_value->cjson_strval->len = 0;

	while (json_text[i]) {
		//printf("[%d]%c\n", i, json_text[i]);
		if (json_text[i] == _T('\\') || json_text[i] == _T('"')) {
			// call token handler
			ret = _token_handlers[_token_fsm_table[json_text[i]]](&json_text[i], in_value, out_value, ctx);
			if (ret == 0) { // a pair of " done processing
				break;
			}
			if (ret == -1) {
				goto lbl_err;
			}

			i += ret;
			continue;
		}

		// just copy the character
		out_value->cjson_strval->s[out_value->cjson_strval->len] = json_text[i];
		out_value->cjson_strval->len++;
		i++;
	}

	out_value->cjson_strval->s[out_value->cjson_strval->len] = 0;
	//printf("%s\n", out_value->cjson_strval);
	return i + 1;

lbl_err:

	if (out_value->__value.strval) {
		my_free(out_value->__value.strval);
	}

	return -1;
}

static int _decode_escape(const tchar_t *json_text,cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx)
{
	int i = 0;

	//printf("=== _decode_escape\n");

	while (i < 2) { // \b
		out_value->cjson_strval->s[out_value->cjson_strval->len] = json_text[i];
		out_value->cjson_strval->len++;
		i++;
	}

	return 2;
}

// decode
static int _decode_array(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx)
{
#define _the_token_char_			_T('[')
	int ret = 0;
	int retv = 0;
	int i = 0;

	decode_context_t my_ctx;
	cjson_value_t my_out_data;
	cjson_value_t *elem = NULL;

	//printf("=== _decode_array\n");

	ret = _stack_push(ctx->stack, _the_token_char_);
	if (ret < 0) {
		return -1; // error
	}
	i++;

	// out value
	out_value->cjson_arrval = (cjson_array_t*)my_malloc(sizeof(cjson_array_t));
	if (out_value->cjson_arrval == NULL) {
		return -1;
	}
	memset(out_value->cjson_arrval, 0, sizeof(cjson_array_t));
	out_value->value_type = _cjson_value_array_;

	my_ctx.stack = ctx->stack;
	my_ctx.s_un.array_state = _array_element_done_;

	while (json_text[i] && _stack_peek(ctx->stack) == _the_token_char_) {
		if (_token_fsm_table[json_text[i]] == -1) { // ignore
			i++;
			continue;
		}
		
		// call token handler
		my_out_data.value_type = _cjson_value_unknown_;
		my_out_data.cjson_valptr = NULL;
		ret = _token_handlers[_token_fsm_table[json_text[i]]](&json_text[i], out_value, &my_out_data, &my_ctx);
		if (ret == 0) { // token end
			break;
		}
		
		if (ret < 0) {
			break;
		}

		// value ','
		switch (my_ctx.s_un.array_state)
		{
		case _array_element_done_:
			elem = (cjson_value_t*)my_malloc(sizeof(cjson_value_t));
			if (elem == NULL) {
				goto lbl_err;
			}
			elem->value_type = my_out_data.value_type;
			elem->cjson_valptr = my_out_data.cjson_valptr;
			elem->next = NULL;
			
			// mount element to out_value->cjson_arrval
			retv = cjson_array_add(out_value->cjson_arrval, elem);
			if (retv < 0) {
				goto lbl_err;
			}
			elem = NULL;

			break;
		case _array_comma_done_:
			my_ctx.s_un.array_state = _array_element_done_;
			break;
		default:
			break;
		}

		i += ret;
	}

	return i + 1;

lbl_err:

	if (out_value->cjson_arrval) {
		cjson_array_free(out_value->cjson_arrval);
	}

	return -1;
#undef _the_token_char_
}

// decode
static int _decode_array_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx)
{
	int ret = 0;

	//printf("=== _decode_array_done\n");
	if (_stack_peek(ctx->stack) == '[') {
		_stack_pop(ctx->stack);
		return 0;
	}
	
	return ret;
}

// decode object
static int _decode_object(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_value, decode_context_t *ctx)
{
#define _the_token_char_			_T('{')

	int ret = 0;
	int retv = 0;
	int i = 0;

	//printf("=== _decode_object\n");
	
	decode_context_t my_ctx;
	cjson_value_t my_out_data;
	cjson_kv_t *kv = NULL;

	ret = _stack_push(ctx->stack, _the_token_char_); // push token '{'
	if (ret < 0) {
		return -1; // error
	}
	i++;
	
	// out value
	out_value->cjson_objval = (cjson_object_t*)my_malloc(sizeof(cjson_object_t));
	if (out_value->cjson_objval == NULL) {
		return -1;
	}
	memset(out_value->cjson_objval, 0, sizeof(cjson_object_t));
	out_value->value_type = _cjson_value_object_;

	my_ctx.stack = ctx->stack;
	my_ctx.s_un.object_state = _object_key_expected_;

	while (json_text[i] && _stack_peek(ctx->stack) == _the_token_char_) {
		if (_token_fsm_table[json_text[i]] == -1) { // ignore
			i++;
			continue;
		}

		// call token handler
		my_out_data.value_type = _cjson_value_unknown_;
		my_out_data.cjson_valptr = NULL;
		ret = _token_handlers[_token_fsm_table[json_text[i]]](&json_text[i], out_value, &my_out_data, &my_ctx);
		if (ret == 0) { // token end
			break;
		}
		
		if (ret < 0) {
			goto lbl_err;
		}

		//printf("=== char: %c, object_state: %d\n", json_text[i], my_ctx.s_un.object_state);
		
		// key : value ','
		// "name": "John Doe",
		switch (my_ctx.s_un.object_state)
		{
		case _object_key_expected_:
			if (my_out_data.value_type == _cjson_value_string_) {
				kv = (cjson_kv_t*)my_malloc(sizeof(cjson_kv_t));
				if (kv == NULL) {
					goto lbl_err;
				}
				kv->key = my_out_data.cjson_strval;
				my_ctx.s_un.object_state++;

				//printf("create key: %s\n", kv->key);
			}
			break;
		case _object_key_done_: // string completely read
			break;
		case _object_colon_done_:
			my_ctx.s_un.object_state++;
			break;
		case _object_value_expected_:
			kv->value.value_type = my_out_data.value_type;
			kv->value.cjson_valptr = my_out_data.cjson_valptr;
			kv->value.next = NULL;
			// mount kv to out_value->cjson_objval
			retv = cjson_object_addkv(out_value->cjson_objval, kv);
			if (retv < 0) {
				goto lbl_err;
			}
			kv = NULL;
			my_ctx.s_un.object_state = _object_key_expected_;
			break;
		case _object_value_done_:
			my_ctx.s_un.object_state = _object_key_expected_;
			break;
		case _object_item_done_:
			my_ctx.s_un.object_state = _object_key_expected_;
			break;

		default:
			goto lbl_err;
			break;
		}

		i += ret;
	}

	return i + 1;

lbl_err:
	if (out_value->cjson_objval) {
		cjson_object_free(out_value->cjson_objval);
	}

	return -1;
#undef _the_token_char_
}

static int _decode_object_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
#define _the_token_char_				_T('{')
	int ret = 0;

	if (_stack_peek(ctx->stack) == _the_token_char_) {
		_stack_pop(ctx->stack);
		return 0;
	}
	
	return ret;
#undef _the_token_char_
}

// decode item done
static int _decode_item_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
	// proecess ','

	//printf("=== _decode_item_done\n");

	if (in_value->value_type == _cjson_value_object_ && ctx->s_un.object_state == _object_value_done_) {
		ctx->s_un.object_state = _object_item_done_;
	} else if (in_value->value_type == _cjson_value_array_ && ctx->s_un.array_state == _array_element_done_) {
		ctx->s_un.array_state = _array_comma_done_;
	}

	return 1;
}

static int _decode_colon_done(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
	// proecess ':'

	if (in_value->value_type == _cjson_value_object_ && ctx->s_un.object_state == _object_key_done_) {
		ctx->s_un.object_state = _object_colon_done_;
		//printf("=== _decode_colon_done, object_state: %d\n", ctx->s_un.object_state);
	}
	return 1;
}

static int _decode_value_number(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
	int ret = 0;
	int i = 0;
	int sign = 1;
	int exponent = 0;
	int exponent_sign = 1;

	//printf("=== _decode_value_number\n");

	out_data->cjson_numval = (cjson_number_t*)my_malloc(sizeof(cjson_number_t));
	if (out_data->cjson_numval == NULL) {
		return -1;
	}
	memset(out_data->cjson_numval, 0, sizeof(cjson_number_t));

	if (json_text[i] == _T('-')) {
		sign = -1;
		i++;
	} else if (json_text[i] == _T('+')) {
		i++;
	}

	while (json_text[i]) {
		if (json_text[i] >= _T('0') && json_text[i] <= _T('9')) {
			out_data->cjson_numval->number = out_data->cjson_numval->number * 10 + (json_text[i] - '0');
			out_data->cjson_numval->divisor *= 10;
		} else if (json_text[i] == _T('.')) {
			out_data->cjson_numval->divisor = 1;
		} else { // maybe json_text[i] == _T('e') || json_text[i] == _T('E')
			break;
		}

		i++;
	}

	out_data->cjson_numval->divisor = (out_data->cjson_numval->divisor == 0 ? 1 : out_data->cjson_numval->divisor); // to prevent from division by zero

	// scientific notation
	if (json_text[i] == _T('e') || json_text[i] == _T('E')) {
		i++; // skip 'e'
		if (json_text[i] == _T('-')) {
			exponent_sign = -1;
			i++;
		} else if (json_text[i] == _T('+')) {
			i++;
		}

		// e-5 : 10^-5
		while (json_text[i]) {
			if (json_text[i] >= _T('0') && json_text[i] <= _T('9')) {
				exponent = exponent * 10 + (json_text[i] - '0');
			} else {
				break;
			}

			i++;
		}
		
		while (exponent) {
			if (exponent_sign == -1) {
				out_data->cjson_numval->divisor *= 10;
			} else {
				out_data->cjson_numval->number *= 10;
			}
			exponent--;
		}
	}

	out_data->cjson_numval->number *= sign;
	out_data->value_type = _cjson_value_number_;
	return i + 1;

lbl_err:

	if (out_data->cjson_numval) {
		my_free(out_data->cjson_numval);
	}

	return -1;
}
static int _decode_value_bool(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
	int ret = 0;
	int i = 0;

	const tchar_t *true_str = _T("true");
	const tchar_t *false_str = _T("false");

	//printf("=== _decode_value_bool\n");

	// true
	i = 0;
	while (json_text[i] && true_str[i]) {
		if (json_text[i] == true_str[i]) {
			ret++;
			i++;
		} else {
			break;
		}
	}

	if (ret == 4) {
		out_data->value_type = _cjson_value_bool_;
		out_data->cjson_boolval = 1;
		return i + 1;
	}

	// false
	i = 0;
	ret = 0;
	while (json_text[i] && false_str[i]) {
		if (json_text[i] == false_str[i]) {
			ret++;
			i++;
		} else {
			break;
		}
	}

	if (ret == 5) {
		out_data->value_type = _cjson_value_bool_;
		out_data->cjson_boolval = 0;
		return i + 1;
	}
	
	out_data->value_type = _cjson_value_unknown_;
	return -1;
}
static int _decode_value_null(const tchar_t *json_text, cjson_value_t *in_value, cjson_value_t *out_data, decode_context_t *ctx)
{
	const uint32_t *p = (uint32_t*)_T("null");
	uint32_t *p2 = (uint32_t*)json_text;

	//printf("=== _decode_null\n");

	if (*p == *p2) {
		out_data->value_type = _cjson_value_null_;
		out_data->cjson_valptr = NULL;
		return sizeof(tchar_t) * 4;
	}

	return -1;
}

//===========================================================
// jsxon text => data
int cjson_decode(const tchar_t *json_text, cjson_t *data)
{
	int ret = 0;
	int i = 0;
	
	_stack_t *stk = NULL;
	
	decode_context_t ctx;

	cjson_value_t root_data;

	if (json_text == NULL || data == NULL) {
		return -1;
	}

	data->object = NULL;

	stk = (_stack_t*)my_malloc(sizeof(_stack_t));
	if (stk == NULL) {
		return -1;
	}
	memset(stk, 0, sizeof(_stack_t));
	stk->stk_top = _token_stack_bottom_;

	ctx.stack = stk;

	memset(&root_data, 0, sizeof(root_data));

	i = 0;
	while (json_text[i]) {
		if (_token_fsm_table[json_text[i]] == -1) {
			i++; // ignore
			continue;
		}

		// call token handler
		ret = _token_handlers[_token_fsm_table[json_text[i]]](&json_text[i], NULL, &root_data, &ctx);
		if (ret == 0) {
			break;
		}

		if (ret < 0) {
			break;
		}

		i += ret;
	}

	if (ret < 0) {
		goto release;
	}

	if (root_data.value_type != _cjson_value_object_) {
		goto release;
	}

	data->object = root_data.cjson_objval;

release:

	if (stk) {
		my_free(stk);
	}

	return (ret < 0 ? -1 : 0);
}

//gcc -I. cjson_decoder.c cjson_encoder.c -o cjson -g
int main(int argc, char *argv[])
{
	const char *text_json = "{\n"
        "\"name\": \"John \\\" Doe\",\n"
        "\"age\": +30,\n"
		"\"numbera\": -10000.98e-3,\n"
		"\"numberb\": 0.5,\n"
        "\"is_active\": true,\n"
        "\"hobbies\": [\"reading\", \"music\", \"sports\"],\n"
        "\"address\": {\n"
            "\"street\": \"123 Main St\",\n"
            "\"city\": \"New York\"\n"
			"\"zip\": null\n"
        "}\n"
    "}";

	char text[1024] = {0};
    
    cjson_t data;
    int result = cjson_decode(text_json, &data);
    if (result == 0) {
        // Successfully parsed
        cjson_value_t *name = cjson_object_get_value(data.object, "name");
        if (name && name->value_type == _cjson_value_string_) {
            printf("Name: \"%s\"\n", name->cjson_strval->s);
        }

		cjson_value_t *age = cjson_object_get_value(data.object, "age");
        if (age && age->value_type == _cjson_value_number_) {
            printf("age: %lld\n", age->cjson_numval->number / age->cjson_numval->divisor);
        }
		cjson_value_t *is_active = cjson_object_get_value(data.object, "is_active");
        if (is_active && is_active->value_type == _cjson_value_bool_) {
            printf("is_active: %d\n", is_active->cjson_boolval);
        }
		cjson_value_t *numbera = cjson_object_get_value(data.object, "numbera");
        if (numbera && numbera->value_type == _cjson_value_number_) {
			printf("numbera: %0.8lf\n", (float)(numbera->cjson_numval->number) / (float)(numbera->cjson_numval->divisor));
        }
		cjson_value_t *numberb = cjson_object_get_value(data.object, "numberb");
        if (numberb && numberb->value_type == _cjson_value_number_) {
			printf("numberb: %lf\n", (float)(numberb->cjson_numval->number) / (float)(numberb->cjson_numval->divisor));
        }
    } else {
        printf("Failed to parse json text\n");
    }

	printf("======================\n");
	cjson_encode(&data, text, sizeof(text));

	printf("%s\n", text);

	cjson_object_free(data.object);
    
    return 0;
}
