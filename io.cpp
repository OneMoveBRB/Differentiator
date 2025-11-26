#include "io.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

struct OperationMapping {
    const char* value;
    Operation_t operation;
};

OperationMapping bin_ops[] = {
    {"U",   UNDEFINED_OPERATION},
    {"+",   ADDITION},
    {"-",   SUBTRACTION},
    {"*",   MULTIPLICATION},
    {"/",   DIVISION},
    {"^",   EXPONENTIATION}
};

size_t bin_ops_size = sizeof(bin_ops)/sizeof(bin_ops[0]);

IOErr_t BufferInit(Buffer_t* buffer, size_t capacity) {
    assert( buffer != NULL );

    buffer->size = 0;
    
    if (capacity != 0) {
        buffer->capacity = capacity;
        buffer->data = (char*)calloc(capacity, sizeof(char));
        if (buffer->data == NULL) {
            return IO_BUFFER_ALLOCATION_FAILED;
        }
    } else {
        buffer->capacity = 0;
        buffer->data = NULL;
    }
    

    return IO_OK;
}

IOErr_t BufferDestroy(Buffer_t* buffer) {
    assert( buffer != NULL );

    buffer->size = 0;
    buffer->capacity = 0;
    FREE(buffer->data);
    
    return IO_OK;
}

IOErr_t BufferGet(Buffer_t* buffer) {
    assert( buffer != NULL );

    buffer->size = (size_t)getline(&buffer->data, &buffer->capacity, stdin);
    if (buffer->size == (size_t)-1) {
        return IO_BUFFER_GETLINE_FAILED;
    }

    buffer->data[buffer->size - 1] = '\0';

    return IO_OK;
}

IOErr_t GetFileSize(const char* file_name, size_t* file_size) {
    assert( file_name != NULL );

	struct stat fileStatbuff;
	if ((stat(file_name, &fileStatbuff) != 0) || (!S_ISREG(fileStatbuff.st_mode))) {
		return IO_GET_FILE_SIZE_FAILED;
	}

    *file_size = (size_t)fileStatbuff.st_size;

	return IO_OK;
}

char* GetStringFromBuffer(char** position) {
    assert( position != NULL );
    
    char* left_idx = *position;
    if (*left_idx != '"') {
        return NULL;
    }

    char* right_idx = left_idx + 1; // skip '"'

    for (; *right_idx != '"'; ++right_idx);
    *right_idx = '\0';

    char* NodeName = strdup(left_idx + 1);
    *right_idx = '"';
    
    *position = right_idx + 1;      // skip '"'

    return NodeName;
}

IOErr_t DefineTreeElem(TreeElemType_t* type, TreeElem_t* data, char* str) {
    assert( str != NULL );

    for (size_t i = 0; i < bin_ops_size; i++) {
        if (strcmp(bin_ops[i].value, str) == 0) {
            *type = TYPE_OPERATION;
            data->operation = bin_ops[i].operation;

            return IO_OK;
        }
    }

    char* endptr = NULL;
    double temp_num = strtod(str, &endptr);
    if (*endptr == '\0') {
        *type = TYPE_NUMBER;
        data->number = temp_num;

        return IO_OK;
    }

    *type = TYPE_VARIABLE;
    data->variable = strdup(str);
    
    return IO_OK;
}

void SkipSpaces(char** position) { // FIXME SkipSpaces(char*) -> char*
    for (; isspace(**position); ++(*position));
}

const char* GetStrOp(Operation_t op) {
    return bin_ops[op].value;
}

char* MultiStrCat(size_t count, ...) {
    size_t size = 0;

    va_list args;
    va_start(args, count);

    for (size_t i = 0; i < count; i++) {
        size += strlen(va_arg(args, char*));
    }

    va_end(args);

    char* str = (char*)calloc(size + 1, sizeof(char));
    if (str == NULL) {
        return NULL;
    }

    va_start(args, count);

    for (size_t i = 0; i < count; i++) {
        strcat(str, va_arg(args, char*));
    }

    va_end(args);

    return str;
}

char* StrFromDouble(double x) {
    int size = snprintf(NULL, 0, "%lg", x);
    if (size < 0) {
        return NULL;
    }

    char* str = (char*)calloc((size_t)size + 1, sizeof(char));
    if (str == NULL) {
        return NULL;
    }

    snprintf(str, (size_t)size + 1, "%lg", x);

    return str;
}

double GetFuncOp(Operation_t operation, double a, double b) {
    switch (operation) {
    case ADDITION:
        return a + b;

    case SUBTRACTION:
        return a - b;

    case MULTIPLICATION:
        return a * b;

    case DIVISION:
        return a / b;

    case EXPONENTIATION:
        return pow(a, b);
    
    case UNDEFINED_OPERATION:
        fprintf(stderr, "UNDEFINED_OPERATION IN GetFuncOp\n");
    }

    return 0;
}

/*!SECTION
(
    "/"
    (
        "x" nil nil
    )
    (
        "*"
        (
            "/"
            (
                "1" nil nil
            )
            (
                "x" nil nil
            )
        )
        (
            "-"
            (
                "x" nil nil
            )
            (
                "6" nil nil
            )
        )
    )
)


*/