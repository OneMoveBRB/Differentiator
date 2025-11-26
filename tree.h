#ifndef TREE_H
#define TREE_H

#include <stddef.h>

#define FREE(ptr) free(ptr); ptr = NULL;

const int UNINITIALIZED = 0xBAD;

enum TreeElemType_t {
    TYPE_UNDEFINED,
    TYPE_OPERATION,
    TYPE_VARIABLE,
    TYPE_NUMBER
};

enum Operation_t {
    UNDEFINED_OPERATION,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    EXPONENTIATION
};

union TreeElem_t {
    Operation_t operation;
    char* variable;
    double number;
};

struct Node_t {
    TreeElemType_t type;
    TreeElem_t data;
    Node_t* parent;
    Node_t* left;
    Node_t* right;
};

struct Tree_t {
    Node_t* root;
    size_t size;
};

enum ArgType_t {
    ARG_UNDEFINED,
    ARG_TYPE,
    ARG_DATA,
    ARG_PARENT,
    ARG_LEFT,
    ARG_RIGHT
};

enum TreeErr_t {
    TREE_OK,
    TREE_ALLOCATION_FAILED,
    NODE_ALLOCATION_FAILED,
    TREE_PRINT_LATEX_FAILED,
    TREE_FILE_OPEN_FAILED,
    TREE_GET_FILE_SIZE_FAILED,
    TREE_BUFFER_FREAD_FAILED,
    TREE_SYNTAX_ERROR
};

typedef TreeErr_t (*TreeFunc)(Node_t**);

TreeErr_t TreeInit(Tree_t** tree);
TreeErr_t TreeDestroy(Tree_t** tree);

TreeErr_t NodeInit(Node_t** node, ...);
TreeErr_t NodeDestroy(Node_t** node);

TreeErr_t ReadTree(Tree_t* tree, char* file_name);
TreeErr_t PrintLatexTree(Tree_t* tree);
TreeErr_t TreeDifferentiation(Tree_t* tree, Tree_t* new_tree);
TreeErr_t ConstOptimization(Node_t* node, Tree_t* tree);

#endif // TREE_H