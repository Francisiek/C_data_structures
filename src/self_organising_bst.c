#include <stdlib.h>
#include <string.h>
#include <iso646.h>
#include <stdbool.h>
#include "../self_organising_bst.h"

const size_t bst_size = sizeof(struct _BST_Tree);
const size_t bst_node_size = sizeof(struct _BST_Node);

/* 
compare_function:
The first argument is the data that is going to be compared against
the second argument which is the data inside a node.
The result is sign of the difference of the first and the second argument
(data - data_in_node). In other words:
if first_argument < second_argument then result = -1 (LEFT)
if first_argument > second_argument then result = 1 (RIGHT)
if first_argument = second_argument then result = 0 (NONE)
This behaviour is needed to properly organise this binary tree model.
In this tree mode smaller child nodes goes on the left and bigger ones goes on the right.
The same value is not inserted multiple times.
*/
void bst_init(bst_t tree, size_t data_bytes, int (*compare_side_function)(void *data, void *node_data)) {
    memcpy(tree, &(bst_t){{NULL, 0, data_bytes, compare_side_function}}, bst_size);
}

bst_node_ptr bst_search_nearest_node(bst_t tree, void *data) {
    if (tree == NULL or data == NULL)
        return NULL;

    bst_compare_function compare = tree->compare_function;
    bst_node_ptr current_node = tree->root;
    bst_node_ptr previous_node = NULL;
    
    while (current_node != NULL) {
        side_t search_side = compare(data, current_node->data);
        previous_node = current_node;

        if (search_side == RIGHT)
            current_node = current_node->right_child;
        else if (search_side == NONE)
            return current_node;
        else
            current_node = current_node->left_child;
    }

    return previous_node;
}

bst_node_ptr allocate_tree_node(bst_node_ptr parent, void *data) {
    bst_node_ptr new_node = malloc(bst_node_size);
    if (new_node == NULL)
        return NULL;
    
    new_node->data = data;
    new_node->left_child = new_node->right_child = NULL;
    new_node->parent = parent;

    return new_node;
}

bst_node_ptr bst_insert(bst_t tree, void *data) {
    if (tree == NULL or data == NULL)
        return NULL;
    
    bst_node_ptr new_node = NULL;
    bst_node_ptr nearest_node = bst_search_nearest_node(tree, data);
    if (nearest_node == NULL) {
        new_node = allocate_tree_node(NULL, data);
        return (tree->root = new_node);
    }
    
    side_t insertion_side = tree->compare_function(data, nearest_node->data);
    if (insertion_side == NONE)
        return nearest_node;
    else if (insertion_side == LEFT)
        new_node = nearest_node->left_child = allocate_tree_node(nearest_node, data);
    else
        new_node = nearest_node->right_child = allocate_tree_node(nearest_node, data);

    return new_node;
}

side_t bst_node_side(bst_node_ptr node) {
    if (node == NULL or node->parent == NULL)
        return NONE;

    if (node->parent->left_child == node)
        return LEFT;
    else if (node->parent->right_child == node)
        return RIGHT;
    else
        return NONE;
}

static void left_rotate(bst_node_ptr lower_node) {
    if (lower_node == NULL)
        return;
    
    bst_node_ptr old_left_child = lower_node->left_child;
    bst_node_ptr old_parent = lower_node->parent;
    bst_node_ptr old_grandma = (old_parent != NULL) ? old_parent->parent : NULL;

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

static void right_rotate(bst_node_ptr lower_node) {
    if (lower_node == NULL)
        return;
    
    bst_node_ptr old_right_child = lower_node->right_child;
    bst_node_ptr old_parent = lower_node->parent;
    bst_node_ptr old_grandma = (old_parent != NULL) ? old_parent->parent : NULL;

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

void bst_splay(bst_t tree, void *data) {
    if (tree == NULL or data == NULL)
        return;

    bst_node_ptr current_node = bst_search_nearest_node(tree, data);
    while (current_node != tree->root) {
        side_t current_node_side = bst_node_side(current_node);
        if (current_node_side == LEFT)
            left_rotate(current_node);
        else
            right_rotate(current_node);
    }
}

/* DEBUG FUNCTIONS */

