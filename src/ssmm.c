#include "ext/doom/z_zone.h"

#include "tree.h"

#include <stdint.h>

// CDE: null is not defined without a stdlib
#define NULL 0

typedef int32_t i32;
typedef int64_t i64;

extern uint8_t *memory;

// DOOM error handler
void I_Error(const char *fmt, ...) {
  // XXX: make varargs work...
  oops(fmt, 100);
}

static tree_node *doom_root = NULL;

void *doom_alloc(int size) {
  return Z_Malloc(size, PU_STATIC, NULL);
}

void doom_free(void *p) {
  Z_Free(p);
}

__attribute__((export_name("memtest"))) void memtest() {
  memory[0] = 1;
}

__attribute__((export_name("init_doom"))) void init_doom(int mem_size) { 
  Z_Init(memory, mem_size);
}

__attribute__((export_name("tick_doom"))) void tick_doom(i32 chance, i32 val) {
  // if chance is even, insert val. otherwise delete if it exists...
  if (chance % 2 == 0) {
    insert(&doom_root, val, doom_alloc);
  } else {
  }
}

__attribute__((export_name("tree_height"))) i32 tree_height(void) {
  return tree_height(doom_root);
}
