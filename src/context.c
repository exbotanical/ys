#include <string.h>  // for strcmp

#include "libhttp.h"

parameter_t* req_get_parameter_at(req_t* req, unsigned int idx) {
  return array_get(req->parameters, idx);
}

void* req_get_parameter(req_t* req, const char* key) {
  for (unsigned int i = 0; i < req_num_parameters(req); i++) {
    parameter_t* param = req_get_parameter_at(req, i);
    if (strcmp(param->key, key) == 0) {
      return param->value;
    }
  }

  return NULL;
}

bool req_num_parameters(req_t* req) { return array_size(req->parameters); }

bool req_has_parameters(req_t* req) {
  return req && array_size(req->parameters) > 0;
}
