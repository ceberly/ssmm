#include "ssmm.h"

#include "doom.h"
#include "tree.h"

extern unsigned char *doom_buffer;

static void *doom_alloc(wasm32_t size) {
  return Z_Malloc(size, PU_STATIC, NULL);
}

static void doom_free(void *p) {
  Z_Free(p);
}


void I_Error(char *error, ...) {
  console_log(-1);
  return;
}

// XXX: is unsigned char * correct here?
void doom_init(unsigned char *memory, wasm32_t mem_size)
{
  // The pointer from wasm comes in as the index 0, which is treated as NULL,
  // which causes everything to stop immediately. So start at 1 :)
  Z_Init(memory, mem_size);
}

tree_node *doom_tick(wasm32_t chance) {
  static tree_node *root = NULL;
  wasm32_t count = tree_node_count(root);

  if (count > 0 && (chance % 3 == 0)) {
    root = tree_delete_nth(root, chance % count, doom_free);
  } else {
    if (count > 100000) {
      root = tree_delete_nth(root, chance % count, doom_free);
    } else {
      root = tree_insert(root, NULL, chance, doom_alloc);
    }
  }

  return root;
}

#ifdef TEST
bool test_doom(void) {
  return false;
}
#endif
