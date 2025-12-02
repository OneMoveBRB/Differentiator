#ifndef IO_H
#define IO_H

#include "tree.h"

enum IOErr_t {
    IO_OK,
    IO_WRONG_VALUE,
    IO_SYNTAX_ERROR,
    IO_NODE_INIT_FAILED,
    IO_FILE_OPEN_FAILED,
    IO_GET_TREE_ELEMENT_FAILED,
    IO_GET_FILE_SIZE_FAILED,
    IO_BUFFER_FREAD_FAILED,
    IO_BUFFER_GETLINE_FAILED,
    IO_BUFFER_ALLOCATION_FAILED
};

struct Buffer_t {
    char* data;
    size_t size;
    size_t capacity;
};

IOErr_t GetFileSize(const char* file_name, size_t* file_size);

IOErr_t BufferInit(Buffer_t* buffer, size_t capacity);
IOErr_t BufferDestroy(Buffer_t* buffer);
IOErr_t BufferGet(Buffer_t* buffer);

IOErr_t DefineTreeElem(TreeElemType* type, TreeElem_t* data, char* str);

char* GetStringFromBuffer(char** position);
void SkipSpaces(char** position);

const char* GetStrOp(Operation_t op);

char* MultiStrCat(size_t count, ...);
char* StrFromDouble(double x);
double GetFuncOp(Operation_t operation, double a, double b);
#endif // IO_H
