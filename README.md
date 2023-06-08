# HTTP - Parser

## Capabilites:
    1. Can parse HTTP GET request headers for now, giving you a data structure which you 
    can use to easily access the fields. 
    2. Doesn't allocates any memory, the headers are just pointers that point to the
     location where the header starts. You need to use get_* functions to have a copy of the text pointed by these pointers.
    3. Basic error handling.

## To-Do:
    1. Add support for more fields in the headers.
    2. Add POST suport.
    3. Add support to parse and create HTTP response headers.
