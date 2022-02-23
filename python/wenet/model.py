import json
import os
import pathlib
import wave

from wenet.assets import MODELS
from wenet.download import download
from wenet.lib._pywrap_wenet import Params, SimpleAsrModelWrapper


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
            res (str)
        """

        with wave.open(filepath, "rb") as f:
            assert f.getnchannels() == 1
            assert f.getframerate() == self.params.sample_rate
            length = int(f.getnframes())
            wav_bytes = f.readframes(length)

        return self.model.recognize(wav_bytes, length, nbest)


def load_model(model_dir=None, language=None, cache_dir=pathlib.Path.home()):
    params = Params()
    if model_dir:
        if not os.path.exists(os.path.join(model_dir,"final.zip")):
            raise FileNotFoundError("final.zio not found in {}".format(model_dir))
        if not os.path.exists(os.path.join(model_dir,"words.txt")):
            raise FileNotFoundError("words.txt not found in {}".format(model_dir))
    elif language:
        url = MODELS.get(language, "")
        if not url:
            raise ValueError("{} not supported".format(language))
        # TODO: exception
        model_dir =  download(url, parent_dir=cache_dir)
    else:
        raise ValueError("must specify model_path or language")

    params.model_path = os.path.join(model_dir, "final.zip")
    params.dict_path = os.path.join(model_dir, "words.txt")
    return WenetWrapper(params)
