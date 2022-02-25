#include <stdlib.h>

#include "wenet.h"

#ifdef __cplusplus
extern "C" {
#endif
cParams *wenet_params_init() {
  auto params_ptr = new Params();
  return (cParams *)params_ptr;
}
void wenet_params_free(cParams *cparams) {
  if (cparams == NULL) {
    return;
  }
  Params *pp = (Params *)cparams;
  delete pp;
}

void wenet_params_set_ctc_opts(cParams *pcparm, int blank, int first_beam_size,
                               int second_beam_size) {
  if (pcparm == NULL) {
    return;
  }
  Params *pp = (Params *)pcparm;
  pp->blank = blank;
  pp->first_beam_size = first_beam_size;
  pp->second_beam_size = second_beam_size;
  return;
}
void wenet_params_set_wfst_opts(cParams *pcparm, int max_active, int min_active,
                                int beam, double lattice_beam,
                                double acoustic_scale, double blank_skip_thresh,
                                int nbest) {
  if (pcparm == NULL) {
    return;
  }
  Params *pp = (Params *)pcparm;
  pp->max_active = max_active;
  pp->min_active = min_active;
  pp->beam = beam;
  pp->lattice_beam = lattice_beam;
  pp->acoustic_scale = acoustic_scale;
  pp->blank_skip_thresh = blank_skip_thresh;
  pp->nbest = nbest;
  return;
}

void wenet_params_set_decode_opts(cParams *pcparm, int chunk_size,
                                  double ctc_weight, double rescoring_weight,
                                  double reverse_weight) {
  if (pcparm == NULL) {
    return;
  }
  Params *pp = (Params *)pcparm;
  pp->chunk_size = chunk_size;
  pp->ctc_weight = ctc_weight;
  pp->rescoring_weight = reverse_weight;
  pp->reverse_weight = reverse_weight;
  return;
}

void wenet_params_set_model_opts(cParams *pcparm, char *model_path,
                                 char *dict_path, int num_threads) {
  if (pcparm == NULL) {
    return;
  }
  Params *pp = (Params *)pcparm;
  pp->model_path = model_path;
  pp->dict_path = dict_path;
  pp->num_threads = num_threads;
  return;
}

Model *wenet_init(const cParams *pcparm) {
  if (pcparm == nullptr) {
    return nullptr;
  }
  const Params *pp = (Params *)pcparm;
  auto m = new SimpleAsrModelWrapper(*pp);
  return (Model *)m;
}
void wenet_free(Model *model) {
  SimpleAsrModelWrapper* m = (SimpleAsrModelWrapper*)model;
  delete m;
}
// caller should call free result
char *wenet_recognize(Model *model, char *data, int n_samples, int nbest) {
  SimpleAsrModelWrapper* m = (SimpleAsrModelWrapper*)model;
  std::string result(std::move(m->Recognize(data, n_samples, nbest)));
  auto cstr = result.c_str();
  char *res = (char*)malloc(result.size());
  memcpy(res, cstr, result.size());
  return res;
}

#ifdef __cplusplus
}
#endif
