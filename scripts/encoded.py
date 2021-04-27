import numpy as np

if __name__ == "__main__":
    with open("encoded", "rb") as istream:
        data = istream.read()
    
    np.save("encoded.npy", 
            np.ndarray(shape=len(data)//4, dtype=np.int32, buffer=data),
            allow_pickle=False)
