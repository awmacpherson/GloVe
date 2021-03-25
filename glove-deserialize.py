import sys, os
import numpy as np
import struct

INT_LEN = 4
DOUBLE_LEN = 8
CREC_LEN = 2*INT_LEN + DOUBLE_LEN
BO = '@' # native

CRec = struct.Struct('@iid')

if __name__ == '__main__'
    src = sys.argv[1]
    data = []
    with open(src, 'rb') as stream:
        while True:
            crec = stream.read(CRec.size)
            if not crec:
                break
            data.append(CRec.unpack(crec))

