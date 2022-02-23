#include <stdlib.h>

#include "wenet.h"

#ifdef __cplusplus
extern "C" {
#endif
void *wenet_construct(const struct Params *params) {
  if (params == nullptr) {
    return nullptr;
  }
  auto model_ptr = new SimpleAsrModelWrapper(*params);
  return (void *)model_ptr;
}
void wenet_free(void *model) {
  auto model = static_cast<SimpleAsrModelWrapper *>(model);
  delete model;
}
// caller should cal free
char *wenet_recognize(void *model, char *data, int n_samples, int nbest) {
  auto model = static_cast<SimpleAsrModelWrapper *>(model);
  std::string result(std::move(model->Recognize(data, n_samples, nbest)));
  auto cstr = result.c_str();
  char *res = malloc(result.size());
  memcpy(res, cstr, result.size());
  return res;
}

#ifdef __cplusplus
}
#endif
