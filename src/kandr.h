#ifndef KANDR_H
#define KANDR_H

#include "ssmm.h"

void *kandr_alloc(wasm32_t size);
void kandr_free(void *);

#ifdef TEST
#include <stdio.h>
#include <stdbool.h>
bool test_kandr(void);
#endif

#endif
