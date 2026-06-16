#!/usr/bin/env python3
"""Convert a P3 PPM file to PNG using only the Python standard library."""

import struct
import sys
import zlib
from pathlib import Path


def read_ppm(path: Path):
    lines = path.read_text().splitlines()
    if not lines or lines[0].strip() != "P3":
        raise ValueError(f"Expected P3 PPM, got {lines[0] if lines else 'empty file'}")

    width, height = map(int, lines[1].split())
    max_val = int(lines[2].strip())
    if max_val != 255:
        raise ValueError(f"Only 8-bit PPM is supported (max value {max_val})")

    pixels = []
    for line in lines[3:]:
        if not line.strip():
            continue
        r, g, b = map(int, line.split())
        pixels.append(bytes((r, g, b)))

    expected = width * height
    if len(pixels) != expected:
        raise ValueError(f"Expected {expected} pixels, found {len(pixels)}")

    return width, height, pixels


def write_png(path: Path, width: int, height: int, pixels):
    raw = b"".join(
        b"\x00" + b"".join(pixels[y * width : (y + 1) * width]) for y in range(height)
    )

    def chunk(tag: bytes, data: bytes) -> bytes:
        return (
            struct.pack(">I", len(data))
            + tag
            + data
            + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)
        )

    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
    png += chunk(b"IDAT", zlib.compress(raw, 9))
    png += chunk(b"IEND", b"")
    path.write_bytes(png)


def main(argv):
    if len(argv) != 2:
        print(f"Usage: {argv[0]} <input.ppm>", file=sys.stderr)
        return 1

    ppm_path = Path(argv[1])
    png_path = ppm_path.with_suffix(".png")
    width, height, pixels = read_ppm(ppm_path)
    write_png(png_path, width, height, pixels)
    print(png_path)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
