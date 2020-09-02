#include <stdio.h>
#include <stdlib.h>
#include "bt_rb.h"


void inorder(bt_node *x)
{
        if (x == NULL) {
                return;
        }
        inorder(x->left);
        printf(" %d ", x->val);
        inorder(x->right);
}


void print_in_order(bt_node *root)
{
        printf("in order: ");
        if (!root) {
                printf("tree is empty.\n");
        } else {
                inorder(root);
        }
        printf("\n");
}


void bt_dot_print_null(int key, int nullcount, FILE* stream)
{
        fprintf(stream, "    null%d [shape=point];\n", nullcount);
        fprintf(stream, "    %d -> null%d;\n", key, nullcount);
}


void bt_dot_print_aux(bt_node *node, FILE *stream)
{
        static int nullcount = 0;

        if (node->left) {
                fprintf(stream, "    %d -> %d;\n", node->val, node->left->val);
                bt_dot_print_aux(node->left, stream);
        } else {
                bt_dot_print_null(node->val, nullcount++, stream);
        }

        if (node->right) {
                fprintf(stream, "    %d -> %d;\n", node->val, node->right->val);
                bt_dot_print_aux(node->right, stream);
        } else {
                bt_dot_print_null(node->val, nullcount++, stream);
        }
}


void bt_dot_print(bt_node *tree, FILE *stream)
{
        fprintf(stream, "digraph BST {\n");
        fprintf(stream, "    node [fontname=\"Arial\"];\n");

        if (!tree) {
                fprintf(stream, "\n");
        } else if (!tree->right && !tree->left) {
                fprintf(stream, "    %d;\n", tree->val);
        } else {
                bt_dot_print_aux(tree, stream);
        }

        fprintf(stream, "}\n");
}



int main(void)
{

        bt_node *root = NULL;

        for (int i = 0; i < 31; i++) {

                bt_insert(&root, rand()/1000000);
        }

        FILE *dotlog = fopen("before_delete.dot", "w");
        bt_dot_print(root, dotlog);
        fclose(dotlog);

        dotlog = fopen("after_delete.dot", "w");
        bt_delete_by_val(&root, 719);
        bt_dot_print(root, dotlog);

        fclose(dotlog);

        return 0;
}


