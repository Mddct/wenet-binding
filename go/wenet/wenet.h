#ifndef _WENET_H
#define _WENET_H

#include "wenet_wrapper/interface/runtime_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cParams cParams;
cParams *wenet_params_init();
void wenet_params_free(cParams *cParams);

// wfst beam search option
void wenet_params_set_wfst_opts(cParams *pcparm, int max_active, int min_active,
                                int beam, double lattice_beam,
                                double acoustic_scale, double blank_skip_thresh,
                                int nbest);

// ctc beam seatch option
void wenet_params_set_ctc_opts(cParams *pcparam, int blank, int first_beam_size,
                               int second_beam_size);
void wenet_params_set_wfst_opts(cParams *pcparm, int max_active, int min_active,
                                int beam, double lattice_beam,
                                double acoustic_scale, double blank_skip_thresh,
                                int nbest);

void wenet_params_set_model_opts(cParams *pcparm, char *model_path,
                                 char *dict_path, int num_threads);

typedef struct model Model;
Model *wenet_init(const cParams *cparams);
void wenet_free(Model *model);
// caller should cal free
char *wenet_recognize(Model *model, char *data, int n_samples, int nbest);
#ifdef __cplusplus
}
#endif

#endif
