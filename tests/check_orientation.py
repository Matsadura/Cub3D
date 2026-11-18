#!/usr/bin/env python3
"""Asserts a screenshot (BMP, 32-bit) shows red on the left of the centre
row and blue on the right: wall textures must not be mirrored."""
import struct
import sys

data = open(sys.argv[1], "rb").read()
off = struct.unpack_from("<I", data, 10)[0]
w, h = struct.unpack_from("<ii", data, 18)
rows = abs(h)
y = rows // 2
row = y if h < 0 else rows - 1 - y
def px(x):
    b, g, r = data[off + (row * w + x) * 4: off + (row * w + x) * 4 + 3]
    return r, b
lr, lb = px(w // 2 - w // 8)
rr, rb = px(w // 2 + w // 8)
ok = lr > lb and rb > rr
print("left r=%d b=%d, right r=%d b=%d -> %s" % (lr, lb, rr, rb,
                                                "ok" if ok else "MIRRORED"))
sys.exit(0 if ok else 1)
