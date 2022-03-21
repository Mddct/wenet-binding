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
        self.final = False

        self.has_data = False

    def AcceptWaveform(self,
                       pcm :bytes,
                       final :bool=True):
        if len(pcm) == 0:
            return
        # when GetInstanceResult get final result, then must call Reset and then call AcceptWaveform
        if self.final:
            return
        self.has_data = True
        self.streamming_decoder.AcceptWaveform(pcm, int(len(pcm)/2), final)

    def GetInstanceResult(self) -> Generator:
        """

        one thread: decoder.AcceptWaveform
        another thread: get result
        for res in decoder.GetInstanceResult:
            print(res)
        Returns:
            Generator:
        """
        assert (self.has_data is True), "no data received"
        result = ""
        final = False
        while not final:
            result, final = self.streamming_decoder.GetInstanceResult()
            yield result

        self.final = True
        self._reset_()

    def _reset_(self, nbest :int =1 , continuous_decoding :bool = False):
        self.final = False
        self.has_data = False
        self.streamming_decoder.Reset(nbest, continuous_decoding)
        self.nbest = nbest
        self.continuous_decoding = continuous_decoding
