import sys, os
import numpy as np
import struct
from argparse import ArgumentParser

INT_LEN = 4
DOUBLE_LEN = 8
CREC_LEN = 2*INT_LEN + DOUBLE_LEN
BO = '@' # native

CRec = struct.Struct('@iid')

fidcounter = 0

def write_to_npy(X_i, X_v, length, tar='two-gram.npz'):
    global fidcounter
    tar = tar[:-4] + f"-{fidcounter:02}.npz"
    fidcounter += 1
    print(f"Writing chunk {fidcounter}...")
    np.savez(tar, X_i=X_i[:length], X_v=X_v[:length])
       
def extract_crecs(int[:,:] X_i, double[:] X_v, crecs):
    cdef int j, i0, i1
    cdef double v
    for j in range(len(crecs)//CREC_LEN):
        i0, i1, v = CRec.unpack(crecs[j*CREC_LEN:(j+1)*CREC_LEN])
        X_i[j,0] = i0
        X_i[j,1] = i1
        X_v[j] = v
    return j + 1 # number of crecs transferred


if __name__ == '__main__':

    parser = ArgumentParser(description="Convert cooccur file from .bin to .npy.")
    parser.add_argument('src', nargs='?', default='cooccur.bin')
    parser.add_argument('--output', '-o', nargs='?', default='two-gram.npz')
    parser.add_argument('--max-memory', type=float, nargs='?', 
                        default=1.0, help="Max memory in GB.")
    args = parser.parse_args()
    


    buflen = int(args.max_memory * (2**30) / (2 * CREC_LEN))

    X_i = np.ndarray(shape=(buflen,2), dtype=np.int32)
    X_v = np.ndarray(shape=(buflen,), dtype=np.float64)
    
    with open(args.src, 'rb') as stream:
        while True:
            crec = stream.read(CRec.size * buflen)

            if not crec: # we finished the file
                break
            
            n_crecs = extract_crecs(X_i, X_v, crec)
            
            write_to_npy(X_i, X_v, n_crecs, args.output)
