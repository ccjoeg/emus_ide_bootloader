#!/usr/bin/python

"""
Original Code:
  Summary: XMODEM protocol implementation.
  Home-page: https://github.com/tehmaze/xmodem
  Author: Wijnand Modderman, Jeff Quast
  License: MIT

Stripped down and modified for this bootloader by Joe George 2015-2016
"""

import time
import sys

def wait_until(s, ch, timeout):
    mustend = time.time() + timeout
    while time.time() < mustend:
        if s.inWaiting() > 0:
            c = s.read().decode('utf-8')
            # print(c)
            sys.stdout.write(c)
            if (c == ch):
                return True
                sys.stdout.write(ch)
                c = 0
                return False


def checkAck(s):
    n = [0 for i in range(64)]
    # n = []
    i = 0
    # ACK = chr(0x06)
    ACK = b'\x06'
    # print(ACK)
    # NAK = chr(0x21)
    NAK = b'\x21'
    # mustend = time.time() + 2
    mustend = time.time() + 10
    while time.time() < mustend:
        if s.inWaiting() > 0:
            # print('reading from serial within checkAck()')
            ch = s.read()
            # print(type(ch))
            if (ch == ACK):
                return True
            if (ch == NAK):
                print("NAK received")
                sys.stdout.write(ch)
                return False
            if (ch == '@'):
                print("CRC Error received")
                sys.stdout.write(ch)
                return False
            if (ch == '^'):
                print("First character is not Start of Header")
                sys.stdout.write(ch)
                return False
            if (ch == '!'):
                print("Wrong number of bytes in packet")
                sys.stdout.write(ch)
                return False
            else:
                print('weird response: ', ch)
                n[i] = ch
                # n.append(ch)
                i = i + 1
    print("Timeout waiting for ACK")
    print(n)
    return False
