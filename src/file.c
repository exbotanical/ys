#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libhttp.h"
#include "logger.h"
#include "util.h"
#include "xmalloc.h"

char* from_file(const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    DIE("Unable to open file %s. Are you sure the path is correct?\n",
        filename);
  }

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* buf = xmalloc(fsize + 1);
  fread(buf, fsize, 1, fp);
  fclose(fp);

  buf[fsize] = NULL_TERM;

  return buf;
}
