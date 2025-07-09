import sys
import os
import random

if len(sys.argv) != 2:
    print("Exactly one argument needed")
    print("It must be number of lines to be generated")
    os._exit(os.EX_USAGE)    

n = int(sys.argv[1])

def to_hex_64bit(nbr):
    D = "0123456789abcdef"
    res = ""
    nbr = int(nbr)
    for i in range(16):
        res += D[nbr % 16]
        nbr //= 16
    res = res[::-1]
    return res

for _ in range(n):
    print(to_hex_64bit(random.randint(0, 2 ** 64 - 1)))
