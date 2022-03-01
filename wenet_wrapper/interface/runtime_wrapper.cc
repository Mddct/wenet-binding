#include "boost/json/src.hpp"
#include "post_processor/post_processor.h"
#include "utils/string.h"
#include "utils/timer.h"
#include "utils/utils.h"

#include "runtime_wrapper.h"

namespace json = boost::json;

// serialize Decode Result, this function should be in Decode
std::string SerializeResult(const std::vector<wenet::DecodeResult> &results,
                            bool finish, int n = 1) {
  json::array nbest;
  for (auto &path : results) {
    json::object jpath({{"sentence", path.sentence}});
    if (finish) {
      json::array word_pieces;
      for (auto &word_piece : path.word_pieces) {
        json::object jword_piece({{"word", word_piece.word},
                                  {"start", word_piece.start},
                                  {"end", word_piece.end}});
        word_pieces.emplace_back(jword_piece);
      }
      jpath.emplace("word_pieces", word_pieces);
    }
    nbest.emplace_back(jpath);

    if (nbest.size() == n) {
      break;
    }
  }
  return json::serialize(nbest);
}

std::once_flag once_nitialized_;

std::shared_ptr<wenet::FeaturePipelineConfig>
InitFeaturePipelineConfigFromParams(const Params &params) {
  auto feature_config = std::make_shared<wenet::FeaturePipelineConfig>(
      params.num_bins, params.sample_rate);
  return feature_config;
}

std::shared_ptr<wenet::DecodeOptions>
InitDecodeOptionsFromParams(const Params &params) {
  auto decode_config = std::make_shared<wenet::DecodeOptions>();
  decode_config->chunk_size = params.chunk_size;
  decode_config->num_left_chunks = params.num_left_chunks;
  decode_config->ctc_weight = params.ctc_weight;
  decode_config->reverse_weight = params.ctc_weight;
  decode_config->rescoring_weight = params.reverse_weight;
  decode_config->ctc_wfst_search_opts.max_active = params.max_active;
  decode_config->ctc_wfst_search_opts.min_active = params.min_active;
  decode_config->ctc_wfst_search_opts.beam = params.beam;
  decode_config->ctc_wfst_search_opts.lattice_beam = params.lattice_beam;
  decode_config->ctc_wfst_search_opts.acoustic_scale = params.acoustic_scale;
  decode_config->ctc_wfst_search_opts.blank_skip_thresh =
      params.blank_threshold;
  decode_config->ctc_wfst_search_opts.nbest = params.nbest;
  decode_config->ctc_prefix_search_opts.first_beam_size =
      params.first_beam_size;
  decode_config->ctc_prefix_search_opts.second_beam_size =
      params.second_beam_size;
  return decode_config;
}

std::shared_ptr<wenet::DecodeResource>
InitDecodeResourceFromParams(const Params &params) {
  auto resource = std::make_shared<wenet::DecodeResource>();

  auto model_path = params.model_path;
  auto num_threads = params.num_threads;
  LOG(INFO) << "Reading model " << model_path << " to use " << num_threads
            << " threads";
  auto model = std::make_shared<wenet::TorchAsrModel>();
  model->Read(model_path, num_threads);
  resource->model = model;

  std::shared_ptr<fst::Fst<fst::StdArc>> fst = nullptr;
  if (params.fst_path != "") {
    auto fst_path = params.fst_path;
    LOG(INFO) << "Reading fst " << fst_path;
    fst.reset(fst::Fst<fst::StdArc>::Read(fst_path));
    CHECK(fst != nullptr);
  }
  resource->fst = fst;

  auto dict_path = params.dict_path;
  LOG(INFO) << "Reading symbol table " << dict_path;
  auto symbol_table =
      std::shared_ptr<fst::SymbolTable>(fst::SymbolTable::ReadText(dict_path));
  resource->symbol_table = symbol_table;

  std::shared_ptr<fst::SymbolTable> unit_table = nullptr;
  if (params.unit_path != "") {
    auto unit_path = params.unit_path;
    LOG(INFO) << "Reading unit table " << unit_path;
    unit_table = std::shared_ptr<fst::SymbolTable>(
        fst::SymbolTable::ReadText(unit_path));
    CHECK(unit_table != nullptr);
  } else if (fst == nullptr) {
    LOG(INFO) << "Using symbol table as unit table";
    unit_table = symbol_table;
  }
  resource->unit_table = unit_table;

  if (params.context_path != "") {
    auto context_path = params.context_path;
    LOG(INFO) << "Reading context " << context_path;
    std::vector<std::string> contexts;
    std::ifstream infile(context_path);
    std::string context;
    while (getline(infile, context)) {
      contexts.emplace_back(wenet::Trim(context));
    }

    wenet::ContextConfig config;
    config.context_score = params.context_score;
    resource->context_graph = std::make_shared<wenet::ContextGraph>(config);
    resource->context_graph->BuildContextGraph(contexts, symbol_table);
  }

  wenet::PostProcessOptions post_process_opts;
  post_process_opts.language_type = params.language_type == 0
                                        ? wenet::kMandarinEnglish
                                        : wenet::kIndoEuropean;
  post_process_opts.lowercase = params.lower_case;
  resource->post_processor =
      std::make_shared<wenet::PostProcessor>(std::move(post_process_opts));
  return resource;
}

SimpleAsrModelWrapper::SimpleAsrModelWrapper(const Params &params) {
  std::call_once(once_nitialized_,
                 [&]() { google::InitGoogleLogging("wenet python wrapper:"); });

  feature_config_ = InitFeaturePipelineConfigFromParams(params);
  decode_config_ = InitDecodeOptionsFromParams(params);
  decode_resource_ = InitDecodeResourceFromParams(params);
}

std::string SimpleAsrModelWrapper::Recognize(char *pcm, int num_samples,
                                             int nbest) {
  std::vector<float> pcm_data(num_samples);
  const int16_t *pdata = reinterpret_cast<const int16_t *>(pcm);
  for (int i = 0; i < num_samples; i++) {
    pcm_data[i] = static_cast<float>(*pdata);
    pdata++;
  }
  auto feature_pipeline =
      std::make_shared<wenet::FeaturePipeline>(*feature_config_);
  feature_pipeline->AcceptWaveform(pcm_data);
  feature_pipeline->set_input_finished();
  // resource_->decode_resource->fst = decoding_fst;
  LOG(INFO) << "num frames " << feature_pipeline->num_frames();

  wenet::TorchAsrDecoder decoder(feature_pipeline, decode_resource_,
                                 *decode_config_);
  while (true) {
    wenet::DecodeState state = decoder.Decode();
    if (state == wenet::DecodeState::kEndFeats) {
      decoder.Rescoring();
      break;
    }
  }

  if (decoder.DecodedSomething()) {
    return SerializeResult(decoder.result(), true, nbest);
  }
  return std::string();
  // std::string n_best_result;
  // if (decoder.DecodedSomething()) {
  //   json::array nbests;
  //   for (auto &path : decoder.result()) {
  //     json::object jpath({{"sentence", path.sentence}});
  //     json::array word_pieces;
  //     for (auto &word_piece : path.word_pieces) {
  //       json::object jword_piece({{"word", word_piece.word},
  //                                 {"start", word_piece.start},
  //                                 {"end", word_piece.end}});
  //       word_pieces.emplace_back(jword_piece);
  //     }
  //     jpath.emplace("word_pieces", word_pieces);

  //     nbests.emplace_back(jpath);

  //     if (nbests.size() == nbest) {
  //       break;
  //     }
  //   }
  //   n_best_result = json::serialize(nbests);
  // }
  // return n_best_result;
}

void StreammingAsrWrapper::DecodeThreadFunc(int nbest) {
  while (true) {
    auto state = decoder_->Decode();
    if (state == wenet::DecodeState::kEndFeats) {
      decoder_->Rescoring();
      std::string result = SerializeResult(decoder->result(), true, nbest);
      std::lock_guard<std::mutex> lock(result_mutex_);
      result_ = std::move(result);
      stop_recognition_ = true;
      break;
    } else if (state == wenet::DecodeState::kEndpoint) {
      decoder_->Rescoring();
      std::string result = SerializeResult(decoder_->result(), true, nbest);
      std::lock_guard<std::mutex> lock(result_mutex_);
      result_ = std::move(result);
      // If it's not continuous decoidng, continue to do next recognition
      // otherwise stop the recognition
      if (continuous_decoding_) {
        decoder_->ResetContinuousDecoding();
      } else {
        stop_recognition_ = true;
        break;
      }
    } else {
      if (decoder_->DecodedSomething()) {
        std::string result = SerializeResult(decoder_->result(), false, 1);
        std::lock_guard<std::mutex> lock(result_mutex_);
        result_ = std::move(result);
      }
    }
  }
}

void StreammingAsrWrapper::AccepAcceptWaveform(char *pcm, int num_samples,
                                               bool final) {
  if (pcm == nullptr || num_samples == 0) {
    return;
  }
  // bytes to wavform
  std::vector<float> pcm_data(num_samples);
  const int16_t *pdata = reinterpret_cast<const int16_t *>(pcm);
  for (int i = 0; i < num_samples; i++) {
    pcm_data[i] = static_cast<float>(*pdata);
    pdata++;
  }
  feature_pipeline_->AcceptWaveform(pcm_data);
  if (final) {
    feature_pipeline_->set_input_finished();
  }
}

void StreammingAsrWrapper::Reset(int nbest, bool continuous_decoding) {
  stop_recognition_ = false;
  result_.clear();
  feature_pipeline_->Reset();
  decoder_->Reset();
  CHECK(!decode_thread_->joinable());

  continuous_decoding_ = continuous_decoding;

  decode_thread_ = std::make_unique<std::thread>(
      &StreammingAsrWrapper::DecodeThreadFunc, this, nbest);
}
