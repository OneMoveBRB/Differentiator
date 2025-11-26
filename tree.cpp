#include "tree.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "io.h"
#include "dump.h"

#define va_arg_enum(type) ((type)va_arg(args, int))

static TreeErr_t InorderTraversal(Node_t* node, TreeFunc func);
static TreeErr_t PostorderTraversal(Node_t* node, TreeFunc func);

Node_t* RecursiveReadTree(char** position);
char* RecursiveLatexTree(Node_t* node);
Node_t* RecursiveDifferentiation(Node_t* node);

Node_t** GetParentNodePointer(Node_t* node);

TreeErr_t TreeInit(Tree_t** tree) {
    assert( tree != NULL );

    Tree_t* tree_ptr = (Tree_t*)calloc(1, sizeof(Tree_t));
    if (tree_ptr == NULL) {
        return TREE_ALLOCATION_FAILED;
    }

    tree_ptr->root = NULL;
    tree_ptr->size = 0;

    *tree = tree_ptr;

    return TREE_OK;
}

TreeErr_t TreeDestroy(Tree_t** tree) {
    assert( tree != NULL );

    if ((*tree)->root != NULL ) 
        PostorderTraversal((*tree)->root, NodeDestroy);

    FREE(*tree);

    return TREE_OK;
}

TreeErr_t NodeInit(Node_t** node, ...) {
    assert( node != NULL );

    Node_t* node_ptr = (Node_t*)calloc(1, sizeof(Node_t));
    if (node_ptr == NULL) {
        return NODE_ALLOCATION_FAILED;
    }

    node_ptr->parent = NULL;
    node_ptr->left = NULL;
    node_ptr->right = NULL;
    node_ptr->type = TYPE_UNDEFINED;
    node_ptr->data.variable = NULL;

    va_list args;
    va_start(args, node);

    ArgType_t current_type;
    while ( (current_type = va_arg_enum(ArgType_t)) != ARG_UNDEFINED ) {
        switch (current_type) {
        case ARG_TYPE:
            node_ptr->type = va_arg_enum(TreeElemType_t);
            break;

        case ARG_PARENT:
            node_ptr->parent = va_arg(args, Node_t*);
            break;

        case ARG_LEFT:
            node_ptr->left = va_arg(args, Node_t*);
            break;

        case ARG_RIGHT:
            node_ptr->right = va_arg(args, Node_t*);
            break;

        case ARG_DATA:
            switch (node_ptr->type) {
            case TYPE_OPERATION:
                node_ptr->data.operation = va_arg_enum(Operation_t);
                break;

            case TYPE_VARIABLE:
                node_ptr->data.variable = va_arg(args, char*);
                break;

            case TYPE_NUMBER:
                node_ptr->data.number = va_arg(args, double);
                break;
            
            default:
                fprintf(stderr, "WRONG TreeElemType_t\n");
                break;
            }
            break;
        
        default:
            break;
        }
    }

    va_end(args);

    *node = node_ptr;

    return TREE_OK;
}

TreeErr_t NodeDestroy(Node_t** node) {
    assert( node != NULL );

    Node_t* node_ptr = *node;
    
    switch (node_ptr->type)
    {
    case TYPE_NUMBER:
        node_ptr->data.number = 0;
        break;
    
    case TYPE_OPERATION:
        node_ptr->data.operation = UNDEFINED_OPERATION;
        break;

    case TYPE_VARIABLE:
        FREE(node_ptr->data.variable);
        break;
    
    default:
        break;
    }

    Node_t** parent_ptr = GetParentNodePointer(node_ptr);
    if (parent_ptr != NULL) {
        *parent_ptr = NULL;
    }

    node_ptr->parent = NULL;
    node_ptr->right = NULL;
    node_ptr->left = NULL;
    
    FREE(*node);

    return TREE_OK;
}

static TreeErr_t InorderTraversal(Node_t* node, TreeFunc func) {
    assert( node != NULL );

    printf("(");

    if (node->left != NULL) {
        InorderTraversal(node->left, func);
    }

    func(&node);

    if (node->right != NULL) {
        InorderTraversal(node->right, func);
    }

    printf(")");

    return TREE_OK;
}

static TreeErr_t PostorderTraversal(Node_t* node, TreeFunc func) {
    assert( node != NULL );

    printf("(");

    if (node->left != NULL) {
        PostorderTraversal(node->left, func);
        fprintf(stderr, "L\n");
    }

    if (node->right != NULL) {
        PostorderTraversal(node->right, func);
        fprintf(stderr, "R\n");
    }

    func(&node);

    printf(")");

    return TREE_OK;
}

// (5 * (x + 6))

TreeErr_t ReadTree(Tree_t* tree, char* file_name) {
    assert( tree != NULL );
    assert( file_name != NULL );

    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        return TREE_FILE_OPEN_FAILED;
    }

    size_t file_size = 0;
    if (GetFileSize(file_name, &file_size) == IO_GET_FILE_SIZE_FAILED) {
        return TREE_GET_FILE_SIZE_FAILED;
    }

    Buffer_t buffer;
    BufferInit(&buffer, file_size + 1);

    buffer.size = fread(buffer.data, sizeof(char), file_size, fp);
    buffer.data[buffer.size++] = '\0';
    if (buffer.size < buffer.capacity) {
        return TREE_BUFFER_FREAD_FAILED;
    }

    char* position = buffer.data;
    tree->root = RecursiveReadTree(&position);
    tree->root->parent = NULL;

    BufferDestroy(&buffer);

    fclose(fp);

    if (tree->root == NULL) {
        return TREE_SYNTAX_ERROR;
    }

    return TREE_OK;
}

Node_t* RecursiveReadTree(char** position) {
    assert( position != NULL );

    if (**position == '(') {
        Node_t* node = NULL;
        NodeInit(&node);

        ++(*position); // skip '('
        SkipSpaces(position);

        char* string = GetStringFromBuffer(position);
        DefineTreeElem(&node->type, &node->data, string);
        FREE(string);

        SkipSpaces(position);

        node->left  = RecursiveReadTree(position);
        node->right = RecursiveReadTree(position);

        if (node->left != NULL && node->right != NULL) {
            node->left->parent  = node;
            node->right->parent = node;
        }

        ++(*position); // skip ')'
        SkipSpaces(position);

        return node;

    } else if ((*position)[0] == 'n' && (*position)[1] == 'i' && (*position)[2] == 'l') {
        *position = *position + 3;
        SkipSpaces(position);
        return NULL;
    }

    fprintf(stderr, "ERROR IN RECREADTREE\n");
    return NULL;
}

TreeErr_t PrintLatexTree(Tree_t* tree) {
    assert( tree != NULL );

    if (tree->root == NULL) {
        return TREE_OK;
    }

    char* tex_str = RecursiveLatexTree(tree->root);
    if (tex_str == NULL) {
        fprintf(stderr, "ERROR IN RECLATEXTREE\n");
        return TREE_PRINT_LATEX_FAILED;
    }
    printf("%s\n", tex_str);

    FREE(tex_str);

    return TREE_OK;
}

char* RecursiveLatexTree(Node_t* node) {
    assert( node != NULL );

    if (node->type == TYPE_VARIABLE) {
        return strdup(node->data.variable);
    } else if (node->type == TYPE_NUMBER) {
        return StrFromDouble(node->data.number);
    }

    char* left = RecursiveLatexTree(node->left);
    char* right = RecursiveLatexTree(node->right);
    
    char* res = NULL;

    if (node->parent != NULL) {
        if (node->data.operation == DIVISION) {
            res = MultiStrCat(5, "\\frac{", left, "}{", right, "}");
        } else if ( (node->data.operation == ADDITION || node->data.operation == SUBTRACTION) 
             && (node->parent->data.operation == MULTIPLICATION) ) {
                res = MultiStrCat(5, "(", left, GetStrOp(node->data.operation), right, ")");
            }
        else {
            res = MultiStrCat(3, left, GetStrOp(node->data.operation), right);
        }
    } else {
        if (node->data.operation == DIVISION) {
            res = MultiStrCat(5, "\\frac{", left, "}{", right, "}");
        } else {
            res = MultiStrCat(3, left, GetStrOp(node->data.operation), right);
        }
    }

    FREE(left);
    FREE(right);

    return res;
}

TreeErr_t TreeDifferentiation(Tree_t* tree, Tree_t* new_tree) {
    assert( tree != NULL );
    assert( new_tree != NULL );

    if (tree->root == NULL) {
        return TREE_OK;
    }

    new_tree->root = RecursiveDifferentiation(tree->root);

    return TREE_OK;
}

// TODO: arg (x, y, z)
Node_t* RecursiveDifferentiation(Node_t* node) {
    assert( node != NULL );

    Node_t* new_node = NULL;
    NodeInit(&new_node);
    if (node->type == TYPE_OPERATION) {
        if (node->data.operation == ADDITION) {
            new_node->type = TYPE_OPERATION;
            new_node->data.operation = ADDITION;

            new_node->left = RecursiveDifferentiation(node->left);
            new_node->right = RecursiveDifferentiation(node->right);

        } else if (node->data.operation == SUBTRACTION) {
            new_node->type = TYPE_OPERATION;
            new_node->data.operation = SUBTRACTION;

            new_node->left = RecursiveDifferentiation(node->left);
            new_node->right = RecursiveDifferentiation(node->right);

        } else if (node->data.operation == MULTIPLICATION) {
            new_node->type = TYPE_OPERATION;
            new_node->data.operation = ADDITION;

            Node_t* mul1 = NULL; NodeInit(&mul1);
            Node_t* mul2 = NULL; NodeInit(&mul2);
            
            // new_node //TODO
        }

        new_node->left->parent = new_node;
        new_node->right->parent = new_node;

    } else if (node->type == TYPE_NUMBER) {
        new_node->type = TYPE_NUMBER;
        new_node->data.number = 0;
    } else if (node->type == TYPE_VARIABLE) {
        new_node->type = TYPE_NUMBER;
        new_node->data.number = 1;
    }

    return new_node;
}

TreeErr_t ConstOptimization(Node_t* node, Tree_t* tree) {
    assert( node != NULL );
    
    if (node->type == TYPE_NUMBER || node->type == TYPE_VARIABLE) {
        return TREE_OK;
    }

    Node_t* left = node->left;
    Node_t* right = node->right;

    if (node->left != NULL) {
        ConstOptimization(node->left, tree);
    }
    if (node->left != NULL) {
        ConstOptimization(node->right, tree);
    }
    
    if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
        node->data.number = GetFuncOp(node->data.operation, left->data.number, right->data.number);
        node->type = TYPE_NUMBER;

        PostorderTraversal(node->left, NodeDestroy);
        PostorderTraversal(node->right, NodeDestroy);

    } else if ((left->type == TYPE_NUMBER
                    && (left->data.number == 1 && node->data.operation == MULTIPLICATION
                        ||
                        left->data.number == 0 && node->data.operation == ADDITION))
                ||
               (right->type == TYPE_NUMBER
                    && ((right->data.number == 1
                              && (node->data.operation == MULTIPLICATION || node->data.operation == DIVISION
                                  || node->data.operation == EXPONENTIATION))
                         ||
                        (right->data.number == 0
                              && node->data.operation == ADDITION)))) {

        Node_t** parent_ptr = GetParentNodePointer(node);
        Node_t* alive_ptr = (left->type == TYPE_NUMBER) ? node->right : node->left;
        Node_t* dead_ptr = (alive_ptr == node->right) ? node->left : node->right;

        PostorderTraversal(dead_ptr, NodeDestroy);
        NodeDestroy(&node);

        if (parent_ptr == NULL) {
            tree->root = alive_ptr;
            alive_ptr->parent = NULL;
        } else {
            *parent_ptr = alive_ptr;
            alive_ptr->parent = *parent_ptr;
        }

    } else if ((left->type == TYPE_NUMBER 
                    && ((left->data.number == 1
                             && node->data.operation == EXPONENTIATION)
                         ||
                        (left->data.number == 0 
                             && (node->data.operation == MULTIPLICATION || node->data.operation == DIVISION))))
                ||
                (right->type == TYPE_NUMBER 
                    && right->data.number == 0 
                        && (node->data.operation == MULTIPLICATION || node->data.operation == EXPONENTIATION))) {

        node->type = TYPE_NUMBER;
        node->data.number = (node->data.operation == EXPONENTIATION) ? 1 : 0; // STUB - 3

        // NodeDestroy(&node->left);
        PostorderTraversal(node->left, NodeDestroy);
        PostorderTraversal(node->right, NodeDestroy);

    }

    return TREE_OK;
}

Node_t** GetParentNodePointer(Node_t* node) {
    assert( node != NULL );

    if (node->parent == NULL) {
        return NULL;
    }

    if (node->parent->left == node){
        return &node->parent->left;
    } else if (node->parent->right == node) {
        return &node->parent->right;
    }

    fprintf(stderr, "GetParentNodePointer failed!\n");
    return NULL;
}


/*
left->type == TYPE_NUMBER && left->data.number == 1 && node->data.operation == MULTIPLICATION
left->type == TYPE_NUMBER && left->data.number == 0 && node->data.operation == ADDITION
// TODO SUBTRACTION

right->type == TYPE_NUMBER && right->data.number == 1 && node->data.operation == MULTIPLICATION
right->type == TYPE_NUMBER && right->data.number == 1 && node->data.operation == DIVISION
right->type == TYPE_NUMBER && right->data.number == 1 && node->data.operation == EXPONENTIATION
right->type == TYPE_NUMBER && right->data.number == 0 && node->data.operation == ADDITION
*/

/*
left->type == TYPE_NUMBER && left->data.number == 1 && node->data.operation == EXPONENTIATION
right->type == TYPE_NUMBER && right->data.number == 0 && node->data.operation == EXPONENTIATION

left->type == TYPE_NUMBER && left->data.number == 0 
               && (node->data.operation == MULTIPLICATION || node->data.operation == DIVISION)
right->type == TYPE_NUMBER && right->data.number == 0 
               && (node->data.operation == MULTIPLICATION)
*/

/*!SECTION
    // if (node->data.operation == DIVISION) {
    //     res = MultiStrCat(5, "\\frac{", left, "}{", right, "}");
    // } else if (node->parent != NULL) {
    //     if ( (node->data.operation == ADDITION || node->data.operation == SUBTRACTION) 
    //          && (node->parent->data.operation == MULTIPLICATION) ) {
    //             res = MultiStrCat(5, "(", left, GetStrOp(node->data.operation), right, ")");
    //          }
    // } else {
    //     res = MultiStrCat(3, left, GetStrOp(node->data.operation), right);
    // }

TreeErr_t ConstOptimization(Node_t* node, Tree_t* tree) {
    assert( node != NULL );
    
    if (node->type == TYPE_NUMBER || node->type == TYPE_VARIABLE) {
        return TREE_OK;
    }

    Node_t* left = node->left;
    Node_t* right = node->right;

    if (node->left != NULL) {
        ConstOptimization(node->left, tree);
    }
    if (node->left != NULL) {
        ConstOptimization(node->right, tree);
    }
    
    if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
        node->data.number = GetFuncOp(node->data.operation, left->data.number, right->data.number);
        node->type = TYPE_NUMBER;

        NodeDestroy(&node->left);
        NodeDestroy(&node->right);

    } else if (left->type == TYPE_NUMBER
               && (left->data.number == 1 && node->data.operation == MULTIPLICATION
                   || left->data.number == 0 && node->data.operation == ADDITION)){

        Node_t** parent_ptr = GetParentNodePointer(node); // TODO SUBTRACTION
        Node_t* right_ptr = node->right;

        PostorderTraversal(node->left, NodeDestroy);

        if (tree->root == node){
            NodeDestroy(&tree->root);
        } else {
            NodeDestroy(&node);
        }

        if (parent_ptr == NULL) {
            tree->root = right_ptr;
            right_ptr->parent = NULL;
        } else {
            *parent_ptr = right_ptr;
            right_ptr->parent = *parent_ptr;
        } //STUB - 2

    } else if (right->type == TYPE_NUMBER
               && (right->data.number == 1
                        && (node->data.operation == MULTIPLICATION || node->data.operation == DIVISION
                            || node->data.operation == EXPONENTIATION)
                   ||
                   right->data.number == 0
                        && node->data.operation == ADDITION)) {
        Node_t** parent_ptr = GetParentNodePointer(node);
        Node_t* left_ptr = node->left;

        PostorderTraversal(node->right, NodeDestroy);

        if (tree->root == node){
            NodeDestroy(&tree->root);
        } else {
            NodeDestroy(&node);
        }

        if (parent_ptr == NULL) {
            tree->root = left_ptr;
            left_ptr->parent = NULL;
        } else {
            *parent_ptr = left_ptr;
            left_ptr->parent = *parent_ptr;
        } //STUB - 1

    } else if (left->type == TYPE_NUMBER && left->data.number == 1 && node->data.operation == EXPONENTIATION) {
        node->type = TYPE_NUMBER;
        node->data.number = 1; // STUB - 3

        NodeDestroy(&node->left);
        PostorderTraversal(node->right, NodeDestroy);

    } else if (left->type == TYPE_NUMBER && left->data.number == 0 
               && (node->data.operation == MULTIPLICATION || node->data.operation == DIVISION)) {
        node->type = TYPE_NUMBER;
        node->data.number = 0; // STUB - 3

        // NodeDestroy(&node->left);
        PostorderTraversal(node->left, NodeDestroy);
        PostorderTraversal(node->right, NodeDestroy);

    } else if (right->type == TYPE_NUMBER && right->data.number == 0 
               && (node->data.operation == MULTIPLICATION)) {
        node->type = TYPE_NUMBER;
        node->data.number = 0; // STUB - 3

        NodeDestroy(&node->right);
        PostorderTraversal(node->left, NodeDestroy); // TODO - EXPONENTION
        
    }

    return TREE_OK;
}
*/