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

typedef enum { LEFT = 0, RIGHT, NONE} side_t;

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


side_t bst_node_side(bst_node *node) {
    if (node == NULL or node->parent == NULL)
        return NONE;

    if (node->parent->left_child == node)
        return LEFT;
    else
        return RIGHT;
}

void left_rotate(bst_node *lower_node) {
    if (lower_node == NULL)
        return;
    
    bst_node *old_left_child = lower_node->left_child;
    bst_node *old_parent = lower_node->parent;
    bst_node *old_grandma = (old_parent != NULL) ? old_parent->parent : NULL;

    if (old_parent != NULL) {
        lower_node->left_child = old_parent;
        old_parent->parent = lower_node;
        
        old_parent->right_child = old_left_child;
        if (old_left_child != NULL)
            old_left_child->parent = old_parent;
        
        lower_node->parent = old_grandma;
        if (old_grandma != NULL) {
            if (bst_node_side(old_parent) == LEFT)
                old_grandma->left_child = lower_node;
            else
                old_grandma->right_child = lower_node;
        }
    }
}

void right_rotate(bst_node *lower_node) {
    if (lower_node == NULL)
        return;
    
    bst_node *old_right_child = lower_node->right_child;
    bst_node *old_parent = lower_node->parent;
    bst_node *old_grandma = (old_parent != NULL) ? old_parent->parent : NULL;

    if (old_parent != NULL) {
        lower_node->right_child = old_parent;
        old_parent->parent = lower_node;
        
        old_parent->left_child = old_right_child;
        if (old_right_child != NULL)
            old_right_child->parent = old_parent;
        
        lower_node->parent = old_grandma;
        if (old_grandma != NULL) {
            if (bst_node_side(old_parent) == LEFT)
                old_grandma->left_child = lower_node;
            else
                old_grandma->right_child = lower_node;
        }
    }
}

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