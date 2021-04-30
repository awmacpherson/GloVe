# GloVe: Global Vectors for Word Representation

This fork of the original GloVe package includes some tweaks to the original algorithms and adds tools (compiled [CP]ython scripts) for remapping the preprocessed data and serialising them as (platform-agnostic) NumPy objects. Changes were made to `common.[ch]`, `cooccur.c`, `vocab-count.c`. and the output of these programs is not entirely compatible with the original repository. 

## Changes

- The original `get_word` (defined in `common.c`)algorithm to something more sensible. (The `get_word` function found in the original repo is not even the one they used in their experiments: the paper states that they used the Stanford tokenizer.)
- `vocab-count.c` now generates a fixed-width (32-bit) integer encoding of the tokenized corpus and saves it in the working directory as `encoded`. The `HASHREC` struct defined in `common.h` and used in `vocab-count.c` has a new field which records this encoding.
- The original version of `cooccur.c` used the corpus and the vocabulary generated by `vocab-count.c` as input. Now it only uses the file `encoded`. (The functionality of checking tokens against the vocabulary and collapsing unknown tokens has been outsourced to `remap.pyx`.)
- Except for `glove.c` (which uses `pthreads.h`), the repo will now compile on Windows (tested with MSVC 2019 and MinGW gcc; note that MinGW won't compile the Cython scripts). Previously there were some non-compliant format strings that caused trouble.
- I haven't made any changes to or tested `glove.c`: the purpose of this fork was to use the preprocessing code to generate input files to a Python reimplementation.

Usage:
```shell
make # compile and link C code in /src directory
make scripts # compile Cython scripts in place in /scripts directory
```

## Scripts
I added some Cython scripts. If you want to compile these by hand, you must run `cython` with the `--embed` flag to generate C code with a `main` function.
- `remap.pyx`
- `twogram.pyx`
- `encoded.py` --- this simple script wraps the binary `encoded` output of `vocab-count` with a NumPy header and saves it with a `.npz` extension.

## Running the preprocessing
You can use the `Makefile` in `./working` to run the preprocessing (perhaps this is a somewhat idiosyncratic approach, but it seems to work well).

Usage:
```shell
make vocab CORPUS={path_to_corpus.txt} # run vocab-count, yielding vocab mapping + fixed-width encoded corpus
make encoded

make remap # this runs the script for collapsing unknown tokens

make two.npz # run cooccur.c and shuffle.c, and wrap the output. This might take a while and use a lot of disk space.

make clean # delete intermediate files, leaving only the original corpus, vocab, and two.npz.
```
Unless you are preprocessing the data inside the actual repo `working` directory, you are going to want to pass `REPO={path_to_repo}` as an argument to `make` or write this into the `Makefile`. You should make sure that you have on the order of `sizeof(corpus) * 3` disk space free (the exact number will depend on the arguments you pass to the preprocessing scripts).

### License
All work contained in this package is licensed under the Apache License, Version 2.0. See the include LICENSE file.
