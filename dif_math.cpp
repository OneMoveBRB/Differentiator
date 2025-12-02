#include "dif_math.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define cL TreeCopySubtree(node->left, node)
#define cR TreeCopySubtree(node->right, node)
#define dL TreeDiff(node->left, var)
#define dR TreeDiff(node->right, var)

#define c(x) \
    NodeInit(NULL, NULL, NULL, TYPE_NUMBER, x)
#define v(x) \
    NodeInit(NULL, NULL, NULL, TYPE_VARIABLE, x)

#define ADD_(left, right) \
    NodeInit(NULL, left, right, TYPE_OPERATION, OPERATION_ADD)

#define SUB_(left, right) \
    NodeInit(NULL, left, right, TYPE_OPERATION, OPERATION_SUB)

#define MUL_(left, right) \
    NodeInit(NULL, left, right, TYPE_OPERATION, OPERATION_MUL)

#define DIV_(left, right) \
    NodeInit(NULL, left, right, TYPE_OPERATION, OPERATION_DIV)

#define EXP_(left, right) \
    NodeInit(NULL, left, right, TYPE_OPERATION, OPERATION_EXP)

#define LN_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_LN)

#define SQRT_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_SQRT)

#define SIN_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_SIN)

#define COS_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_COS)

#define SINH_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_SINH)

#define COSH_(right) \
    NodeInit(NULL, NULL, right, TYPE_OPERATION, OPERATION_COSH)


Node_t* TreeDiff(Node_t* node, const char* var) {
    assert( node != NULL );
    assert( var != NULL );

    if (node->type == TYPE_NUMBER) {
        return c(0.f);
    }

    if (node->type == TYPE_VARIABLE) {
        if (strcmp(node->data.variable, var) == 0) {
            return c(1.f);
        } else {
            return v(node->data.variable);
        }
    }

    switch (node->data.operation) {
    case OPERATION_ADD:
        return ADD_(dL, dR);

    case OPERATION_SUB:
        return SUB_(dL, dR);

    case OPERATION_MUL:
        return ADD_(MUL_(dL, cR), MUL_(cL, dR));

    case OPERATION_DIV:
        return DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), EXP_(cR, c(2.f)));

    case OPERATION_EXP: {
        if (node->left->type == TYPE_NUMBER) {          // (a^x)` = (a^x * ln a) * x`
            return MUL_(MUL_(EXP_(cL, cR), LN_(cL)), dR);
        } else if (node->right->type == TYPE_NUMBER) {  // (x^a)` = (a * x ^ (a-1)) * x`
            return MUL_(MUL_(cR, EXP_(cL, SUB_(cR, c(1.f)))), dL);
        }                                               // (u^v)` = u^v * (v` * ln u + v/u * u`)

        return MUL_(EXP_(cL, cR), ADD_(MUL_(dR, LN_(cL)), MUL_(DIV_(cR, cL), dL)));
    }

    case OPERATION_SQRT:                                // sqrt(x)` = x` / (2 * sqrt(x))
        return DIV_(dR, MUL_(c(2.f), SQRT_(cR)));

    case OPERATION_LN:                                  // ln(x)` = x` / x
        return DIV_(dR, cR);

    case OPERATION_LOG:                                 // log(a, x)` = x` / (x * ln a)
        return DIV_(dR, MUL_(cR, LN_(cL)));

    case OPERATION_SIN:                                 // sin(x)` = cos(x) * x`
        return MUL_(COS_(cR), dR);

    case OPERATION_COS:                                 // cos(x)` = (0 - sin(x)) * x`
        return MUL_(SUB_(c(0.f), SIN_(cR)), dR);

    case OPERATION_TAN:                                 // tan(x)` = (1 / cos(x) ^ 2) * x`
        return MUL_(DIV_(c(1.f), EXP_(COS_(cR), c(2.f))), dR);

    case OPERATION_COT:                                 // cot(x)` = (0 - 1 / sin(x) ^ 2) * x`
        return MUL_(SUB_(c(0.f), DIV_(c(1.f), EXP_(SIN_(cR), c(2.f)))), dR);

    case OPERATION_SINH:                                // sinh(x)` = cosh(x) * x`
        return MUL_(COSH_(cR), dR);

    case OPERATION_COSH:                                // cosh(x)`= sinh(x) * x`
        return MUL_(SINH_(cR), dR);

    case OPERATION_TANH:                                // tanh(x)` = (1 / cosh(x) ^ 2) * x`
        return MUL_(DIV_(c(1.f), EXP_(COSH_(cR), c(2.f))), dR);

    case OPERATION_COTH:                                // coth(x)` = (0 - 1 / sinh(x) ^ 2) * x`
        return MUL_(SUB_(c(0.f), DIV_(c(1.f), EXP_(SINH_(cR), c(2.f)))), dR);

    case OPERATION_ASIN:                                // arcsin(x)` = x` / sqrt(1 - x^2)
        return DIV_(dR, SQRT_(SUB_(c(1.f), EXP_(cR, c(2.f)))));

    case OPERATION_ACOS:                                // arccos(x)` = 0 - x` / sqrt(1 - x^2)
        return SUB_(c(0.f), DIV_(dR, SQRT_(SUB_(c(1.f), EXP_(cR, c(2.f))))));

    case OPERATION_ATAN:                                // arctan(x)` = x` / (1 + x^2)
        return DIV_(dR, ADD_(c(1.f), EXP_(cR, c(2.f))));

    case OPERATION_ACOT:                                // arccot(x)` = 0 - x` / (1 + x^2)
        return SUB_(c(0.f), DIV_(dR, ADD_(c(1.f), EXP_(cR, c(2.f)))));

    default:
        fprintf(stderr, "TreeDiff: default\n");
        break;
    }

    // if (new_node->left) {
    //     new_node->left->parent = new_node;
    // }
    // if (new_node->right) {
    //     new_node->right->parent = new_node;
    // }

    return NULL;
}

/*
    OPERATION_SQRT,
    OPERATION_LN,
    OPERATION_LOG,
    OPERATION_SIN,
    OPERATION_COS,
    OPERATION_TAN,
    OPERATION_COT,
    OPERATION_SINH,
    OPERATION_COSH,
    OPERATION_TANH,
    OPERATION_COTH
*/

/*!SECTION
(
    "+"
    (
        "*"
        (
            "0" nil nil
        )
        (
            "x" nil nil
        )
    )
    (
        "1" nil nil
    )
)
*/
