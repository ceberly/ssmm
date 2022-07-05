#ifndef TREE_H
#define TREE_H

#include <stdint.h>

// no stdlib is available
#ifndef NULL
#define NULL 0
#endif

typedef int32_t wasm32_t;

/* most of this was taken from the Cormen et al. binary tree chapter ... */
typedef struct node {
  struct node *left;
  struct node *right;
  struct node *parent;
  wasm32_t val;
} tree_node;

wasm32_t tree_node_count(tree_node *root);
tree_node *tree_delete_nth(tree_node *root, wasm32_t);
tree_node *tree_insert(tree_node *root, tree_node *parent,
    wasm32_t, void *alloc(unsigned long));

#ifdef TEST
#include <stdio.h>
#include <stdbool.h>
bool test_tree(void);
void print_node(tree_node *t);
#endif

#endif
