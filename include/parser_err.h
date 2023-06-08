#ifndef _PARSER_ERR
#define _PARSER_ERR

enum {
    ERR_INV_REQ,
    ERR_INCOM_HEADER,
    ERR_VER,
    ERR_CRLF,
    ERR_METHOD,
    ERR_INV_SEP,
    BUFF_OVERFLOW,
    CONTENT_LEN_LARGE,
};
extern const char* _err_str[8];
extern int err_num;

const char* err_str(int err_num);

#endif