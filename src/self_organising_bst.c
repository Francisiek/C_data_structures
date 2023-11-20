#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iso646.h>
#include <stdbool.h>
#include "../hdr/self_organising_bst.h"

const size_t bst_size = sizeof(struct _BST_Tree);
const size_t bst_node_size = sizeof(struct _BST_Node);

#define DEBUG

/* 
compare_function:
The first argument is the data that is going to be compared against
the second argument which is the data inside a node.
The result is sign of the difference of the first and the second argument
(data - data_in_node). In other words:
if first_argument < second_argument then result = -1 (bst_left)
if first_argument > second_argument then result = 1 (bst_right)
if first_argument = second_argument then result = 0 (bst_none)
This behaviour is needed to properly organise this binary tree model.
In this tree mode smaller child nodes goes on the bst_left and bigger ones goes on the bst_right.
The same value is not intended to be inserted multiple times.
*/
void bst_init(bst_t tree, size_t data_bytes, bst_compare_function cmp) {
    memcpy(tree, &(bst_t){{NULL, 0, data_bytes, cmp}}, bst_size);
}

void inline set_node_to_side_child(bst_node_ptr* node, side_t side) {
    if (side == bst_left)
        *node = (*node)->left_child;
    else if (side == bst_right)
        *node = (*node)->right_child;
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

        if (search_side == bst_right)
            current_node = current_node->right_child;
        else if (search_side == bst_none)
            return current_node;
        else
            current_node = current_node->left_child;
    }

    return previous_node;
}

bst_node_ptr allocate_tree_node(bst_t tree, bst_node_ptr parent, void *data) {
    bst_node_ptr new_node = malloc(bst_node_size);
    if (tree == NULL or new_node == NULL)
        return NULL;
    
    new_node->data = malloc(tree->data_container_bytes);
    if (new_node->data == NULL)
        return NULL;
    memcpy(new_node->data, data, tree->data_container_bytes);
    new_node->left_child = new_node->right_child = NULL;
    new_node->parent = parent;

    return new_node;
}

bst_node_ptr bst_insert_root(bst_t tree, void* new_root_data) {
    bst_node_ptr new_root = allocate_tree_node(tree, NULL, new_root_data);
    bst_node_ptr root = tree->root;
    side_t insertion_side = tree->compare_function(root->data, new_root_data);

    if (insertion_side == bst_left) {
        new_root->left_child = root;
        new_root->right_child = root->right_child;
        if (root->right_child != NULL)
            root->right_child->parent = new_root;
        root->right_child = NULL;
    } else if (insertion_side == bst_right) {
        new_root->right_child = root;
        new_root->left_child = root->left_child;
        if (root->left_child != NULL)
            root->left_child->parent = new_root;
        root->left_child = NULL;
    }
    root->parent = new_root;
    tree->root = new_root;

    return new_root;
}

bst_node_ptr bst_insert(bst_t tree, void *data) {
    if (tree == NULL or data == NULL)
        return NULL;
    
    bst_node_ptr new_node = NULL;
    bst_node_ptr nearest_node = bst_search_nearest_node(tree, data);
    if (nearest_node == NULL) {
        new_node = allocate_tree_node(tree, NULL, data);
        return (tree->root = new_node);
    }
    
    bst_splay(tree, nearest_node);
    side_t compare_result = tree->compare_function(tree->root->data, data);
    if (compare_result == bst_equal)
        return tree->root;
    else
        new_node = bst_insert_root(tree, data);

    return new_node;
}

side_t bst_node_side(bst_node_ptr node) {
    if (node == NULL or node->parent == NULL)
        return bst_none;

    if (node->parent->left_child == node)
        return bst_left;
    else if (node->parent->right_child == node)
        return bst_right;
    else
        return bst_none;
}

void bst_rotate_node_universal(bst_t tree, bst_node_ptr lower_node, side_t rotation_side) {
    bst_node_ptr old_right_child = lower_node->right_child;
    bst_node_ptr old_left_child = lower_node->left_child;
    bst_node_ptr old_parent = lower_node->parent;
    bst_node_ptr old_grandma = (old_parent != NULL) ? old_parent->parent : NULL;
    side_t old_parent_side = bst_node_side(old_parent);

    if (old_parent == NULL)
        return;

    old_parent->parent = lower_node;

    if (rotation_side == bst_left) {
        lower_node->left_child = old_parent;

        old_parent->right_child = old_left_child;
        if (old_left_child != NULL)
            old_left_child->parent = old_parent;
    } else if (rotation_side == bst_right) {
        lower_node->right_child = old_parent;
        
        old_parent->left_child = old_right_child;
        if (old_right_child != NULL)
            old_right_child->parent = old_parent;
    }

    lower_node->parent = old_grandma;
    if (old_grandma != NULL) {
        if (old_parent_side == bst_left)
            old_grandma->left_child = lower_node;
        else if (old_parent_side == bst_right)
            old_grandma->right_child = lower_node;
    } else if (old_parent == tree->root)
        tree->root = lower_node;
}

void bst_rotate_node_left(bst_t tree, bst_node_ptr lower_node) {
    if (tree == NULL or lower_node == NULL or bst_node_side(lower_node) != bst_right)
        return;
    bst_rotate_node_universal(tree, lower_node, bst_left);
}

void bst_rotate_node_right(bst_t tree, bst_node_ptr lower_node) {
    if (tree == NULL or lower_node == NULL or bst_node_side(lower_node) != bst_left)
        return;
    bst_rotate_node_universal(tree, lower_node, bst_right);
}

void bst_splay(bst_t tree, bst_node_ptr start_node) {
    if (tree == NULL)
        return;

    bst_node_ptr current_node = start_node;
    while (current_node != tree->root) {
        side_t current_node_side = bst_node_side(current_node);
        if (current_node_side == bst_right)
            bst_rotate_node_left(tree, current_node);
        else
            bst_rotate_node_right(tree, current_node);
        
        #ifdef DEBUG
            printf("step\n");
            bst_print_horizontal(tree);
            printf("is organised %d\n", bst_is_organised(tree));
        #endif
    }
}

/* TREE PRINTING FUNCTIONS */

int max_int(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
}

int _bst_height_rec(bst_node_ptr node) {
    if (node == NULL)
        return 0;

    return max_int(_bst_height_rec(node->left_child), _bst_height_rec(node->right_child)) + 1;
}


size_t bst_height(bst_t tree) {
    if (tree == NULL) {
        return 0;
    }

    return _bst_height_rec(tree->root);
}

void print_spaces_and_node(bst_node_ptr node, size_t spaces_counter) {
    for (size_t count = 0; count < spaces_counter; count++)
            printf(" ");
        printf("%d\n", (node->data) == NULL ? 0 : *(int*)node->data);
}

void _bst_print_horizontal_recursion(bst_node_ptr node, size_t spaces_counter) {
    static const int spaces = 5;

    if (node == NULL) {
        return;
    }

    _bst_print_horizontal_recursion(node->right_child, spaces_counter + spaces);
    print_spaces_and_node(node, spaces_counter);
    _bst_print_horizontal_recursion(node->left_child, spaces_counter + spaces);
}


int bst_print_horizontal(bst_t tree) {
    if (tree == NULL)
        return 1;
    
    _bst_print_horizontal_recursion(tree->root, 0);
    return 0;
}

/* DEBUG FUNCTIONS */

static bst_compare_function current_cmp_function;

static int bst_organisation_errors_util(bst_node_ptr node) {
    if (node == NULL)
        return 0;

    int sum = 0;
    
    if (node->left_child != NULL) {
        side_t insertion_side = current_cmp_function(node->left_child->data, node->data);
        if (insertion_side != bst_left)
            sum++;
    }

    if (node->right_child != NULL) {
        side_t insertion_side = current_cmp_function(node->right_child->data, node->data);
        if (insertion_side != bst_right)
            sum++;
    }

    return sum + bst_organisation_errors_util(node->left_child) + 
        bst_organisation_errors_util(node->right_child);
}

int bst_organisation_errors(bst_t tree) {
    if (tree == NULL)
        return false;

    current_cmp_function = tree->compare_function;
    int organisation_errors = bst_organisation_errors_util(tree->root);
    current_cmp_function = NULL;

    return organisation_errors;
}

bool bst_is_organised(bst_t tree) {
    return bst_organisation_errors(tree) == 0;
}
