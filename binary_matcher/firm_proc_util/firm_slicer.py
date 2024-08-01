import os
import sys
import binascii

def b2hexstr(d):
    return binascii.hexlify(d).hex()

def largeZero(data, off, thres):
    hexdata = data[off:off+thres].hex()
    for i in range(0, len(hexdata)):
        if hexdata[i] != "f":
            return i & 0xFFFFFFFC

    return thres

def retZeroLen(data, off):
    orioff = off
    while True:
        hexdata = data[off:off+4].hex()
        if hexdata != "ffffffff":
            break
        off = off + 4
    return off - orioff

# if len(sys.argv) != 3:
#     print("Arg1: raw binary firmware file name")
#     print("Arg2: sliced binary firmware file name (of the newly generated file)")
#     sys.exit()

def slice_firmware(data: bytes):
    minoff2maxoff = {}
    thres = 30 # TODO is this value arbitrary? 
    off = 0
    minoff = 0
    while off + 4 < len(data):
        d = data[off:off+4].hex()
        if d == "ffffffff": #"00000000":
            l = largeZero(data, off, thres)
            if l == thres:
                zl = retZeroLen(data, off)
                minoff2maxoff[minoff] = off
                off = off + zl
                minoff = off
                #minoff2maxoff[off] = off + zl
                #off = off + zl
                continue
            else:
                off = off + l
                continue

        # idx update
        off = off + 4

    finalmin = len(data)
    finalmax = len(data)
    # choose the largest binoff. Typically, there are (1) bootloader and (2) main code. 
    # But, w/ non-trival program, the size of main code is much larger than the size of bootloader
    for k in minoff2maxoff:
        if minoff2maxoff[k] - k > finalmax - finalmin:
            finalmin = k
            finalmax = minoff2maxoff[k]

    # f = open(sys.argv[2], mode='wb')
    # data = f.write(data[finalmin:finalmax])
    # f.close()

    # print("Dumped minoff: " + hex(finalmin))
    # print("Dumped maxoff: " + hex(finalmax))
    return (finalmin, finalmax, data[finalmin:finalmax])
