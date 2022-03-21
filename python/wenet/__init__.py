from ctypes import cdll
from os import path

root_path = path.dirname(__file__)
libs_dirname = path.join(root_path, 'lib')

libs = [
    "libtorch.so",
    "libtorch_cpu.so",
    "libc10.so",
    "libfst.so.8",
    "libgomp-a34b3233.so.1",
]


for lib in libs:
    cdll.LoadLibrary(path.join(libs_dirname, lib))

from wenet.lib._pywrap_wenet import Params
from wenet.model import StreammingAsrDecoder, WenetWrapper, load_model
from wenet.streamming_decoder import StreammingAsrDecoder
