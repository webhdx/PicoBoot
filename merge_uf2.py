#!/usr/bin/env python3

from dataclasses import dataclass
import itertools
import math
import struct
import sys

block_format = "< 8I 476B I"
magic0 = 0x0A324655 # "UF2\n"
magic1 = 0x9E5D5157
magic2 = 0x0AB16F30

@dataclass
class Uf2Block:
    # Fields are ordered as they appear in the serialized binary
    # DO NOT CHANGE
    flags: int
    address: int
    size: int
    seq: int
    total_blocks: int
    family_id: int
    data: bytes = bytes()

def read_file(name):
    with open(name, "rb") as f:
        data = f.read()
    return data

def decode_uf2(data):
    while data:
        block, data = data[:512], data[512:]
        block = struct.unpack(block_format, block)

        header, body, footer = block[:8], block[8:-1], block[-1:]
        assert (header[:2], footer) == ((magic0, magic1), (magic2,))

        block = Uf2Block(*header[2:])
        block.data = bytes(body[:block.size])

        yield block

def write_uf2(blocks, name):
    with open(name, "wb") as f:
        for seq, b in enumerate(blocks):
            data = struct.pack(
                block_format,
                magic0,
                magic1,
                b.flags,
                b.address,
                b.size,
                seq,
                len(blocks),
                b.family_id,
                *b.data.ljust(476, b"\x00"),
                magic2,
            )
            f.write(data)

def padding_block(flags, address, seq, total_blocks, family_id):
    return Uf2Block(
        flags,
        address,
        256,
        seq,
        total_blocks,
        family_id,
        bytes(476)
    )

def main():
    output = sys.argv[1]
    inputs = sys.argv[2:]

    inputs = (read_file(f) for f in inputs)
    blocks = []
    for f in inputs:
        file_blocks = list(decode_uf2(f))
        first_block = file_blocks[0]
        last_block = file_blocks[-1]
        blocks += file_blocks
        total_size = (last_block.address + last_block.size) - first_block.address

        if total_size % 4096 != 0:
            new_size = (math.ceil(total_size / 4096) + 1) * 4096
            padding_size = new_size - total_size
            padding_blocks = int(padding_size / 256)
            total_blocks = last_block.total_blocks + padding_blocks
            
            for index in range(padding_blocks):
                seq = index + 1
                blocks.append(
                    padding_block(
                        first_block.flags,
                        last_block.address + (seq * 256),
                        last_block.seq + seq,
                        total_blocks,
                        last_block.family_id
                    )
                )

    ref = blocks[0]
    for b in blocks:
        assert b.flags == ref.flags
        assert b.size <= ref.size
        assert b.family_id == ref.family_id

    blocks.sort(key=lambda b: b.address)
    for a, b in itertools.pairwise(blocks):
        # Ensure no blocks overlap
        assert a.address + a.size <= b.address

    write_uf2(blocks, output)

if __name__ == "__main__":
    sys.exit(main())
