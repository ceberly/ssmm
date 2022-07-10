#include "tree.h"

static tree_node *tree_min(tree_node *t) {
  if (t == NULL) return NULL;

  while (t->left != NULL) {
    t = t->left;
  }

  return t;
}

static tree_node *tree_succ(tree_node *t) {
  if (t == NULL) return NULL;

  if (t->right != NULL) return tree_min(t->right);

  tree_node *y = t->parent;
  while(y != NULL && (t == y->right)) {
    t = y;
    y = y->parent;
  }

  return y;
}

static tree_node *transplant(tree_node *t, tree_node *u, tree_node *v) {
  if (t == NULL || u == NULL) return NULL;

  if (u->parent == NULL) {
    t = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }

  if (v != NULL) v->parent = u->parent;

  return t;
}

wasm32_t tree_node_count(tree_node *t) {
  if (t == NULL) {
    return 0;
  }

  return 1 + tree_node_count(t->left) + tree_node_count(t->right);
}

// deletes the nth node from the tree, 0-indexed.
// Nodes are numbered in sorted order, so the very bottom left is 0.
tree_node *tree_delete_nth(tree_node *t, wasm32_t n, void dealloc(void *)) {
  if (t == NULL) return NULL;

  wasm32_t count = tree_node_count(t);
  if (count < (n + 1)) return t;

  tree_node *found = tree_min(t);

  wasm32_t index = 0;
  while (index < n) {
    found = tree_succ(found);
    index++;
  }

  if (found->left == NULL) {
    t = transplant(t, found, found->right);
    dealloc(found);
    return t;
  }

  if (found->right == NULL) {
    t = transplant(t, found, found->left);
    dealloc(found);
    return t;
  }

  tree_node *y= tree_min(found->right);
  if (y->parent != found) {
    t = transplant(t, y, y->right);
    y->right = found->right;
    y->right->parent = y;
  }

  t = transplant(t, found, y);
  y->left = found->left;
  y->left->parent = y;

  dealloc(found);
  return t;
}

tree_node *tree_insert(
    tree_node *t,
    tree_node *parent,
    wasm32_t v,
    void *alloc(wasm32_t))
{
  if (t == NULL) {
    t = alloc(1 * sizeof(tree_node)); 
    t->parent = parent;
    t->val = v;
    t->left = NULL;
    t->right = NULL;
  } else if (v < t->val){
    t->left = tree_insert(t->left, t, v, alloc);
  }
  else {
    t->right = tree_insert(t->right, t, v, alloc);
  }

  return t;
}

#ifdef TEST
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  wasm32_t *v;
  size_t len;
} wasm32_array;

void *malloc32(wasm32_t size) {
  return malloc((size_t) size);
}

void print_node(tree_node *t) {
  printf("node (%p):", (void *)t);
  if (t == NULL) {
    printf(" (NULL)\n");
    return;
  }

  printf("\n");
  printf("\tval: %d\n", t->val);

  // prints "nil" if pointer is null in clang, YMMV.
  printf("\tparent: %p\n", (void *)t->parent);
  printf("\tleft: %p\n", (void *)t->left);
  printf("\tright: %p\n", (void *)t->right);
}

void print_tree(tree_node *t) {
  if (t != NULL) {
    print_tree(t->left);
    printf("node %d\n", t->val);
    print_tree(t->right);
  }
}

tree_node *tree_destroy(tree_node *t, void dealloc(void *)) {
  if (t != NULL) {
    tree_destroy(t->left, dealloc);
    tree_destroy(t->right, dealloc);
    dealloc(t);
  }

  return NULL;
}

void tree_sort(tree_node *t, wasm32_array *dest) {
  if (t != NULL) {
    tree_sort(t->left, dest);
    dest->v[dest->len++] = t->val;
    tree_sort(t->right, dest);
  }
}

bool test_tree_delete_nth(void) {
  tree_node *root = NULL;

  root = tree_delete_nth(root, 0, free);
  if (root != NULL) {
    printf("expected root to be NULL\n");
    goto fail;
  }

  root = tree_insert(root, NULL, 10, malloc32);
  root = tree_delete_nth(root, 1, free);
  if (root == NULL) {
    printf("expected root not to be NULL\n");
    goto fail;
  }

  root = tree_delete_nth(root, 0, free);
  if (root != NULL) {
    printf("expected root to be NULL\n");
    goto fail;
  }

  root = tree_destroy(root, free);

  // found->left == NULL
  root = tree_insert(root, NULL, 4, malloc32);
  root = tree_insert(root, NULL, 20, malloc32);

  root = tree_delete_nth(root, 0, free);
  if (tree_node_count(root) != 1) {
    printf("expected tree node count == 1\n");
    goto fail;
  }

  if (root->val != 20) {
    printf("expected a different node\n");
    goto fail;
  }

  root = tree_destroy(root, free);

  // found->right == NULL
  root = tree_insert(root, NULL, 40, malloc32);
  root = tree_insert(root, NULL, 2, malloc32);

  root = tree_delete_nth(root, 1, free);
  if (tree_node_count(root) != 1) {
    printf("expected tree node count == 1\n");
    goto fail;
  }

  if (root->val != 2) {
    printf("expected a different node\n");
    goto fail;
  }

  root = tree_destroy(root, free);

  // found has 2 children
  root = tree_insert(root, NULL, 10, malloc32);
  root = tree_insert(root, NULL, 2, malloc32);
  root = tree_insert(root, NULL, 20, malloc32);

  root = tree_delete_nth(root, 1, free);
  if (tree_node_count(root) != 2) {
    printf("expected tree node count == 2\n");
    goto fail;
  }
  
  if (root->val != 20) {
    printf("expected a different node value\n");
    goto fail;
  }

  if (root->left->val != 2) {
    printf("expected a different node value\n");
    goto fail;
  }

  return true;

fail:
  tree_destroy(root, free);
  return false;
}

bool test_tree_transplant(void) {
  tree_node *root = NULL;
  tree_node *v = NULL;

  // u->parent == NULL and v == NULL
  root = tree_insert(root, NULL, 10, malloc32);
  root = transplant(root, root, v);
  if (root != NULL || root != v) {
    printf("expected v to be transplanted to root\n");
    goto fail;
  }

  // v is aliased into root, so don't double free :)
  root = tree_destroy(root, free);
  v = NULL;

  // u->parent == NULL and v != NULL
  root = tree_insert(root, NULL, 10, malloc32);
  v = tree_insert(v, NULL, 100, malloc32);
  root = transplant(root, root, v);
  if (root == NULL || root != v) {
    printf("expected v to be transplanted to root\n");
    goto fail;
  }

  root = tree_destroy(root, free);
  v = NULL;

  // u == u->parent->left
  root = tree_insert(root, NULL, 100, malloc32);
  root = tree_insert(root, NULL, 10, malloc32);

  v = tree_insert(v, NULL, 1000, malloc32);

  root = transplant(root, root->left, v);

  if (root->left != v) {
    printf("expected v to be transplanted at root->left\n");
    goto fail;
  }

  root = tree_destroy(root, free);
  v = NULL;

  // u == u->parent->right
  root = tree_insert(root, NULL, 101, malloc32);
  root = tree_insert(root, NULL, 201, malloc32);

  v = tree_insert(v, NULL, 1001, malloc32);

  root = transplant(root, root->right, v); 

  if (root->right != v) {
    printf("expected v to be transplanted at root->right\n");
    goto fail;
  }

  tree_destroy(root, free);
  return true;

fail:
  tree_destroy(root, free);
  return false;
}

bool test_tree_delete_regressions(void) {
  // fix bugs from property testing
  tree_node *root = NULL;

  root = tree_insert(root, NULL, 5, malloc32); // 2
  root = tree_insert(root, NULL, 7, malloc32); // 4
  root = tree_insert(root, NULL, 4, malloc32); // 6
  root = tree_insert(root, NULL, 6, malloc32); // 8

  root = tree_delete_nth(root, 1, free);

  int precount = tree_node_count(root);
  root = tree_delete_nth(root, precount - 1, free);
  int postcount = tree_node_count(root);

  if (precount == postcount) {
    printf("expected tree node count to decrease.\n");
    return false;
  }

  return true;
}

struct counter {
  size_t n;
};

size_t alloc_count = 0;
size_t dealloc_count = 0;

void *counting_alloc(wasm32_t s) {
  alloc_count += 1;
  return malloc32(s);
}

void counting_dealloc(void *p) {
  dealloc_count += 1;
  free(p);
}

bool test_tree_properties(void) {
  size_t n_iter = 10000;
  int32_t *sorted_v = calloc(n_iter, sizeof(int32_t));
  wasm32_array sorted = { .v = sorted_v, .len = 0 };

  tree_node *root = NULL;
  time_t seed = time(NULL);
  srand(seed);

  int delete_count = 0;
  int insert_count = 0;
  for (size_t i = 0; i < n_iter; i++) {
    wasm32_t chance = rand();
    wasm32_t count = tree_node_count(root);

    if (count == 0 || chance % 2 == 0) {
      root = tree_insert(root, NULL, chance, counting_alloc);
      insert_count++;
    } else {
      root = tree_delete_nth(root, chance % count, counting_dealloc);
      delete_count++;
    }

    // re-use the sorted vector
    sorted.len = 0;
    tree_sort(root, &sorted);

    if (sorted.len != (insert_count - delete_count)) {
      printf("expected sorted size to be %d, got %zu instead.\n",
          insert_count - delete_count, sorted.len);
      goto fail;
    }

    for (size_t j = 0; j < sorted.len; j++) {
      if (j > 0) {
        int32_t a = sorted.v[j-1];
        int32_t b = sorted.v[j];
        if (a > b) {
          printf("expected %d to be less than %d at index %zu\n", a, b, j);
          goto fail;
        }
      }
    }
  }

  tree_destroy(root, counting_dealloc);
  if (alloc_count != dealloc_count) {
    printf("expected alloc count to equal dealloc count\n");
    goto fail;
  }

  free(sorted.v);

  return true;

fail:
    free(sorted.v);
    return false;
}

bool test_tree(void) {
  if (!test_tree_transplant()) return false;
  if (!test_tree_delete_nth()) return false;
  if (!test_tree_delete_regressions()) return false;
  if (!test_tree_properties()) return false;

  return true;
}

#endif
