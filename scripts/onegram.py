import numpy as np, sys
VOCAB = "vocab"

def get_onegram_array(vfile=VOCAB):
    X = []
    with open(vfile) as stream:
        for line in stream.read().split('\n'):
            try:
                X.append(line.split(" ")[1])
            except IndexError:
                break

    return np.array(X, dtype=np.int32)

if __name__ == '__main__':
    try:
        vfile = sys.argv[1]
    except IndexError:
        vfile = VOCAB
    X = get_onegram_array(vfile)
    np.save("one-gram.npy", X, allow_pickle=False)

    print(f"Total vocab length: {len(X)}")
    print(f"Tokens occurring more than once: {len(X[X>1])}")
    print(f"Tokens occurring more than twice: {len(X[X>2])}")
    print(f"Tokens occurring more than thrice: {len(X[X>3])}")
