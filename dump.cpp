#include "dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "io.h"

static size_t DotInitNodes(Node_t* node, FILE* fp, size_t* node_cnt);

void DotVizualizeTree(const Tree_t* tree, const char* filename) {
    assert( tree != NULL );
    assert( filename != NULL );

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        return; 
    }

    fprintf(fp, "digraph Tree {\n\t");
    fprintf(fp, "rankdir=HR;\n\t");
    fprintf(fp, "node [shape=record, style=filled, fillcolor=lightblue];\n\t");
    fprintf(fp, "edge [fontsize=10,  color=black];\n\n\t");

    size_t node_cnt = 0;
    DotInitNodes(tree->root, fp, &node_cnt);

    fprintf(fp, "\n}");

    fclose(fp);

    system("dot -Tsvg img.txt > img.svg");

    return;
}

static size_t DotInitNodes(Node_t* node, FILE* fp, size_t* node_cnt) {
    assert( node != NULL );
    assert(  fp != NULL  );

    // fprintf(stderr, "NOde: %p\n", node);
    size_t left_node = 0, right_node = 0;
    if (node->left != NULL) {
        left_node = DotInitNodes(node->left, fp, node_cnt);
    }

    if (node->right != NULL) {
        right_node = DotInitNodes(node->right, fp, node_cnt);
    }

    ++(*node_cnt);

    if (node->type == TYPE_NUMBER)
        fprintf(fp, "node%zu [label=\"{{{<f0> %p | <f1> type = NUMBER | <f2> data = %lg}} | { <f3> left: %p | <f4> right: %p}}\"];\n\t", 
                *node_cnt, node, node->data.number, node->left, node->right);
    else if (node->type == TYPE_OPERATION) 
        fprintf(fp, "node%zu [label=\"{{{<f0> %p | <f1> type = OPERATION | <f2> data = %s}} | { <f3> left: %p | <f4> right: %p}}\"];\n\t", 
                *node_cnt, node, GetStrOp(node->data.operation), node->left, node->right);
    else if (node->type == TYPE_VARIABLE)
        fprintf(fp, "node%zu [label=\"{{{<f0> %p | <f1> type = VARIABLE | <f2> data = %s}} | { <f3> left: %p | <f4> right: %p}}\"];\n\t", 
                *node_cnt, node, node->data.variable, node->left, node->right);

    if (node->left != NULL) {
        if (node->left->parent == node) {
            fprintf(fp, "node%zu:f3 -> node%zu [color=red, dir=both, arrowhead=normal];\n\t", *node_cnt, left_node);
        } else {
            fprintf(fp, "node%zu:f3 -> node%zu [color=red, arrowhead=normal];\n\t", *node_cnt, left_node);
            fprintf(fp, "node%zu -> node%zu:f3 [color=red, arrowhead=normal];\n\t", left_node, *node_cnt);
        }
    }
    if (node->right != NULL) {
        if (node->right->parent == node) {
            fprintf(fp, "node%zu:f4 -> node%zu [color=green, dir=both, arrowhead=normal];\n\t", *node_cnt, right_node);
        } else {
            fprintf(fp, "node%zu:f4 -> node%zu [color=green, arrowhead=normal];\n\t", *node_cnt, right_node);
            fprintf(fp, "node%zu -> node%zu:f4 [color=green, arrowhead=normal];\n\t", right_node, *node_cnt);
        }
    }

    return *node_cnt;
}

/*!SECTION
digraph Differentiator {
    rankdir=HR;
	node [shape=record, style=filled, fillcolor=lightblue];
	edge [fontsize=10,  color=black];

	node0 [label="{{{<f0> 0x23424141 | <f1> type = TYPE_OPERATION | <f2> data = MUL}} | { <f3> left: 0x23424141 | <f4> right: 0x23424141}}"];
	node1 [label="{{{<f0> 0x23424141 | <f1> type = TYPE_OPERATION | <f2> data = MUL}} | { <f3> left: 0x23424141 | <f4> right: 0x23424141}}"];
	node2 [label="{{{<f0> 0x23424141 | <f1> type = TYPE_OPERATION | <f2> data = MUL}} | { <f3> left: 0x23424141 | <f4> right: 0x23424141}}"];
	
	{ rank = same; node0; }
	{ rank = same; node1; node2; }
	
	node0:f3 -> node1 [color=blue, arrowhead=normal];
	node0:f4 -> node2 [color=blue, arrowhead=normal];
	
}
*/