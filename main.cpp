#include <stdio.h>

#include "tree.h"
#include "dump.h"

int main() {
    Tree_t* tree = NULL;
    TreeInit(&tree);

    ReadTree(tree, "input.txt");

    PrintLatexTree(tree);

    Tree_t* dif_tree = NULL;
    TreeInit(&dif_tree);

    PrintLatexTree(tree);
    ConstOptimization(tree->root, tree);
    PrintLatexTree(tree);
    // TreeDifferentiation(tree, dif_tree);
    // PrintLatexTree(dif_tree);    

    DotVizualizeTree(dif_tree, "img.txt");

    TreeDestroy(&dif_tree);
    TreeDestroy(&tree);
    return 0;
}