#!/usr/bin/env python3

#
# Based on dol2ipl.py from https://github.com/redolution/iplboot
#
# Original source by 9ary, bootrom descrambler reversed by segher
#

from datetime import datetime
import math
import struct
import sys

payload_padding = [
    0x81, 0x4a, 0xe6, 0xc8, 0x00, 0x04, 0xc5, 0x77, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
]

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

def bytes_to_c_array(data):
    p_list = [data[i:i + 4] for i in range(0, len(data), 4)]
    return ["0x%08x" % int.from_bytes(b, byteorder='big', signed=False) for b in p_list]

def generate_header_file(elements, executable, input_file, output_file, size):
    output = '#include <stdio.h>\n\n'
    output += '//\n'
    output += '// Command: {0} {1} {2}\n'.format(executable, input_file, output_file)
    output += '//\n'
    output += '// File: {0}, size: {1} bytes\n'.format(input_file, size)
    output += '//\n'
    output += '// File generated on {0}\n'.format(datetime.now().strftime("%d.%m.%Y %H:%M:%S"))
    output += '//\n\n'
    output += 'uint32_t __in_flash("ipl_data") ipl[]  = {\n\t'

    for num in range(len(elements)):
        if num > 0 and num % 4 == 0:
            output += '\n\t'
        
        output += elements[num]

        if num != len(elements):
            output += ', '

    output += '\n};\n'

    return output

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

    payload = bytearray(0x700) + bytearray(payload_padding)+ img
    payload_size = size + 0x20; # there are 32 bytes of additional padding needed 

    if payload_size % 1024 != 0:
        new_size_k = math.ceil(payload_size / 1024)
        new_size = new_size_k * 1024
        print(f"Payload needs to be aligned to {new_size_k}K")
        payload += bytearray(new_size - payload_size)
        payload_size = new_size

    print(f"Output binary size: {payload_size} bytes ({payload_size / 1024}K)")

    scrambled_payload = scramble(payload)[0x700:]
    payload_size = len(scrambled_payload);

    byte_groups = bytes_to_c_array(scrambled_payload)

    output = generate_header_file(byte_groups, sys.argv[0], sys.argv[1], sys.argv[2], size)

    with open(sys.argv[2], "w") as f:
        f.write(output)

if __name__ == "__main__":
    sys.exit(main())
