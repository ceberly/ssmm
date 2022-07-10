#ifndef SSMM_H
#define SSMM_H

#include <stdint.h>

// no stdlib is available in wasm
#ifndef NULL
#define NULL 0
#endif

#define MEMORY_SIZE 1024 * 1024 // 1 MB

typedef int32_t wasm32_t;

//XXX: does this need to be extern ?
// how does this work with --allow-undefined
void console_log(wasm32_t);

#endif
