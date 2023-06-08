#include "../include/parser.h"
#include "../include/parser_err.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *supported_methods[] = {"GET",    "POST", "UPDATE",
                                   "DELETE", "HEAD", "PUT"};

void http_init(http_request_t *request) {
  request->url.ptr = NULL;
  request->url.len = 0;
  memset(&request->headers, 0, sizeof(request->headers));
  request->method[0] = '\0';
  request->version[0] = '\0';
}

int parse_http(char *msg, http_request_t *request) {

  size_t msg_len = strlen(msg);
  char *itr1 = msg;          // the iterator that will traverse the string.
  char *itr2 = msg;          // another iterator for scanning
  size_t left = strlen(msg); // the size of remaining string when we have
                             // traversed it and at a position, say i.

  if (msg_len < 4) { // if the msg len is less than 4, we can be sure that it
                     // doesn't have \r\n\r\n at the end
    // to-do : Add better error handling. For now just return -1 for any errors.
    err_num = ERR_INCOM_HEADER;
    return -1;
  }

  if (strncmp(itr1 + msg_len - 4, "\r\n\r\n", 4) != 0) {
    // the string doesn't end with two CRLF, no need to parse it. It's invalid.
    err_num = ERR_INCOM_HEADER;
    return -1;
  }

  // parse the method
  int method_valid = 0;
  for (int i = 0; i < MAX_METHODS; i++) {
    const char *method = supported_methods[i];
    size_t m_len = strlen(method);
    if (safe_strncmp(itr1, method, left, m_len) == 1) {
      strncpy(request->method, method, m_len);
      method_valid = 1;
      MOVE(itr1, left, m_len)
      break;
    }
  }

  CHECK_ERR(method_valid, ERR_METHOD)
  CHECK_SPACE(itr1, left,
              ERR_INV_SEP) // if there is no space, throw an error and return -1

  MOVE(itr1, left, 1); // move by 1 since there is a space
  itr2 = itr1;

  // parse URL
  int valid_url = 0;

  while (*itr2 != ' ') {
    if (*itr2 == '\0')
      break;
    itr2++;
  }

  CHECK_NULL(itr2, ERR_INCOM_HEADER) // check if we have exhausted our string.
                                     // Return error if we have.
  CHECK_SPACE(itr2, left, ERR_INV_SEP);

  // URL is just a pointer to the start of the url and the length of the url
  // It is NOT a null-terminated string
  // DO NOT print it with printf, instead use logger(string_t*)
  int l = itr2 - itr1;
  request->url.ptr = itr1;
  request->url.len = l;

  MOVE(itr1, left, l + 1) // move by l + 1 ( +1 for the space )

  const char *supported_versions[] = {
      "HTTP/1.0",
      "HTTP/1.1",
  };

  int valid_version = 0;
  if (safe_strncmp(itr1, "HTTP/1.0", left, 8) == 1 ||
      safe_strncmp(itr1, "HTTP/1.1", left, 8) == 1) {
    valid_version = 1;
    strncpy(request->version, itr1, 8);
    MOVE(itr1, left, 8)
  }

  CHECK_ERR(valid_version, ERR_VER)
  CHECK_CRLF(itr1, left, ERR_CRLF) // check if the line is terminated by \r\n
  MOVE(itr1, left, 2)              // move by two if it is

  // if it has '\r\n' then we have parsed all
  // return from the function then
  CHECK_EOF(itr1, left)
  // otherwise move by two characters

  int by = 0;
  while (1) {
    if ((by = parse_headers(itr1, left, request)) == -1) {
      return -1;
    }
    MOVE(itr1, left, by)
    CHECK_EOF(itr1, left)
  }

  return 0;
}
int parse_field(char *itr1, size_t left, string_t *header) {
  int by = 0; // how many bytes have we covered;
  if (left > 0 && *itr1 != ' ') {
    err_num = ERR_COLON;
    return -1;
  }
  if (left > 0 && *itr1 == ' ') {
    MOVE(itr1, left, 1)
    by += 1;
  } else {
    err_num = ERR_INV_SEP;
    return -1;
  }
  const char *itr2 = itr1;

  while (*itr2 != '\r') {
    if (*itr2 == ':' && *(itr1 + 1) == ' ') {
      err_num = ERR_INV_SEP;
      return -1;
    }
    ++itr2;
  }

  header->ptr = itr1;
  header->len = itr2 - itr1;

  MOVE(itr1, left, itr2 - itr1)

  CHECK_CRLF(itr1, left,
             ERR_CRLF) // if it doesn't end with \r\n, throw error and return

  // if it does, move by two characters.
  MOVE(itr1, left, 2)

  by += header->len + 2;
  return by;
}
int parse_headers(char *itr1, size_t left, http_request_t *request) {

  int by = 0;
  int a = 0;
  if ((a = safe_strncmpcase(itr1, "Host", left, 4))) {
    if (a == -1) {
      return -1;
    }
    MOVE(itr1, left, 5)
    // printf("it is host!\n");
    by = parse_field(itr1, left, &request->headers.host);
    by = (by >= 0) ? by + 5 : -1;

  } else if ((a = safe_strncmpcase(itr1, "Connection", left, 10))) {
    if (a == -1) {
      return -1;
    }
    MOVE(itr1, left, 11)
    by = parse_field(itr1, left, &request->headers.connection);
    by = (by >= 0) ? by + 11 : -1;
  } else if ((a = safe_strncmpcase(itr1, "Content-Type", left, 12))) {
    if (a == -1) {
      return -1;
    }
    MOVE(itr1, left, 13)
    by = parse_field(itr1, left, &request->headers.content_type);
    by = (by >= 0) ? by + 13 : -1;
  } else if ((a = safe_strncmpcase(itr1, "Content-Length", left, 14))) {
    if (a == -1) {
      return -1;
    }
    MOVE(itr1, left, 15)
    by = parse_field(itr1, left, &request->headers.content_length);
    by = (by >= 0) ? by + 15 : -1;
  } else {
    err_num = ERR_INVALID_FIELD;
    return -1;
  }
  return by;
}

int safe_strncmpcase(const char *dst, const char *src, size_t dst_len, int n) {
  if (n > dst_len) {
    err_num = BUFF_OVERFLOW;
    return -1;
  }
  return strncasecmp(dst, src, n) == 0;
}

int safe_strncmp(const char *dst, const char *src, size_t dst_len, int n) {
  if (n > dst_len) {
    // if proceded, will end with index out of bounds
    err_num = BUFF_OVERFLOW;
    return -1;
  }

  return strncmp(src, dst, n) == 0;
}

int logger(string_t *s) {
  int t = s->len;
  const char *p = s->ptr;
  while (t) {
    printf("%c", *p);
    p++;
    t--;
  }
  printf("\n");
  return 0;
}

int get_url(char *buffer, size_t buf_len, http_request_t *request) {
  uint16_t len = request->url.len;
  CHECK_BUFF(buf_len, len)
  strncpy(buffer, request->url.ptr, request->url.len);
  return 0;
}

int get_host(char *buffer, size_t buf_len, http_request_t *request) {
  string_t *p = &request->headers.host;
  uint16_t len = p->len;
  CHECK_BUFF(buf_len, len)
  strncpy(buffer, p->ptr, len);
  return 0;
}

int get_content_type(char *buffer, size_t buf_len, http_request_t *request) {
  string_t *p = &request->headers.content_type;
  uint16_t len = p->len;
  CHECK_BUFF(buf_len, len)
  strncpy(buffer, p->ptr, len);
  return 0;
}

int get_content_length(size_t *len, http_request_t *request) {
  string_t *p = &request->headers.content_length;
  if (p->len >= 21) {
    fprintf(stderr, "content-length too large.\n");
    return -1;
  }
  char buffer[21] = "";
  strncpy(buffer, p->ptr, p->len);
  *len = strtoul(buffer, 0, 10);
  if (*len == ULONG_MAX && errno == ERANGE) {
    fprintf(stderr, "content-length too large\n");
    *len = 0;
    return -1;
  }

  return 0;
}
