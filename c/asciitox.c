
#define tchar_t             char

typedef long long           int64_t;
typedef unsigned long long  uint64_t;

typedef tchar_t             ape_word_t[2]; // double tchar_t

#define _T(s)               s

// ' ', '\t' -> skip
// '+', '-' -> sign
// '0' .. '9'
// 'a' .. 'f'
// 'A' .. 'F'
// '.'
// 'e', 'E'
static const tchar_t _ascii_dict[128] = {
    // 0 ~ 15
    -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,
    // 16 ~ 31
    -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,   -1, -1, -1, -1,
    // 32 ~ 47
    //      '"'                                     '+'   ',' '-'
    -1, -1,  0, -1,   -1, -1, -1, -1,   -1, -1, -1,  8,    6,  8, -1, -1,
    // 48 ~ 63  '3'   '4' '5' '6' '7'   '8' '9'':'
     0,  1,  2,  3,    4,  5,  6,  7,    8,  9,  0,  0,    0,  0,  0,  0,
    // 64 ~ 79                'F'                                 'N'
    -1, -1, -1, -1,   -1, -1,  9, -1,   -1, -1, -1, -1,   -1, -1, 10, -1,
    // 80 ~ 95        'T'                           '['   '\' ']'
    -1, -1, -1, -1,    9, -1, -1, -1,   -1, -1, -1,  2,    1,  3, -1, -1,
    // 96 ~ 111               'f'                                 'n'
    -1, -1, -1, -1,   -1, -1,  9, -1,   -1, -1, -1, -1,   -1, -1, 10, -1,
    // 112 ~ 127      't'                           '{'       '}'
    -1, -1, -1, -1,    9, -1, -1, -1,   -1, -1, -1,  4,   -1,  5, -1, -1,
};

static const unsigned short _hex_prefix_ = _T('0x');
static const unsigned tchar_t _oct_prefix_ = _T('0');

// ascii to int8_t
int c_atoi8(const tchar_t *s)
{
    char ret = 0;
    int sign = 1;
    int base = 10;

    // 0  -> Oct
    // 0x -> Hex

    if (base != 10 && base != 16) {
        return 0;
    }

    while (s && (*s == ' ' || *s == '\t')) {
        s++;
    }

    if () {

    }

    // check sign
    if (s && *s == '-') {
        sign = -1;
        s++;
    }

    while (s && ret < __INT8_MAX__) {
        ret *= base;
        ret += *s;
    }

    ret *= sign;

    return ret;
}

// ascii to uint8_t
unsigned int c_atou8(const tchar_t *s)
{
    unsigned char ret = 0;

    return ret;
}

// ascii to int16_t
int c_atoi16(const tchar_t *s)
{
    short ret = 0;
    int sign = 1;

    while (s && (*s == ' ' || *s == '\t')) {
        s++;
    }

    // check sign
    if (s && *s == '-') {
        sign = -1;
        s++;
    }

    while (s && ret < __INT16_MAX__) {
        ret *= 10;
        ret += *s;
    }

    ret *= sign;

    return ret;
}

// ascii to ushort
unsigned int c_atou16(const tchar_t *s)
{

}

// ascii to int
int c_atoi(const tchar_t *s)
{
    int n = 0;
    int sign = 1;

    // check sign
    while (s) {

    }

    if (*s == '-') {
        sign = -1;
        s++;
    }
}

// ascii to uint
unsigned int c_atoui(const tchar_t *s)
{

}


// ascii to long
long c_atol(const tchar_t *s)
{

}

// ascii to long
unsigned long c_atoul(const tchar_t *s)
{

}

// ascii to long long
long long c_atoll(const tchar_t *s)
{

}

// ascii to unsigned long long
unsigned long long c_atoull(const tchar_t *s)
{

}

// ascii to int64_t
int64_t c_atoi64(const tchar_t *s)
{

}

// ascii to uint64_t
uint64_t c_atou64(const tchar_t *s)
{

}

// ascii to float
float c_atof(const tchar_t *s)
{

}

// ascii to double
double c_atod(const tchar_t *s)
{

}
