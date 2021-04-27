import numpy as np

if __name__ == "__main__":
    with open("vocab") as istream:
        vocab = istream.read().split("\n")

    vocab = set([line.split()[2] if line else -1 for line in vocab])

    unk_code = 99999999
    print(f"Using {unk_code} for unknown tokens.")

    X = np.load("encoded.npy")

    for i in range(X.shape[0]):
        if X[i] not in vocab:
            X[i] = unk_code
    
    np.save("reencoded.npy", X, allow_pickle=False)

