from typing import Generator

from wenet.lib._pywrap_wenet import (Params, SimpleAsrModelWrapper,
                                     StreammingAsrWrapper)
from wenet.model import WenetWrapper


class StreammingAsrDecoder:
    '''StreammingAsrDecoder is a class for
    streamming decoding given a model
    '''
    def __init__(self,
                 model : WenetWrapper,
                 nbest :int = 1,
                 continuous_decoding :bool= False):
        self.streamming_decoder = StreammingAsrWrapper(model.model, nbest, continuous_decoding)
        self.continuous_decoding = continuous_decoding
        self.nbest = nbest

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
        result = ""
        final = False
        while not final:
            result, final = self.streamming_decoder.GetInstanceResult(self.streamming_decoder)
            yield result

#        self.Reset(1, self.continuous_decoding)

#    def Reset(self, nbest :int =1 , continuous_decoding :bool = False):
#        self.streamming_decoder.Reset(nbest, continuous_decoding)
