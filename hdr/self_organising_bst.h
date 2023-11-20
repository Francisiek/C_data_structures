#include <stdlib.h>
#include <stdbool.h>

#ifndef _BST
#define _BST

struct _BST_Node {
    struct _BST_Node *parent;
    struct _BST_Node *left_child, *right_child;
    void *data;
};

struct _BST_Tree {
    struct _BST_Node *root;
    size_t number_of_nodes;
    size_t data_container_bytes;
    int (*compare_function)(void*, void*);
};

typedef int (*bst_compare_function)(void *data, void *node_data);
typedef struct _BST_Tree bst_t[1];
typedef struct _BST_Node bst_node;
typedef bst_node* bst_node_ptr;

extern const size_t bst_size;
extern const size_t bst_node_size;

typedef enum { bst_left = -1, bst_right = 1, bst_none = 0} side_t;
enum { bst_less = -1, bst_greater = 1, bst_equal = 0};

void bst_init(bst_t tree, size_t data_bytes, bst_compare_function cmp);

bst_node_ptr bst_search_nearest_node(bst_t tree, void *data);

bst_node_ptr allocate_tree_node(bst_t tree, bst_node_ptr parent, void *data);

bst_node_ptr bst_insert(bst_t tree, void *data);

side_t bst_node_side(bst_node_ptr node);

void bst_rotate_node_left(bst_t tree, bst_node_ptr lower_node);

void bst_rotate_node_right(bst_t tree, bst_node_ptr lower_node);

void bst_splay(bst_t tree, bst_node_ptr start_node);

size_t bst_height(bst_t tree);

int bst_print_horizontal(bst_t tree);

int bst_organisation_errors(bst_t tree);

bool bst_is_organised(bst_t tree);

#endif