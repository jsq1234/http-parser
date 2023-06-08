#include "../include/parser.h"

int main(){
    http_request_t request;
    http_init(&request);

    char* msg = "GET /lmao_folder/lmao.js HTTP/1.1\r\nConnection: keep-alive\r\nHost: localhost:8888\r\n\r\n";
    printf("%s",msg);
    parse_http(msg, &request);

    char buffer[40] = "";
    get_url(buffer,40,&request);
    printf("%s %s %s\n",request.method,buffer,request.version);
    
    return 0;
}