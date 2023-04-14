#ifndef XMALLOC_H
#define XMALLOC_H

#include <stdlib.h>

/**
 * xmalloc is a malloc wrapper that exits the program if out of memory
 */
void* xmalloc(size_t sz);

#endif /* XMALLOC_H */
