#ifndef _WENET_H
#define _WENET_H

#include "wenet_wrapper/interface/runtime_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif
void *wenet_construct(const struct Params *params);
void wenet_free(void *model);
// caller should cal free
char *wenet_recognize(void *model, char *data, int n_samples, int nbest);
#ifdef __cplusplus
}
#endif

#endif
