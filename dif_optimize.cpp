#include "dif_optimize.h"

#include <stdio.h>
#include <assert.h>

#include "io.h"
#include "utils.h"

#define cL TreeCopySubtree(node->left, node)
#define cR TreeCopySubtree(node->right, node)
#define c(x) \
    NodeInit(NULL, NULL, NULL, TYPE_NUMBER, x)

#define IS_VALUE(ptr, val) \
    (ptr->type == TYPE_NUMBER && isEqual(ptr->data.number, val))

static TreeElemType CreateConstNode(Node_t* node, Node_t** parent_ptr, Node_t* parent);
static TreeElemType ConstOptimizationAdd(Node_t* node, Node_t** parent_ptr, Node_t* parent);
static TreeElemType ConstOptimizationSub(Node_t* node, Node_t** parent_ptr, Node_t* parent);
static TreeElemType ConstOptimizationMul(Node_t* node, Node_t** parent_ptr, Node_t* parent);
static TreeElemType ConstOptimizationDiv(Node_t* node, Node_t** parent_ptr, Node_t* parent);
static TreeElemType ConstOptimizationExp(Node_t* node, Node_t** parent_ptr, Node_t* parent);

TreeElemType TreeOptimization(Tree_t* tree, Node_t* node) {
    assert( tree != NULL );
    assert( node != NULL );
    fprintf(stderr, "TreeOptimization: %p\n", node);
    getchar();

    TreeElemType  left_type = TYPE_UNDEFINED,
                   right_type = TYPE_UNDEFINED;
    if (node->left) {
        left_type = TreeOptimization(tree, node->left);
    }
    if (node->right) {
        right_type = TreeOptimization(tree, node->right);
    }

    Node_t** parent_ptr = (node->parent) ? GetParentNodePointer(node) : &tree->root;
    if (left_type == TYPE_NUMBER && right_type == TYPE_NUMBER) {
        return CreateConstNode(node, parent_ptr, node->parent);
    }

    if (node->type != TYPE_OPERATION) {
        return node->type;
    }

    switch (node->data.operation) {
    case OPERATION_ADD:
        return ConstOptimizationAdd(node, parent_ptr, node->parent);

    case OPERATION_SUB:
        return ConstOptimizationSub(node, parent_ptr, node->parent);

    case OPERATION_MUL:
        return ConstOptimizationMul(node, parent_ptr, node->parent);

    case OPERATION_DIV:
        return ConstOptimizationDiv(node, parent_ptr, node->parent);

    case OPERATION_EXP:
        return ConstOptimizationExp(node, parent_ptr, node->parent);
    
    default:
        break;
    }

    return node->type;
}

TreeElemType CreateConstNode(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "CreateConstNode: %p\n", node);
    getchar();

    double value = GetFuncOp(node->data.operation, 
                                node->left->data.number, 
                                node->right->data.number);

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = NodeInit(parent, NULL, NULL, TYPE_NUMBER, value);

    return TYPE_NUMBER;
}

#define STR(x_) #x_

#define ConstOtimizationHandler(func_name, expressions)                                     \
static TreeElemType func_name(Node_t* node, Node_t** parent_ptr, Node_t* parent) {          \
    assert( node != NULL );                                                                 \
    assert( parent_ptr != NULL );                                                           \
    fprintf(stderr, STR(func_name)": %p\n", node);                                          \
    getchar();                                                                              \
                                                                                            \
    Node_t* new_node = NULL;                                                                \
                                                                                            \
    expressions                                                                             \
                                                                                            \
    PostorderTraversal(node, NodeDestroy);                                                  \
    *parent_ptr = new_node;                                                                 \
    new_node->parent = parent;                                                              \
                                                                                            \
    return TYPE_OPERATION;                                                                  \
}

ConstOtimizationHandler(
    ConstOptimizationAdd,
    if      (IS_VALUE(node->left,  0.f)) new_node = cR;
    else if (IS_VALUE(node->right, 0.f)) new_node = cL;
    else    return TYPE_OPERATION;
)

ConstOtimizationHandler(
    ConstOptimizationSub,
    if      (IS_VALUE(node->right, 0.f) ) new_node = cL;
    else    return TYPE_OPERATION;
)

ConstOtimizationHandler(
    ConstOptimizationMul,
    if      (IS_VALUE(node->left,  0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->right, 0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->left,  1.f) ) new_node = cR;
    else if (IS_VALUE(node->right, 1.f) ) new_node = cL;
    else    return TYPE_OPERATION;
)

ConstOtimizationHandler(
    ConstOptimizationDiv,
    if      (IS_VALUE(node->left,  0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->right, 1.f) ) new_node = cL;
    else    return TYPE_OPERATION;
)

ConstOtimizationHandler(
    ConstOptimizationExp,
    if      (IS_VALUE(node->right, 0.f)) new_node = c(1.f);
    else if (IS_VALUE(node->left,  1.f)) new_node = c(1.f);
    else if (IS_VALUE(node->right, 1.f)) new_node = cL;
    else    return TYPE_OPERATION;
)


/*

TreeElemType ConstOptimizationAdd(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "ConstOptimizationAdd: %p\n", node);
    getchar();

    Node_t* new_node = NULL;
    
    if      (IS_VALUE(node->left,  0.f)) new_node = cR;
    else if (IS_VALUE(node->right, 0.f)) new_node = cL;
    else    return TYPE_OPERATION;

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = new_node;
    new_node->parent = parent;

    return TYPE_OPERATION;
}

TreeElemType ConstOptimizationSub(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "ConstOptimizationSub: %p\n", node);
    getchar();

    Node_t* new_node = NULL;
    
    if      (IS_VALUE(node->right, 0.f) ) new_node = cL;
    else    return TYPE_OPERATION;

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = new_node;
    new_node->parent = parent;

    return TYPE_OPERATION;
}

TreeElemType ConstOptimizationMul(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "ConstOptimizationMul: %p\n", node);
    getchar();

    Node_t* new_node = NULL;
    
    if      (IS_VALUE(node->left,  0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->right, 0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->left,  1.f) ) new_node = cR;
    else if (IS_VALUE(node->right, 1.f) ) new_node = cL;
    else    return TYPE_OPERATION;

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = new_node;
    new_node->parent = parent;

    return TYPE_OPERATION;
}

TreeElemType ConstOptimizationDiv(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "ConstOptimizationDiv: %p\n", node);
    getchar();

    Node_t* new_node = NULL;
    
    if      (IS_VALUE(node->left,  0.f) ) new_node = c(0.f);
    else if (IS_VALUE(node->right, 1.f) ) new_node = cL;
    else    return TYPE_OPERATION;

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = new_node;
    new_node->parent = parent;

    return TYPE_OPERATION;
}

TreeElemType ConstOptimizationExp(Node_t* node, Node_t** parent_ptr, Node_t* parent) {
    assert( node != NULL );
    assert( parent_ptr != NULL );
    fprintf(stderr, "ConstOptimizationExp: %p\n", node);
    getchar();

    Node_t* new_node = NULL;
    
    if      (IS_VALUE(node->right, 0.f)) new_node = c(1.f);
    else if (IS_VALUE(node->left,  1.f)) new_node = c(1.f);
    else if (IS_VALUE(node->right, 1.f)) new_node = cL;
    else    return TYPE_OPERATION;

    PostorderTraversal(node, NodeDestroy);
    *parent_ptr = new_node;
    new_node->parent = parent;

    return TYPE_OPERATION;
}

*/
