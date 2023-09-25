#include <stdlib.h>
#include <string.h>
#include <iso646.h>

struct _BST_Node {
    struct _BST_Node *parent;
    struct _BST_Node *left_child, *right_child;
    void *data_container;
};

struct _BST {
    struct _BST_Node *root;
    size_t number_of_nodes;
    size_t data_container_bytes;
    int (*compare_function)(void*, void*);
};

typedef int (*bst_compare_function)(void*, void*);
typedef struct _BST bst_t[1];
typedef struct _BST_Node bst_node;
const size_t bst_size = sizeof(struct _BST);
const size_t bst_node_size = sizeof(struct _BST_Node);

typedef enum { LEFT = 0, RIGHT = 1} side_t;

void bst_init(bst_t tree, size_t data_bytes, int (*compare_function)(void*, void*)) {
    memcpy(tree, (bst_t){NULL, 0, data_bytes, compare_function}, bst_size);
}

bst_node* bst_search_nearest_node(bst_t tree, void *value) {
    if (tree == NULL or value == NULL)
        return;
    
    bst_compare_function compare = tree->compare_function;
    bst_node *current_node = tree->root;
    bst_node *previous_node = NULL;
    while (current_node != NULL) {
        int result = compare(current_node->data_container, value);
        previous_node = current_node;

        if (result < 0)
            current_node = current_node->right_child;
        else if (result == 0)
            return current_node;
        else
            current_node = current_node->left_child;
    }

    return previous_node;
}


side_t bst_node_side(bst_node* node);

void bst_splay(bst_t tree, void *value) {
    if (tree == NULL or value == NULL)
        return;

    bst_node *current_node = bst_search_nearest_node(tree, value);
    while (current_node != tree->root) {
        side_t current_node_side = bts_node_side(current_node);
        if (current_node_side == LEFT)
            left_rotate(current_node);
        else
            right_rotate(current_node);
    }
}