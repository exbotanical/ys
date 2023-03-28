#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

static int c = 1;
#define checkpoint() printf("CHECKPOINT %d\n", c++)

#define isnull(thing) printf("IS NULL? %s\n", thing == NULL ? "yes" : "no")

#define print(thing) printf("%s=%s\n", #thing, thing)

#endif /* DEBUG_H */
