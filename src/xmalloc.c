#include "xmalloc.h"

#include "logger.h"

void* xmalloc(size_t sz) {
  void* ptr;
  if ((ptr = malloc(sz)) == NULL) {
    DIE(EXIT_FAILURE, "%s failed to allocate memory\n", __func__);
  }

  return ptr;
}
