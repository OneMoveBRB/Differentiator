#include <stdio.h>

#include "tree.h"
#include "dif_math.h"
#include "dif_optimize.h"
#include "dump.h"


int main() {
    // Node_t* node = NodeInit(NULL, NULL, NULL, TYPE_NUMBER, 1.0);

    // printf("%d\n", node->type);
    // printf("%lg\n", node->data.number);

    // NodeDestroy(&node);

    Tree_t* tree = NULL;
    TreeInit(&tree);

    ReadTree(tree, "input.txt");

    // PrintLatexTree(tree);

    DotVizualizeTree(tree, "img.txt");
    getchar();

    // TreeOptimization(tree, tree->root);

    // DotVizualizeTree(tree, "img.txt");
    // getchar();

    Tree_t* tree2 = NULL;
    TreeInit(&tree2);

    tree2->root = TreeDiff(tree->root, "x");
    tree2->root->parent = NULL;

    // TreeOptimization(tree2, tree2->root);
    // TreeOptimization(tree2, tree2->root);
    // TreeOptimization(tree2, tree2->root);
    // PrintLatexTree(tree2);
    PrintTree(tree2);

    DotVizualizeTree(tree2, "img.txt");
    getchar();

    TreeDestroy(&tree2);
    
    
    TreeDestroy(&tree);
    return 0;
}
