#ifndef WENET_PYTHON_LIB_H_
#define WENET_PYTHON_LIB_H_

#include "decoder/torch_asr_decoder.h"
#include "utils/utils.h"

struct Params {
  // model and dict
  std::string model_path;
  std::string dict_path;
  std::string unit_path;

  int num_threads;

  // fst
  std::string fst_path;

  // context
  std::string context_path;
  double context_score;

  // frontend
  int num_bins;
  int sample_rate;

  // decode options
  int chunk_size;
  int num_left_chunks;
  double ctc_weight;
  double rescoring_weight;
  double reverse_weight;

  int blank;
  double blank_threshold; // blank threshold to be silence

  // ctc prefix
  int first_beam_size;
  int second_beam_size;

  // ctc wfst
  double acoustic_scale;
  double nbest;
  double blank_skip_thresh;
  int max_active;
  int min_active;
  double beam;
  double lattice_beam;

  // languge
  int language_type;
  bool lower_case;

  Params(std::string model_path, std::string dict_path, std::string unit_path,
         int num_threads, std::string fst_path, std::string context_path,
         double context_score, int num_bins, int sample_rate, int chunk_size,
         int num_left_chunks, double ctc_weigth, double rescoring_weight,
         double reverse_weight, int blank, double blank_threshold,
         int first_beam_size, int second_beam_size, double acoustic_scale,
         double nbest, double blank_skip_thresh, int max_active, int min_active,
         double beam, double lattice_beam, int language_type, bool lower_case)
      : model_path(model_path), dict_path(dict_path), unit_path(unit_path),
        num_threads(num_threads), fst_path(fst_path),
        context_path(context_path), context_score(context_score),
        num_bins(num_bins), sample_rate(sample_rate), chunk_size(chunk_size),
        num_left_chunks(num_left_chunks), ctc_weight(ctc_weight),
        rescoring_weight(rescoring_weight), reverse_weight(reverse_weight),
        blank(blank), blank_threshold(blank_threshold),
        first_beam_size(first_beam_size), second_beam_size(second_beam_size),
        acoustic_scale(acoustic_scale), nbest(nbest),
        blank_skip_thresh(blank_skip_thresh), max_active(max_active),
        min_active(min_active), beam(beam), lattice_beam(lattice_beam),
        language_type(language_type), lower_case(lower_case) {}
};

// for google log init once
std::once_flag once_nitialized_;

class SimpleAsrModelWrapper {
public:
  SimpleAsrModelWrapper(const Params &params);
  std::string Recognize(char *pcm, int num_sampleds, int n_best = 1);

private:
  std::shared_ptr<wenet::FeaturePipelineConfig> feature_config_;
  std::shared_ptr<wenet::DecodeOptions> decode_config_;
  std::shared_ptr<wenet::DecodeResource> decode_resource_;

};

#endif // WENET_PYTHON_LIB_H_
