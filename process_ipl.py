#!/usr/bin/env python3

from datetime import datetime
import math
import struct
import sys

#
# Based on dol2ipl.py from https://github.com/redolution/gekkoboot
#
# Original source by novenary, bootrom scrambling logic by segher
#

def scramble(data):
    acc = 0
    nacc = 0

    t = 0x2953
    u = 0xD9C2
    v = 0x3FF1

    x = 1

    it = 0
    while it < len(data):
        t0 = t & 1
        t1 = (t >> 1) & 1
        u0 = u & 1
        u1 = (u >> 1) & 1
        v0 = v & 1

        x ^= t1 ^ v0
        x ^= u0 | u1
        x ^= (t0 ^ u1 ^ v0) & (t0 ^ u0)

        if t0 == u0:
            v >>= 1
            if v0:
                v ^= 0xB3D0

        if t0 == 0:
            u >>= 1
            if u0:
                u ^= 0xFB10

        t >>= 1
        if t0:
            t ^= 0xA740

        nacc = (nacc + 1) % 256
        acc = (acc * 2 + x) % 256
        if nacc == 8:
            data[it] ^= acc
            nacc = 0
            it += 1

    return data

def flatten_dol(data):
    header = struct.unpack(">64I", data[:256])

    dol_min = min(a for a in header[18:36] if a)
    dol_max = max(a + s for a, s in zip(header[18:36], header[36:54]))

    img = bytearray(dol_max - dol_min)

    for offset, address, length in zip(header[:18], header[18:36], header[36:54]):
        img[address - dol_min:address + length - dol_min] = data[offset:offset + length]

    # Entry point, load address, memory image
    return header[56], dol_min, img

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <executable> <output>")
        return -1

    with open(sys.argv[1], "rb") as f:
        exe = bytearray(f.read())

    if sys.argv[1].endswith(".dol"):
        entry, load, img = flatten_dol(exe)
        entry &= 0x017FFFFF
        entry |= 0x80000000
        load &= 0x017FFFFF
        size = len(img)

        print(f"Entry point:   0x{entry:0{8}X}")
        print(f"Load address:  0x{load:0{8}X}")
        print(f"Image size:    {size} bytes ({size // 1024}K)")
    else:
        print("Unknown input format")
        return -1

    if entry != 0x81300000 or load != 0x01300000:
        print("Invalid entry point and base address (must be 0x81300000)")
        return -1

    img = scramble(bytearray(0x720) + img)[0x720:]

    align_size = 4
    img = (
        img.ljust(math.ceil(len(img) / align_size) * align_size, b"\x00")
        + b"PICO"
    )

    header_size = 32
    header = struct.pack(
        "> 8B I 20x",
        *b"IPLBOOT ",
        len(img) + header_size,
    )

    assert len(header) == header_size

    out = header + img

    with open(sys.argv[2], "wb") as f:
        f.write(out)

if __name__ == "__main__":
    sys.exit(main())
