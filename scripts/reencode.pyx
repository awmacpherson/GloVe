import numpy as np
import cython
import ctypes


cdef Py_ssize_t i
cdef int [:] X_view

MAX_LEN = 2*(2**30)
MAX_VSIZE = 10*(2**20)

@cython.wraparound(False)
@cython.boundscheck(False)
cpdef replace(int[:] X, vocab, int unk_code):
    cdef Py_ssize_t i
    for i in range(X.shape[0]):
        if X[i] not in vocab:
            X[i] = unk_code
    
@cython.wraparound(False)
@cython.boundscheck(False)
cpdef reencode(int[:] X, int[:] reencoding):
    cdef Py_ssize_t i
    for i in range(X.shape[0]):
        if X[i] != -1:
            X[i] = reencoding[X[i]]


def compose(vocab_in, vocab_out, mapping):
    vocabf = open(vocab_in)
    with open(vocab_out, "w") as ostream:
        for line in vocabf.readlines():
            try:
                word, code = line.split(" ")
            except ValueError:
                print(line)
            ostream.write(f"{word} {mapping[int(code)])}\n")
    vocabf.close()

def load(path):
    if path.endswith(".npy"):
        print("Loading numpy file...")
        X = np.load(path)
        return X
    else:
        print("Loading binary file...")
        X = np.ndarray(shape=MAX_LEN, dtype=np.int32)
        with open(path, "rb") as istream:
            n = istream.readinto(X)
            print(f"{n} bytes read.")
            return X[:n//4]

def save(X, path):
    if path.endswith(".npy"):
        print("Saving as numpy...")
        np.save(path, X, allow_pickle=False)
    else:
        print(f"Saving as binary of length {X.nbytes}...")
        with open(path, "wb") as ostream:
            ostream.write(bytes(X)[:X.nbytes])

def load_vocab(path):
    with open(path) as istream:
        vocab = istream.readlines()
    vocab = [int(line.split(" ")[1]) for line in vocab]
    return vocab

def generate_reencoding(vocab):
    fwd = np.ndarray(shape=MAX_VSIZE+1, dtype=np.int32)
    for x, y in enumerate(vocab):
        fwd[y] = x
    return fwd

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser(description="Map unrecognised tokens to a single symbol.")
    parser.add_argument("src", nargs="?", default="encoded.npy")
    parser.add_argument("--vocab", nargs="?", default="vocab")
    parser.add_argument("--output-vocab", nargs="?", default="vocab")
    parser.add_argument("--output-encoded", "-o", nargs="?", default="reencoded")
    args = parser.parse_args()

    print("Loading vocabulary...")
    vocab = load_vocab(args.vocab)
    print("Generating reencoding...")
    fwd = generate_reencoding(vocab)
    fwd[MAX_VSIZE] = len(vocab)
    compose(args.vocab, args.output_vocab, fwd)

    X = load(args.src)
    print(f"Replacing unknown tokens with {MAX_VSIZE}...")
    replace(X, set(vocab), MAX_VSIZE)
    print("Remapping...")
    reencode(X, fwd)
    
    save(X, args.output_encoded)
