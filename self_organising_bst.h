#include <stdlib.h>

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

typedef int (*bst_compare_function)(void*, void*);
typedef struct _BST_Tree bst_t[1];
typedef struct _BST_Node bst_node;
typedef bst_node* bst_node_ptr;

extern const size_t bst_size;
extern const size_t bst_node_size;

typedef enum { LEFT = -1, RIGHT = 1, NONE = 0} side_t;

void bst_init(bst_t tree, size_t data_bytes, 
    int (*compare_side_function)(void *data, void *node_data));

bst_node_ptr bst_search_nearest_node(bst_t tree, void *data);

bst_node_ptr allocate_tree_node(bst_t tree, bst_node_ptr parent, void *data);

bst_node_ptr bst_insert(bst_t tree, void *data);

side_t bst_node_side(bst_node_ptr node);

void bst_splay(bst_t tree, void *data);

#endif