#include "libhttp.h"

parameter_t* context_get_parameter_at(route_context_t* context,
                                      unsigned int idx) {
  return array_get(((__route_context_t*)context)->parameters, idx);
}

void* context_get_parameter(route_context_t* context, const char* key) {
  for (unsigned int i = 0; i < context_num_parameters(context); i++) {
    parameter_t* param = context_get_parameter_at(context, i);
    if (strcmp(param->key, key) == 0) {
      return param->value;
    }
  }

  return NULL;
}

bool context_num_parameters(route_context_t* context) {
  return array_size(((__route_context_t*)context)->parameters);
}

bool context_has_parameters(route_context_t* context) {
  __route_context_t* internal_context = (__route_context_t*)context;
  return internal_context && array_size((internal_context)->parameters) > 0;
}
