# wenet-python
binding wrapper for runtime

features:
-  go binding

-  python binding

-  streamming and nonstreamming

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
- [x] asynchronous for streamming decoding
- [x] streamming decode go example
- [x] reset new utterance for same decoder
