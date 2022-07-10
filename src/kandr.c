#include "kandr.h"

#include "tree.h"

// This holds the "memory" from which the allocator gets its raw material.
unsigned char kandr_buffer[MEMORY_SIZE] = {0};
unsigned char *brk = &kandr_buffer[0];

// This is taken from K&R C, second edition, chapter 8 section 7
// "Example - A Storage Allocator"

// not too sure that this makes sense. We only use "pointers" in the 
// tree struct, which is 32 bits in wasm, so i guess it's ok alignment-wise.
typedef wasm32_t Align;

union header {
  struct {
    union header *ptr;
    wasm32_t size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

#define NALLOC 1024
static Header *morecore(wasm32_t nu) {
  if (nu < NALLOC) nu = NALLOC;
  // simulate the sbrk() system call...
  // XXX: check for overflow
  unsigned char *cp = brk;
  brk += (nu * sizeof(Header));

  Header *up = (Header *) cp;
  up->s.size = nu;
  kandr_free((void *)(up+1));

  return freep;
}

// "malloc" in the example code.
void *kandr_alloc(wasm32_t nbytes) {
  wasm32_t nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  Header *prevp;

  if ((prevp = freep) == NULL) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }

  // this style deserves a "(sic)" :) 
  for (Header *p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits) {
        prevp->s.ptr = p->s.ptr;
      }
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }

      freep = prevp;
      return (void *)(p+1);
    }

    if (p == freep) {
      if ((p = morecore(nunits)) == NULL)
        return NULL;
    }

  }

  return NULL;
}

// "free" in the example code.
void kandr_free(void *ap) {
  Header *p;
  Header *bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr)) break;

  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else {
    bp->s.ptr = p->s.ptr;
  }

  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else {
    p->s.ptr = bp;
  }

  freep = p;
}

// XXX: this does the same thing in all allocators,
// DRY it out to the main js function, or another wasm function...
tree_node *kandr_tick(wasm32_t chance) {
  static tree_node *root = NULL;
  wasm32_t count = tree_node_count(root);

  if (count > 0 && (chance % 3 == 0)) {
    root = tree_delete_nth(root, chance % count, kandr_free);
  } else {
    if (count > 100000) {
      root = tree_delete_nth(root, chance % count, kandr_free);
    } else {
      root = tree_insert(root, NULL, chance, kandr_alloc);
    }
  }

  return root;
}


#ifdef TEST
bool test_kandr_properties(void) {
  void *p10 = kandr_alloc(10);
  void *p100 = kandr_alloc(100);
  void *p1000 = kandr_alloc(1000);
  void *p10000 = kandr_alloc(10000);

  if (p10 == NULL) return false;
  if (p100 == NULL) return false;
  if (p1000 == NULL) return false;
  if (p10000 == NULL) return false;

  kandr_free(p10);
  kandr_free(p100);
  kandr_free(p1000);
  kandr_free(p10000);

  return true;
}

bool test_kandr(void) {
  if (!test_kandr_properties()) return false;

  return true;
}

#endif
