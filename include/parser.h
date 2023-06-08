#ifndef _HTTP_PARSER_
#define _HTTP_PARSER_

#include <stdint.h>
#include <errno.h>
#include <stdio.h>

#define MAX_HEADERS 8
#define MAX_METHODS 6

#define ERR_REQ "Invalid request line"
#define ERR_SIZE "Incomplete header"
#define ERR_CRLF "Not terminated by CRLF"
#define ERR_VER "Invalid version"
#define ERR_METH "Invalid Method"

// check for space, throw error e 
#define CHECK_SPACE(p,l,e) if( (*p) != ' ' || l <= 0 ) { \
                        fprintf(stderr,"%s\n", e); \
                        return -1; }
// check for NULL character, throw e.                      
#define CHECK_NULL(p,e) if( (*p) == '\0') { \
                        fprintf(stderr,"%s\n", e); \
                        return -1; }

// check for CRLF at the position pointed by p
#define CHECK_CRLF(p,l,e) if( l < 2 || strncmp(p,"\r\n",2) != 0 ){ \
                            fprintf(stderr,"%s\n", e); \
                            return -1; }
#define CHECK_EOF(p,l) if ( l == 2 &&  strncmp(p,"\r\n\0", 3) == 0 ){ \
                            return 0; }
// check if a val is true, otherwise throw errro e.
#define CHECK_ERR(v,e) if(!v){ fprintf(stderr,"%s\n",e); return -1; } 

#define MOVE(p,l,b) p += b; l -= b; // move a pointer p by b bytes and reduce the bytes left by b bytes

#define CHECK_BUFF(buf_len,str_len) if( str_len + 1 >= buf_len) { \
                                        fprintf(stderr, "Buffer too small. Consider increasing the size.\n"); \
                                        return -1; }

extern const char* supported_methods[MAX_METHODS];

typedef struct _string_pointer {
    const char* ptr; // a pointer that points to start of the string
    uint16_t len;   // the length of the string, at max you can have 65535 bytes long string.
                    // which is sufficient for now.
} string_t;

typedef struct headers{
    string_t host;
    string_t connection;
    string_t content_type;
    string_t content_length;
    string_t accept[10];
    string_t accept_encoding[10];
    string_t accept_language[10];
} header_t;



typedef struct http_request_msg{
    header_t headers;
    char method[10];
    string_t url;
    char version[10];
} http_request_t;

void http_init(http_request_t* request);
int parse_http(char* msg, http_request_t* request);

int safe_strncmp(const char* dst, const char* src, size_t dst_len, int n);
int safe_strncmpcase(const char* dst, const char* src, size_t dst_len, int n);

int logger(string_t* pt);

int parse_headers(char* itr1, size_t left, http_request_t* request);
int parse_field(char* itr1, size_t left, string_t* header);

int get_url(char* buffer, size_t buf_len, http_request_t* request);
int get_content_length(size_t* len, http_request_t* request);
int get_content_type(char* buffer,size_t len, http_request_t* request);
int get_host(char* buffer,size_t len, http_request_t* request);

#endif