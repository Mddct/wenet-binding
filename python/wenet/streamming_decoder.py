from typing import Generator

import model
from wenet.lib._pywrap_wenet import (Params, SimpleAsrModelWrapper,
                                     StreammingAsrWrapper)


class StreammingAsrDecoder:
    '''StreammingAsrDecoder is a class for
    streamming decoding given a model
    '''
    def __init__(self,
                 model : model.WenetWrapper,
                 nbest :int = 1,
                 continuous_decoding :bool= False):
        self.streamming_decoder = StreammingAsrWrapper(model.model, nbest, continuous_decoding)
        self.continuous_decoding = continuous_decoding

    def AcceptWaveform(self,
                       pcm :bytes,
                       final :bool=True):
        if len(pcm) == 0:
            return
        self.streamming_decoder.AcceptWaveform(pcm, len(pcm), final)

    def GetInstanceResult(self) -> Generator:
        """

        one thread: decoder.AcceptWaveform
        another thread: get result
        for res in decoder.GetInstanceResult:
            print(res)
        Returns:
            Generator:
        """
        while True:
            result = ""
            final = self.streamming_decoder.GetInstanceResult(result)

            yield result

            if final:
                self.Reset(1, self.continuous_decoding)
                break
    def Reset(self, nbest :int =1 , continuous_decoding :bool = False):
        self.streamming_decoder.Reset(nbest, continuous_decoding)
