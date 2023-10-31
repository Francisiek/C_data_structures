#include "../self_organising_bst.h"
#include <iso646.h>

//FUCK IT DOESN'T WORKS BECAUSE OF PREPROCESSING RULES

#define _bst_declare_rotate_node(_side, _other_side) \
void bst_rotate_node_##_side## (bst_t tree, bst_node_ptr lower_node) { \
    if (tree == NULL or lower_node == NULL or \
        (bst_##_side## == bst_##_other_side## ) or (bst_##_side##==bst_none) \
            or (bst_##_other_side##==bst_none)) \
        return; \
    \
    side_t node_side = bst_node_side(lower_node); \
    if (node_side != bst_##_other_side##) \
        return; \
    \
        bst_node_ptr old_##_side##_child = lower_node->##_side##_child; \
        bst_node_ptr old_parent = lower_node->parent; \
        bst_node_ptr old_grandma = (old_parent != NULL) ? old_parent->parent : NULL; \
        side_t old_parent_side = bst_node_side(old_parent); \
    \
        if (old_parent != NULL) { \
            lower_node->##_side##_child = old_parent; \
            old_parent->parent = lower_node; \
    \
            old_parent->##_other_side##_child = old_##_side##_child; \
            if (old_##_side##_child != NULL) \
                old_##_side##_child->parent = old_parent; \
    \
            lower_node->parent = old_grandma; \
            if (old_grandma != NULL) { \
                if (old_parent_side == bst_left) \
                    old_grandma->left_child = lower_node; \
                else if (old_parent_side == bst_right) \
                    old_grandma->right_child = lower_node; \
            } else if (old_parent == tree->root) \
                tree->root = lower_node; \
        } \
} \

_bst_declare_rotate_node(left, right)
_bst_declare_rotate_node(right, left)

