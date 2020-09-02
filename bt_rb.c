#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bt_rb.h"


static bt_node *bt_node_create(int val)
{
        bt_node *tmp;

        tmp = (bt_node *)malloc(sizeof(bt_node));
        if (!tmp) {
                return NULL;
        }

        tmp->parent = NULL;
        tmp->left = NULL;
        tmp->right = NULL;
        tmp->color = RED;
        tmp->val = val;

        return tmp;
}


static inline bool bt_is_left_child(bt_node *e)
{
        return e == e->parent->left;
}


static bt_node *bt_uncle(bt_node *n)
{
        if (!n->parent || !n->parent->parent) {
                return NULL;
        }

        /* if the parent is the left child of its own parent,
         * then the uncle is the right child of its parent */
        if (bt_is_left_child(n->parent)) {
                return n->parent->parent->right;
        } else {
                return n->parent->parent->left;
        }
}


static bt_node *bt_sibling(bt_node *n)
{
        if (!n->parent) {
                return NULL;
        }

        if (bt_is_left_child(n)) {
                return n->parent->right;
        }

        return n->parent->left;
}



/* moves node down and moves given node in its place */
static void bt_move_down(bt_node *n, bt_node *nParent)
{
        if (n->parent) {
                if (bt_is_left_child(n)) {
                        n->parent->left = nParent;
                } else {
                        n->parent->right = nParent;
                }
        }
        nParent->parent = n->parent;
        n->parent = nParent;
}


static bool bt_has_red_child(bt_node *n)
{
        return (n->left && n->left->color == RED) ||
                (n->right && n->right->color == RED);
}


static void bt_rotate_left(bt_node **root, bt_node *x)
{
        bt_node *nParent = x->right;

        if (x == *root) {
                *root = nParent;
        }

        bt_move_down(x, nParent);
        x->right = nParent->left;

        if (nParent->left) {
                nParent->left->parent = x;
        }

        nParent->left = x;
}


static void bt_rotate_right(bt_node **root, bt_node *x)
{
        bt_node *nParent = x->left;

        if (x == *root) {
                *root = nParent;
        }

        bt_move_down(x, nParent);
        x->left = nParent->right;

        if (nParent->right) {
                nParent->right->parent = x;
        }

        nParent->right = x;
}


static inline void bt_color_swap(bt_node *x1, bt_node *x2)
{
        bt_color temp;
        temp = x1->color;
        x1->color = x2->color;
        x2->color = temp;
}


static inline void bt_value_swap(bt_node *u, bt_node *v)
{
        int temp;
        temp = u->val;
        u->val = v->val;
        v->val = temp;
}


static void bt_fix_red_red(bt_node **root, bt_node *x)
{
        if (x == *root) {
                x->color = BLACK;
                return;
        }

        bt_node *parent = x->parent;
        bt_node *grandparent = parent->parent;
        bt_node *uncle = bt_uncle(x);

        if (parent->color != BLACK) {
                if (uncle && uncle->color == RED) {
                        // uncle red, perform recoloring and recurse
                        parent->color = BLACK;
                        uncle->color = BLACK;
                        grandparent->color = RED;
                        bt_fix_red_red(root, grandparent);
                } else {
                        // Else perform LR, LL, RL, RR
                        if (bt_is_left_child(parent)) {
                                if (bt_is_left_child(x)) {
                                        // for left right
                                        bt_color_swap(parent, grandparent);
                                } else {
                                        bt_rotate_left(root, parent);
                                        bt_color_swap(x, grandparent);
                                }
                                // for left left and left right
                                bt_rotate_right(root, grandparent);
                        } else {
                                if (bt_is_left_child(x)) {
                                        // for right left
                                        bt_rotate_right(root, parent);
                                        bt_color_swap(x, grandparent);
                                } else {
                                        bt_color_swap(parent, grandparent);
                                }

                                // for right right and right left
                                bt_rotate_left(root, grandparent);
                        }
                }
        }
}


/* find node that do not have a left child
 * in the subtree of the given node */
static bt_node *bt_successor(bt_node *x)
{
        bt_node *tmp = x;

        while (tmp->left) {
                tmp = tmp->left;
        }

        return tmp;
}


/* find node that replaces a deleted node in BST */
static bt_node *bt_replace(bt_node *x)
{
        // if node has 2 children
        if (x->left && x->right) {
                return bt_successor(x->right);
        }

        // if leaf
        if (!x->left && !x->right) {
                return NULL;
        }

        // if single child
        if (x->left != NULL) {
                return x->left;
        } else {
                return x->right;
        }
}


static void bt_fix_double_black(bt_node **root, bt_node *x)
{
        if (x == *root)
        {
                return;
        }

        bt_node *sibling = bt_sibling(x);
        bt_node *parent = x->parent;

        if (sibling == NULL) {
                // No sibiling, double black pushed up
                bt_fix_double_black(root, parent);
        } else {
                if (sibling->color == RED) {
                        // Sibling red
                        parent->color = RED;
                        sibling->color = BLACK;
                        if (bt_is_left_child(sibling)) {
                                bt_rotate_right(root, parent);
                        } else {
                                bt_rotate_left(root, parent);
                        }
                        bt_fix_double_black(root, x);
                } else {
                        if (bt_has_red_child(sibling)) {
                                // at least 1 red children
                                if (sibling->left && (sibling->left->color == RED)) {
                                        if (bt_is_left_child(sibling)) {
                                                // left left
                                                sibling->left->color = sibling->color;
                                                sibling->color = parent->color;
                                                bt_rotate_right(root, parent);
                                        } else {
                                                // right left
                                                sibling->left->color = parent->color;
                                                bt_rotate_right(root, sibling);
                                                bt_rotate_left(root, parent);
                                        }
                                } else {
                                        if (bt_is_left_child(sibling)) {
                                                // left right
                                                sibling->right->color = parent->color;
                                                bt_rotate_left(root, sibling);
                                                bt_rotate_right(root, parent);
                                        } else {
                                                // right right
                                                sibling->right->color = sibling->color;
                                                sibling->color = parent->color;
                                                bt_rotate_left(root, parent);
                                        }
                                }
                                parent->color = BLACK;
                        } else {
                                // 2 black children
                                sibling->color = RED;
                                if (parent->color == BLACK) {
                                        bt_fix_double_black(root, parent);
                                } else {
                                        parent->color = BLACK;
                                }
                        }
                }
        }

}


void bt_delete(bt_node **root, bt_node *v)
{
        bt_node *u = bt_replace(v);

        bool uvBlack = (u == NULL || u->color == BLACK) && (v->color == BLACK);
        bt_node *parent = v->parent;

        if (!u) {
                /* u is NULL therefore v is leaf */
                if (v == *root) {
                        // v is root, making root null
                        *root = NULL;
                } else {
                        if (uvBlack) {
                                /* u and v both black
                                 * v is leaf, fix double black at v */
                                bt_fix_double_black(root, v);
                        } else {
                                /* u or v is red */
                                if (bt_sibling(v)) {
                                        bt_sibling(v)->color = RED;
                                }
                        }

                        // delete v from the tree
                        if (bt_is_left_child(v))
                        {
                                parent->left = NULL;
                        } else {
                                parent->right = NULL;
                        }
                }
                free(v);
                return;
        }

        if (!v->left || !v->right)
        {
                /* v has one child */
                if (v == *root) {
                        // v is root, assign the value of u to v, and delete u
                        v->val = u->val;
                        v->left = v->right = NULL;
                        free(u);
                } else {
                        // Detach v from tree and move u up
                        if (bt_is_left_child(v)) {
                                parent->left = u;
                        } else {
                                parent->right = u;
                        }
                        free(v);
                        u->parent = parent;
                        if (uvBlack) {
                                /* u and v both black, fix double black at u */
                                bt_fix_double_black(root, u);
                        } else {
                                /* u or v red, color u black */
                                u->color = BLACK;
                        }
                }
                return;
        }

        /* v has 2 children, swap values with successor and recurse */
        bt_value_swap(u, v);
        bt_delete(root, u);
}


bt_node *search(bt_node **root, int n)
{
        bt_node *temp;

        temp = *root;
        while (temp != NULL) {
                if (n < temp->val) {
                        if (temp->left == NULL) {
                                break;
                        }
                        temp = temp->left;
                        continue;
                }

                if (n == temp->val || !temp->right) {
                        break;
                }
                temp = temp->right;

        }
        return temp;
}


void bt_insert(bt_node **root, int n)
{
        bt_node *newNode = bt_node_create(n);

        if (!*root) {
                newNode->color = BLACK;
                *root = newNode;
                return;
        }

        bt_node *temp = search(root, n);

        if (temp->val == n) {
                return;
        }

        newNode->parent = temp;

        if (n < temp->val) {
                temp->left = newNode;
        } else {
                temp->right = newNode;
        }

        bt_fix_red_red(root, newNode);
}


void bt_delete_by_val(bt_node **root, int n)
{
        if (!*root) {
                return;
        }

        bt_node *v = search(root, n);
        bt_node *u;

        if (v->val != n) {
                return;
        }

        bt_delete(root, v);
}

