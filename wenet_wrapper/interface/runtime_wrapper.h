#ifndef WENET_PYTHON_LIB_H_
#define WENET_PYTHON_LIB_H_

#include <mutex>
#include <thread>

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
extern std::once_flag once_nitialized_;

class SimpleAsrModelWrapper {
public:
  SimpleAsrModelWrapper(const Params &params);
  std::string Recognize(char *pcm, int num_sampleds, int n_best = 1);

  std::shared_ptr<wenet::FeaturePipelineConfig> feature_config() {
    return feature_config_;
  }
  std::shared_ptr<wenet::DecodeOptions> decode_config() {
    return decode_config_;
  }
  std::shared_ptr<wenet::DecodeResource> decode_resource() {
    return decode_resource_;
  }

private:
  std::shared_ptr<wenet::FeaturePipelineConfig> feature_config_;
  std::shared_ptr<wenet::DecodeOptions> decode_config_;
  std::shared_ptr<wenet::DecodeResource> decode_resource_;
};

class StreammingAsrWrapper {
public:
  StreammingAsrWrapper(std::shared_ptr<SimpleAsrModelWrapper> model,
                       int nbest = 1, bool continuous_decoding = false)
      : model_(model),
        feature_pipeline_(std::make_shared<wenet::FeaturePipeline>(
            *model_->feature_config())),
        decoder_(std::make_shared<wenet::TorchAsrDecoder>(
            feature_pipeline_, model_->decode_resource(),
            *model_->decode_config())),
        decode_thread_(std::make_unique<std::thread>(
            &StreammingAsrWrapper::DecodeThreadFunc, this, nbest)),
        continuous_decoding_(continuous_decoding) {}

  ~StreammingAsrWrapper() {
    if (decode_thread_->joinable()) {
      decode_thread_->join();
    }
  }
  // caller: onethread call accept wavform
  // another call GetInstanceResult and check IsEnd
  void AccepAcceptWaveform(char *pcm, int num_samples, bool final);
  // decoder set current result , and return if it final
  bool GetInstanceResult(std::string &result);

  // reset for new utterance
  void Reset(int nbest = 1, bool continuous_decoding = false);

private:
  void DecodeThreadFunc(int nbest);

  std::shared_ptr<SimpleAsrModelWrapper> model_;
  std::shared_ptr<wenet::FeaturePipeline> feature_pipeline_;
  std::shared_ptr<wenet::TorchAsrDecoder> decoder_;

  // decode thread for seperate decoding
  std::unique_ptr<std::thread> decode_thread_;

  // contronl decoding opts
  // continuous decoding
  bool continuous_decoding_;
  // start and end signal, caller should set start and end_, no mutex needed
  // model detect feature pipeline finished or  end_ is true
  bool stop_recognition_;

  // instant results, Channels should be used here but locks are used for
  // simplicity
  mutable std::mutex result_mutex_;
  std::condition_variable has_result_;
  std::string result_;
};
#endif // WENET_PYTHON_LIB_H_
