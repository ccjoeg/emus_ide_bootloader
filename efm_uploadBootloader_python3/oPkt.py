#!/usr/bin/python

"""
Original Code:
  Summary: XMODEM protocol implementation.
  Home-page: https://github.com/tehmaze/xmodem
  Author: Wijnand Modderman, Jeff Quast
  License: MIT

Stripped down and modified for this bootloader by Joe George 2015-2016
"""

from calcCRC import calc_crc

# Protocol bytes
SOH = b'\x01'

packet_size   = 128
pad           = b'\x1a'


def asmPkt(data, sequence):
    while len(data) < packet_size:
        data += bytearray(pad)

    crc = calc_crc(data)

    temp = SOH + bytes([sequence]) + bytes([0xff - sequence]) + data + bytes([crc >> 8]) + bytes([crc & 0xff])

    return temp

if __name__ == "__main__":
    import sys, binhex
    out = asmPkt('abcdefghijklmnopqrstuvwxyz0123456789', 1)
    for ch in out:
        sys.stdout.write(hex(ord(ch))+' ')
    print()
    print()


def verify(data, fullcrc):

    data = data.ljust(packet_size, pad)
    crc = calc_crc(data, fullcrc)

    return crc
