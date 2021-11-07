#!/usr/bin/env python

import argparse
import re


SYMBOLS_TABLE = []


def load_symbols_table(symbols):
    with open(symbols, "r") as symbols_file:
        for line in symbols_file.readlines():
            [addr, name] = line.split(" ")
            SYMBOLS_TABLE.append([int(addr, 16), name.strip()])


def find_symbol(addr):
    left = -1
    right = len(SYMBOLS_TABLE)

    while right - left > 1:
        mid = (left + right) // 2

        if addr >= SYMBOLS_TABLE[mid][0]:
            left = mid
        else:
            right = mid

    return SYMBOLS_TABLE[left] if left >= 0 else [addr, "(unknown)"]


def fix_stacktrace():
    parser = argparse.ArgumentParser()
    parser.add_argument("symbols")
    parser.add_argument("logfile")
    args = parser.parse_args()

    load_symbols_table(args.symbols)

    with open(args.logfile, "r") as logfile:
        stacktrace = re.compile(".+kernel_dump_stacktrace.+([0-9A-Z]{16}).+\\?\\?\\?")
        offset = len(" - ???+0x0\n")

        for line in logfile.readlines():
            matches = stacktrace.match(line)

            if matches:
                addr = int(matches.group(1), 16)
                [symbol_addr, name] = find_symbol(addr)
                addr_offset = '+0x{:x}'.format(addr - symbol_addr)
                print(f"{line[:-offset]} - {name}{addr_offset}")
            else:
                print(line, end="")


if __name__ == "__main__":
    fix_stacktrace()
