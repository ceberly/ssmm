// Run the tests, mostly.

#include <stdlib.h>
#include <stdio.h>

#include "tree.h"

int main(void) {
  if (test_tree()) {
    printf("passed.\n");
    return EXIT_SUCCESS;
  }

  printf("failed.\n");
  return EXIT_FAILURE;
}
