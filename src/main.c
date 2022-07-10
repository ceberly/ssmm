// Run the tests, mostly.

#include <stdlib.h>
#include <stdio.h>

#include "tree.h"
#include "kandr.h"

int main(void) {
  if (!test_tree()) goto fail;
  if (!test_kandr()) goto fail;

  printf("passed.\n");
  return EXIT_SUCCESS;
fail:
  printf("failed.\n");
  return EXIT_FAILURE;
}
