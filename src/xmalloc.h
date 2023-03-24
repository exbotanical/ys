#ifndef XMALLOC_H
#define XMALLOC_H

#include <stdlib.h>  // for size_t, malloc

/**
 * xmalloc is a malloc wrapper that exits the program if out of memory
 *
 * @param sz
 * @return void*
 */
void* xmalloc(size_t sz);

#endif /* XMALLOC_H */
