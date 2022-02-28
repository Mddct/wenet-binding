package wenet

// #cgo CFLAGS: -I .
// #cgo LDFLAGS: -L .   -lcgowenet
// #include "wenet.h"
// #include <stdlib.h>
import "C"

import (
	"os"
	"path"
	"runtime"
	"unsafe"
)

// TODO: default params
func Load(modelDir string) (*SimpleAsrModelWrapper, error) {
	zip := path.Join(modelDir, "final.zip")
	dict := path.Join(modelDir, "words.txt")
	if _, err := os.Stat(zip); err != nil {
		return nil, err
	}
	if _, err := os.Stat(dict); err != nil {
		return nil, err
	}
	ctcopts := WithCtc(0, 10, 10)
	// feature pipeline opts
	fpopts := WithFeaturePipeline(80, 16000)
	modelopts := WithModel(zip, dict, 1)

	p := NewParams(ctcopts, fpopts, modelopts)
	return New(p), nil
}

func New(params *Params) *SimpleAsrModelWrapper {
	var model *C.struct_model
	model = C.wenet_init(params.params)

	m := &SimpleAsrModelWrapper{model}
	runtime.SetFinalizer(m, free)
	return m

}
func free(model *SimpleAsrModelWrapper) {
	C.wenet_free(model.inst)
}

type Params struct {
	params *C.struct_cParams
}

func NewParams(opts ...ParamsOpts) *Params {
	p := &Params{
		C.wenet_params_init(),
	}

	for _, opt := range opts {
		opt(p)
	}
	return p
}

type ParamsOpts func(*Params)

func WithCtc(blank int, firstBeamSize int, secondBeamSize int) ParamsOpts {
	return func(p *Params) {
		opts := p.params
		C.wenet_params_set_ctc_opts(
			opts,
			C.int(blank),
			C.int(firstBeamSize),
			C.int(secondBeamSize),
		)
	}
}

func WithtWfst(
	maxActive int,
	minActive int,
	beam int,
	latticeBeam float64,
	acousticScale float64,
	blankSkipThresh float64,
	nbest int) ParamsOpts {
	return func(p *Params) {
		opts := p.params
		C.wenet_params_set_wfst_opts(
			opts,
			C.int(maxActive),
			C.int(minActive),
			C.int(beam),
			C.double(latticeBeam),
			C.double(acousticScale),
			C.double(blankSkipThresh),
			C.int(nbest),
		)
	}
}

func WithModel(modelpath string, dictpath string, numThreads int) ParamsOpts {
	return func(p *Params) {
		opts := p.params
		// c model path
		cmp := C.CString(modelpath)
		// c dict path
		cdp := C.CString(dictpath)

		defer C.free(unsafe.Pointer(cmp))
		defer C.free(unsafe.Pointer(cdp))
		C.wenet_params_set_model_opts(
			opts,
			cmp,
			cdp,
			C.int(numThreads),
		)
	}
}

func WithFeaturePipeline(numBins int, sampleRate int) ParamsOpts {
	return func(p *Params) {
		opts := p.params
		// c model path
		C.wenet_params_set_feature_pipeline_opts(
			opts,
			C.int(numBins),
			C.int(sampleRate),
		)
	}
}

func WithDecodeOpts(
	chunkSize int,
	ctcWeight float64,
	rescoreWeight float64,
	reverseWeight float64) ParamsOpts {
	return func(p *Params) {
		opts := p.params
		C.wenet_params_set_decode_opts(
			opts,
			C.int(chunkSize),
			C.double(ctcWeight),
			C.double(rescoreWeight),
			C.double(reverseWeight),
		)
	}
}

type SimpleAsrModelWrapper struct {
	inst *C.struct_model
}

func (samw *SimpleAsrModelWrapper) Recognize(pcm []byte, nbest int) string {
	if len(pcm) == 0 {
		return ""
	}
	cBytes := C.CBytes(pcm)
	defer C.free(cBytes)
	res := C.wenet_recognize(
		samw.inst,
		(*C.char)(cBytes),
		C.int(len(pcm)/2),
		C.int(nbest),
	)
	defer C.free(unsafe.Pointer(res))
	return C.GoString(res)
}
