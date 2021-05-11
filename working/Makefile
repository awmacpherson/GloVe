TRUNCATE = 7
WINDOW = 5
MEMORY = 8
CORPUS = WestburyLab.Wikipedia.Corpus.txt

REPO = ..

all: twogram.txt

vocab: $(CORPUS)
	$(REPO)/build/vocab_count -max-vocab $(TRUNCATE) < $^ > vocab

REMAP_INP = encoded --vocab vocab
REMAP_OUT = --output-encoded encoded_remap --output-vocab vocab_remap

.PHONY: remap
remap: encoded_remap vocab_remap

encoded_remap vocab_remap: vocab encoded
	$(REPO)/scripts/remap $(REMAP_INP) $(REMAP_OUT)

TWOGRAMOPTS = -max-memory $(MEMORY) -window-size $(WINDOW)

2-gram: 2-gram.manifest

2-gram.manifest: encoded_remap
	$(REPO)/build/cooccur $(TWOGRAMOPTS) < encoded_remap > cooccur.bin
	$(REPO)/build/shuffle < cooccur.bin > cooccur.shuf.bin
	$(REPO)/scripts/twogram cooccur.shuf.bin -o 2-gram --max-memory $(MEMORY)

.PHONY: clean clobber

clean:
	rm cooccur.bin cooccur.shuf.bin encoded encoded_remap

clobber: clean
	rm vocab vocab_remap 2-gram.*
