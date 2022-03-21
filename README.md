# wenet-python
wenet python wrapper for runtime

streamming example
```python

import wenet
import time
import threading

params = wenet.Params()
params.chunk_size = 16
params.model_path = "../model/final.zip"
params.dict_path = "../model/words.txt"

model = wenet.WenetWrapper(params)
with open("../model/test.wav", "rb") as f:
    b = f.read()
b = b[44:]

decoder = wenet.StreammingAsrDecoder(model)
def send():
    nbytes = int((16000/1000)*2*800)
    final = False
    r = nbytes
    l = 0
    while True:
        if r == len(b):
            final = True
        decoder.AcceptWaveform(b[l:r], final)

        l = r
        if l >= len(b):
            break

        if r + nbytes < len(b):
            r = r + nbytes
        else:
            r = len(b)

t = threading.Thread(target=send)
t.start()
for res in decoder.GetInstanceResult():
    print(str(res))
t.join()

decoder.AcceptWaveform(b, True)
for res in decoder.GetInstanceResult():
    print(str(res))
```
```bash
[{"sentence":"甚至"}]
[{"sentence":"甚至出现"}]
[{"sentence":"甚至出现交易几"}]
[{"sentence":"甚至出现交易几乎停滞"}]
[{"sentence":"甚至出现交易几乎停滞的情"}]
[{"sentence":"甚至出现交易几乎停滞的情况","word_pieces":[{"word":"甚","start":0,"end":880},{"word":"至","start":880,"end":1120},{"word":"出","start":1120,"end":1400},{"word":"现","start":1400,"end":1720},{"word":"交","start":1720,"end":1960},{"word":"易","start":1960,"end":2120},{"word":"几","start":2120,"end":2400},{"word":"乎","start":2400,"end":2640},{"word":"停","start":2640,"end":2800},{"word":"滞","start":2800,"end":3040},{"word":"的","start":3040,"end":3240},{"word":"情","start":3240,"end":3600},{"word":"况","start":3600,"end":4120}]}]
[{"sentence":"甚至"}]
[{"sentence":"甚至出现"}]
[{"sentence":"甚至出现交易几"}]
[{"sentence":"甚至出现交易几乎停滞"}]
[{"sentence":"甚至出现交易几乎停滞的情"}]
[{"sentence":"甚至出现交易几乎停滞的情况","word_pieces":[{"word":"甚","start":0,"end":880},{"word":"至","start":880,"end":1120},{"word":"出","start":1120,"end":1400},{"word":"现","start":1400,"end":1720},{"word":"交","start":1720,"end":1960},{"word":"易","start":1960,"end":2120},{"word":"几","start":2120,"end":2400},{"word":"乎","start":2400,"end":2640},{"word":"停","start":2640,"end":2800},{"word":"滞","start":2800,"end":3040},{"word":"的","start":3040,"end":3240},{"word":"情","start":3240,"end":3600},{"word":"况","start":3600,"end":4120}]}]
```

```go
# go run example.go
func main(){
model, err := wenet.Load("/root/model")
wav, err := ioutil.ReadFile("/root/model/test.wav")
if err != nil{
panic(err)
}

streamming := wenet.NewStreammingAsrDecoder(model, 1, false)
go func(){

    nbytes := ((16000/1000)*2*200)
    for i := 0; i < len(wav); {
        r := i+nbytes
        final := false
        if r > len(wav){
            r = len(wav)
            final = true
        }
                    streamming.AcceptWaveform(wav[i:r], final)
        i = r
    }
}()
for res := range streamming.Result{
    fmt.Println(res)
}

}
```
```bash
[{"sentence":"甚至"}]
[{"sentence":"甚至出现"}]
[{"sentence":"甚至出现交易几"}]
[{"sentence":"甚至出现交易几乎停滞"}]
[{"sentence":"甚至出现交易几乎停滞的情"}]
[{"sentence":"甚至出现交易几乎停滞的情况","word_pieces":[{"word":"甚","start":0,"end":880},{"word":"至","start":880,"end":1120},{"word":"出","start":1120,"end":1400},{"word":"现","start":1400,"end":1720},{"word":"交","start":1720,"end":1960},{"word":"易","start":1960,"end":2160},{"word":"几","start":2160,"end":2400},{"word":"乎","start":2400,"end":2640},{"word":"停","start":2640,"end":2800},{"word":"滞","start":2800,"end":3040},{"word":"的","start":3040,"end":3240},{"word":"情","start":3240,"end":3600},{"word":"况","start":3600,"end":4160}]}]

```

# TODO:
- [x] simple wrapper support load and recognize funtion
- [x] wheel test
- [x] download model and model dir
- [x] offline go binding
- [ ] github action many linux
- [ ] exception
- [ ] asynchronous for streamming decoding
- [ ] streamming decode go example
- [ ] reset new utterance for same decoder
