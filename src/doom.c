#include "ssmm.h"

#include "doom.h"
#include "tree.h"


static void *doom_alloc(wasm32_t size) {
  return Z_Malloc(size, PU_STATIC, NULL);
}

//static void doom_free(void *p) {
//  Z_Free(p);
//}


void I_Error(char *error, ...) {
  console_log(-1);
  return;
}

// XXX: is unsigned char * correct here?
void doom_init(unsigned char *memory, wasm32_t mem_size)
{
  // The pointer from wasm comes in as the index 0, which is treated as NULL,
  // which causes everything to stop immediately. So start at 1 :)
  Z_Init(memory + 1, mem_size - 1);
}

void doom_tick(wasm32_t chance, wasm32_t val) {
  static tree_node *root = NULL;

  root = tree_insert(root, NULL, val, doom_alloc);
  root = tree_insert(root, NULL, val + 1, doom_alloc);
  root = tree_insert(root, NULL, val + 2, doom_alloc);
  root = tree_insert(root, NULL, val + 3, doom_alloc);
  root = tree_insert(root, NULL, val + 4, doom_alloc);
  root = tree_insert(root, NULL, val + 5, doom_alloc);
  root = tree_insert(root, NULL, val + 6, doom_alloc);
  console_log((wasm32_t)root);
  console_log(tree_node_count(root));
}

#ifdef TEST
bool test_doom(void) {
  return false;
}
#endif
