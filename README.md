# wenet-binding
binding wrapper for runtime (early stage)

## Highlights:

* go binding
* python binding
* streamming and nonstreamming decoding
* c interface you can binding for other language
* some instresting tools eg: label_check

## example:
- [go example](go/README.md)
- [python example](python/README.md) 

# TODO:
- [x] simple wrapper support load and recognize funtion
- [x] wheel test
- [x] download model and model dir
- [x] offline go binding
- [ ] github action many linux
- [ ] test case cover 
- [ ] wenet ctc decoder for batch inference and mwer loss
- [ ] exception
- [x] non streamming label check
- [x] asynchronous for streamming decoding
- [x] streamming decode go example
- [x] reset new utterance for same decoder
