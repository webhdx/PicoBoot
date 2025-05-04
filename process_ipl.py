#!/usr/bin/env python3

import math
import struct
import sys

# bootrom descrambler reversed by segher
def scramble(data, *, qoobsx=False):
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
    offsets = header[:18]
    addresses = header[18:36]
    sizes = header[36:54]
    bss_address = header[54]
    bss_size = header[55]
    entry = header[56]

    dol_min = min(a for a in addresses if a)
    dol_max = max(a + s for a, s in zip(addresses, sizes))

    img = bytearray(dol_max - dol_min)

    for offset, address, size in zip(offsets, addresses, sizes):
        img[address - dol_min:address + size - dol_min] = data[offset:offset + size]

    # Entry point, load address, memory image
    return entry, dol_min, img

def pack_uf2(data, base_address, family):
    if family == "rp2040":
        family_id = 0xE48BFF56 # RP2040
    elif family == "rp2350":
        family_id = 0xE48BFF58 # DATA family ID compatible with RP2350
    else:
        raise ValueError(f"Unknown family: {family}")

    ret = bytearray()

    seq = 0
    addr = base_address
    chunk_size = 256
    total_chunks = math.ceil(len(data) / chunk_size)
    while data:
        chunk = data[:chunk_size]
        data = data[chunk_size:]

        ret += struct.pack(
            "< 8I 476B I",
            0x0A324655, # Magic 1 "UF2\n"
            0x9E5D5157, # Magic 2
            0x00002000, # Flags (family ID present)
            addr,
            chunk_size,
            seq,
            total_chunks,
            family_id, # Board family: Raspberry Pi RP2040
            *chunk.ljust(476, b"\x00"),
            0x0AB16F30, # Final magic
        )

        seq += 1
        addr += chunk_size

    return ret

def main():
    if len(sys.argv) not in range(3, 4 + 1):
        print(f"Usage: {sys.argv[0]} <output> <executable> [<uf2_family>]")
        return 1

    output = sys.argv[1]
    executable = sys.argv[2]
    family = sys.argv[3]

    with open(executable, "rb") as f:
        exe = bytearray(f.read())

    if executable.endswith(".dol"):
        entry, load, img = flatten_dol(exe)
        entry &= 0x017FFFFF
        entry |= 0x80000000
        load &= 0x017FFFFF
        size = len(img)

        print(f"Entry point:   0x{entry:0{8}X}")
        print(f"Load address:  0x{load:0{8}X}")
        print(f"Image size:    {size} bytes ({size // 1024}K)")
    elif executable.endswith(".elf"):
        pass
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

    if output.endswith(".bin"):
        out = header + img

    elif output.endswith(".uf2"):
        out = pack_uf2(header + img, 0x10080000, family)

    else:
        print("Unknown output format")
        return -1

    with open(output, "wb") as f:
        f.write(out)

if __name__ == "__main__":
    sys.exit(main())
