#!/usr/bin/python

"""
Original Code:
  Summary: XMODEM protocol implementation.
  Home-page: https://github.com/tehmaze/xmodem
  Author: Wijnand Modderman, Jeff Quast
  License: MIT

Stripped down and modified for this bootloader by Joe George 2015-2016
"""

import sys
from oPkt import asmPkt
from oPkt import verify
from tty import checkAck

packet_size   = 128
debug = 0

def debugPrint(s):
    if s.inWaiting() > 0:
        print(s.read().decode('utf-8'))
    return

def sendPackets(s, filename):

    with open(filename, 'rb') as stream:
        sequence = 1

        data = stream.read(packet_size)
        # data = "".join(chr(x) for x in data)
        # data = "".join(chr(x) for x in bytearray(data))
        data = bytearray(data)
        # print(data)
        # print(data[:10])
        # for b in data:
        #     print(b)
        print(type(data))
        if not data:
            print("Error: Empty Compiled Script File")
            return False 

        errcnt = 0
        fullcrc = 0
        
        print()
        
        while 1:
            pkt = asmPkt(data, sequence)
            print(type(pkt))
            print(pkt)
            fullcrc = verify(data, fullcrc)

            for ch in pkt:
                print(ch)
                # s.write(ch)
                s.write(chr(ch).encode())
                if debug: sys.stdout.write(hex(ord(ch))+' ')
            if debug:
                print()
                print()
            # s.write(pkt)

            if debug: debugPrint(s)

            print('checking for ACK..')

            if checkAck(s) == True:
                if debug:
                    print("checkAck -> True")
                    print(" %r" % sequence)
                else:
                    sys.stdout.write('.')
                    sys.stdout.flush()
                sequence = (sequence + 1) % 0x100  # increment sequence number
                if debug: print(sequence)
                # data = stream.read(packet_size)
                data = bytearray(stream.read(packet_size)) # TODO IT MAY HAVE BRICKED THE CHIP BC YOU WERE NOT CONVERTING TO BYTEARRAY
                if not data:
                    print()
                    print("CRC: " + hex(fullcrc))
                    return True
            else:
                print()
                print("Packet %r Failed to Upload" % sequence)
                errcnt += 1
                if errcnt > 0:
                    sys.stdout.flush()
                    return False

            continue

    print()
    return True

if __name__ == "__main__":
    import sys
    from sys import argv
    script, filename = argv
    debug = 1
    sendPackets(filename)


    fullcrc = calc_crc(data)
    print(hex(fullcrc))


