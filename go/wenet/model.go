package model

import "C"

// #cgo CFLAGS: -I${SRCDIR}
// #include "wenet.h"
import (
	"runtime"
	"unsafe"
)

func New(params *Params) *SimpleAsrModelWrapper {
	var model *C.struct_Model
	model = C.wenet_init(params.params)

	runtime.SetFinalizer(model, free)
	return model
}
func free(model *SimpleAsrModelWrapper) {
	C.wenet_free(model.inst)
}

type Params struct {
	parms *C.struct_cParams
}
type SimpleAsrModelWrapper struct {
	inst *C.struct_Model
}

func (samw *SimpleAsrModelWrapper) Recognize(pcm []byte) string {
	if len(pcm) == 0 {
		return ""
	}

	res := C.wenet_recognize(
		C.CBytes(pcm),
		len(pcm),
	)
	defer C.free(unsafe.Pointer(res))
	C.GoString(res)
}
