
from wenet.lib._pywrap_wenet import Params, SimpleAsrModelWrapper

import json
import wave
import os

# TODO: other model support eg: wenetspeech
MODELS = {
    "zh-cn":"http://mobvoi-speech-public.ufile.ucloud.cn/public/wenet/multi_cn/20210815_unified_conformer_server.tar.gz",
    "en":"http://mobvoi-speech-public.ufile.ucloud.cn/public/wenet/gigaspeech/20210728_u2pp_conformer_server.tar.gz",
}

class WenetWrapper(object):
        def __init__(self, params:Params):
            if not params.model_path:
                raise FileNotFoundError("final.zip not found in {}".format(params.model_path))
            if not params.dict_path:
                raise FileNotFoundError("words.txt not found in {}".format(params.dict_path))

            self.params = params
            self.model = SimpleAsrModelWrapper(params)

        def recognize(
                self, 
                filepath: str, 
                nbest: int = 1):
            """
            Args:

                filepath (path-like object or file-like object):
                Source of audio data.

            Returns:
                res(str)
            """

            with wave.open(filepath, "rb") as f:
                assert f.getnchannels() == 1
                assert f.getframerate() == self.params.sample_rate 
                length = int(f.getnframes())
                wav_bytes = f.readframes(length)

            return self.model.recognize(wav_bytes, length, nbest)


def load_model(model_dir=None, language=None):
    params = Params()
    if model_dir:
        if not os.path.exists(os.path.join(model_dir,"final.zip")):
            raise FileNotFoundError("final.zio not found in {}".format(model_dir))
        if not os.path.exists(os.path.join(model_dir,"words.txt")):
            raise FileNotFoundError("words.txt not found in {}".format(model_dir))
        params.model_path = os.path.join(model_dir, "final.zip")
        params.dict_path = os.path.join(model_dir, "words.txt")
        
    elif language:
        # TODO: language
        pass
    else:
        raise ValueError("must specify model_path or language")
    return WenetWrapper(params)
