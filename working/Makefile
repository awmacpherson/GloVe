TRUNCATE = 7
WINDOW = 5
MEMORY = 8
CORPUS = WestburyLab.Wikipedia.Corpus.txt

REPO = ..

all: twogram.txt

vocab: $(CORPUS)
	$(REPO)build/vocab_count -min-count $(TRUNCATE) < $^ > vocab

REMAP_INP = encoded --vocab vocab
REMAP_OUT = --output-encoded encoded_remap --output-vocab vocab_remap

.PHONY: remap
remap: encoded_remap vocab_remap

encoded_remap vocab_remap: vocab encoded
	$(REPO)/scripts/reencode $(REMAP_INP) $(REMAP_OUT)

OL = 536870912
TWOGRAMOPTS = -max-memory $(MEMORY) -window-size $(WINDOW) -overflow-length $(OL)

twogram: encoded_remap
	$(REPO)/build/cooccur $(TWOGRAMOPTS) < encoded_remap > cooccur.bin
	$(REPO)/build/shuffle < cooccur.bin > cooccur.shuf.bin
	$(REPO)/scripts/twogram cooccur.bin -o cooccur -max-memory $(MEMORY)

.PHONY: clean clobber

clean:
	rm cooccur.bin cooccur.shuf.bin encoded

clobber: clean
	rm vocab twogram.*
