#ifndef _PARSER_ERR
#define _PARSER_ERR

enum {
    ERR_INV_REQ, // invalid request line
    ERR_INCOM_HEADER, // incomplete header
    ERR_VER, // invalid version
    ERR_CRLF, // the line isn't terminated by CRLF
    ERR_METHOD, // invalid method
    ERR_INV_SEP, // no separtor/ invalid sepator
    BUFF_OVERFLOW, // buffer overflow
    CONTENT_LEN_LARGE, // content_length is too large
    ERR_SPACE_FIELD, // space between header field and : 
    ERR_COLON, // expected colon
    ERR_INVALID_FIELD 
};

extern const char* _err_str[11];
extern int err_num;
const char* err_str(int err_num);

#endif