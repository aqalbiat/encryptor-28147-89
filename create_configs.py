import random

X = []
K = []

signature = random.randint(0, 2 ** 64 - 1)

C1_bin_str = "000000010000000" + "100000001" + "00000100"
C2_bin_str = "000000010000000" + "100000001" + "00000001"

C1 = 0
C2 = 0

def from_bin(nbr_str):
    res = 0
    for x in nbr_str:
        res = 2 * res + int(x)
    return res

C1 = from_bin(C1_bin_str)
C2 = from_bin(C2_bin_str)

for i in range(8):
    X.append(random.randint(0, 2 ** 32 - 1))

for i in range(8):
    K.append(random.randint(0, 2 ** 64 - 1))

def to_hex_64bit(nbr):
    D = "0123456789abcdef"
    res = ""
    nbr = int(nbr)
    for i in range(16):
        res += D[nbr % 16]
        nbr //= 16
    res = res[::-1]
    return res

def to_hex_32bit(nbr):
    D = "0123456789abcdef"
    res = ""
    nbr = int(nbr)
    for i in range(8):
        res += D[nbr % 16]
        nbr //= 16
    res = res[::-1]
    return res

output = []

for i in range(0, 8, 2):
    output.append(to_hex_32bit(X[i]) + to_hex_32bit(X[i + 1]))

for i in range(8):
    output.append(to_hex_64bit(K[i]))

output.append(to_hex_64bit(signature))

output.append(to_hex_32bit(C1) + to_hex_32bit(C2))

for token in output:
    print(token)
