#!/usr/bin/env python3
"""Bounded regression: StreamViX TV event IDs must survive every source lookup stage.

StreamViX Live.py: event ID is slug[:60] + '-' + YYYYMMDD; catalog adds 'tv:'.
This test checks the fixed-width buffers in the Guide -> player -> addon ->
source-cache pipeline. It intentionally does not contact a third-party stream.
"""
from pathlib import Path
import re

MAX_STREAMVIX_ID_LEN = len("tv:") + 60 + len("-") + len("20261015")
assert MAX_STREAMVIX_ID_LEN == 72
MIN_CAPACITY = MAX_STREAMVIX_ID_LEN + 1

checks = {
    "src/guia.c": [r"char\s+id\[(\d+)\];"],
    "src/catalogo.h": [r"char\s+imdb\[(\d+)\];"],
    "src/addons.c": [r"static\s+char\s+alvoId\[(\d+)\]",
                     r"static\s+char\s+pendId\[(\d+)\]"],
    "src/streams.c": [r"static\s+char\s+alvoPedido\[(\d+)\]",
                      r"alvoLista\[(\d+)\]"],
    "src/fontecache.c": [r"char\s+id\[(\d+)\]",
                         r"static\s+char\s+pend\[2\]\[(\d+)\]",
                         r"static\s+char\s+emCurso\[(\d+)\]"],
}
for path, patterns in checks.items():
    content = Path(path).read_text(encoding="utf-8")
    for pat in patterns:
        match = re.search(pat, content)
        if not match:
            raise SystemExit(f"FAIL: {path}: no field matching {pat}")
        actual = int(match.group(1))
        if actual < MIN_CAPACITY:
            raise SystemExit(f"FAIL: {path}: buffer {pat} is {actual}, needs {MIN_CAPACITY}")
        print(f"PASS: {path}: {pat} => {actual} >= {MIN_CAPACITY}")
print("PASS: StreamViX 72-byte event ID fits guide/player/addon/cache pipeline")
