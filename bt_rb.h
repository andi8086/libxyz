#ifndef __BT_RB_H__
#define __BT_RB_H__

/* Header for red-black self-balancing binary trees */

typedef enum {
        RED,
        BLACK
} bt_color;


typedef struct _bt_node {
        int val;
        bt_color color;
        struct _bt_node *left;
        struct _bt_node *right;
        struct _bt_node *parent;
} bt_node;


void bt_delete(bt_node **root, bt_node *v);
bt_node *search(bt_node **root, int n);
void bt_insert(bt_node **root, int n);
void bt_delete_by_val(bt_node **root, int n);


#endif
