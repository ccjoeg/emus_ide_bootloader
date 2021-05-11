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
import serial
import time
import os
# import StringIO

from calcCRC import calc_crc
from getArgs import getArgs2
from tty import wait_until, wait_until_checksum
from fRead import sendPackets


"""
XMODEM 128 byte blocks

    SENDER                                 RECEIVER

                                            <-- C
    SOH 01 FE Data[128] CRC CRC            -->
                                            <-- ACK
    SOH 02 FD Data[128] CRC CRC            -->
                                            <-- ACK
    SOH 03 FC Data[128] CRC CRC             -->
                                            <-- ACK
    SOH 04 FB Data[100] CPMEOF[28] CRC CRC  -->
                                            <-- ACK
    EOT                                     -->
                                            <-- ACK
"""

# Protocol bytes
SOH = b'\x01'
STX = b'\x02'
EOT = b'\x04'
ACK = b'\x06'
DLE = b'\x10'
NAK = b'\x15'
CAN = b'\x18'
CRC = b'\x43' # C

timeout = 10

args = getArgs2()  # get filename and serial port
print("Uploading {0} to {1}".format(args[0], args[2]))
sys.stdout.flush()

try:
    s = serial.Serial(port= args[2],
                  baudrate=115200, 
                  bytesize=8,
                  parity='N',
                  stopbits=1,
                  timeout=None,
                  xonxoff=0,
                  rtscts=0)
except serial.serialutil.SerialException as err:
    print(f"Error opening serial port {args[2]}: {err}")
    print("Double check the serial port is available and retry.")
    print("Upload Failed")
    sys.exit(-1)

try:
    
    if(s.isOpen() == False):
        s.open()
        print(args[2] + " is open already")
        sys.exit(-1)

    print("Reset Device")
    sys.stdout.flush()

    print("Press reset button within next 10 sec")

    for i in range(1, 11):
        time.sleep(1)
        print(i)

    s.write('U'.encode())
    time.sleep(1)

    # loop waiting for question mark, send r and ' ' again after timeout
    print("Device Reset Check")
    sys.stdout.flush()

    cnt = timeout
    while True:
        s.write(' '.encode())
        if(wait_until(s, '?', 1) == True):
            break
        else:
            cnt = cnt -1
            if cnt == 0:
                print("Timed out checking for device reset.")
                s.close()
                sys.exit(-1)

    print("\nSending upload command: u")
    sys.stdout.flush()
    
    # send upload command
    s.write('u'.encode())
    
    #reset timeout
    mustend = time.time() + timeout
    
    # timeout if C doesn't come
    ch = 0
    while ((ch != 'C') and (time.time() < mustend)):
        ch = s.read().decode('utf-8')
    
    if(ch == 'C'):
        print("Upload Starting")
        sys.stdout.flush()
    else:
        print("Upload Failed to Start")
        s.close()
        sys.exit(-1)

    #hide sendPackets print outs from the end user
    if(sendPackets(s, args[0])):
        print("Upload Finished")
        sys.stdout.flush()
        s.write(EOT)        # end of transmission
    else:
        # sys.stdout = stdout
        print("Error Uploading Bootloader")
        print("Upload Failed")
        s.close()
        sys.exit(-1)

    print("\nSending checksum command: c")
    # s.write('c'.encode())
    time.sleep(1)
    sys.stdout.flush()

    cnt = timeout
    while True:
        s.write('c'.encode())
        if(wait_until_checksum(s, '000056B9', 1) == True):
            break
        else:
            cnt = cnt -1
            if cnt == 0:
                print("Timed out checking for correct checksum.")
                s.close()
                sys.exit(-1)

    #wait until ? is sent or 1 second, this allows the board enough time to finish the upload before we send the b
    wait_until(s, '?', 1) 
    wait_until(s, '?', 1)
    wait_until(s, '?', 1)

    print("\nSending boot command: b")
    sys.stdout.flush()
    s.write('b'.encode())
    time.sleep(.2)

    #wait until ? is sent or 1 second, this allows the board enough time to finish the upload before we send the b
    wait_until(s, '?', 1)

    print("\nSending break, space, break")
    sys.stdout.flush()
    s.send_break(duration=.25)
    time.sleep(1)
    s.write(' '.encode())
    time.sleep(1)
    s.send_break(duration=.25)
    time.sleep(1)

    #wait until ? is sent or 1 second, this allows the board enough time to finish the upload before we send the b
    wait_until(s, '?', 1) 

    print("\nUploading {0} to {1}".format(args[1], args[2]))
    print("Sending upload command: u")
    sys.stdout.flush()
    s.write('u'.encode())
    
    #reset timeout
    mustend = time.time() + timeout
    
    # timeout if C doesn't come
    ch = 0
    while ((ch != 'C') and (time.time() < mustend)):
        ch = s.read().decode('utf-8')
    
    if(ch == 'C'):
        print("Upload starting")
        sys.stdout.flush()
    else:
        print("Upload failed to start")
        s.close()
        sys.exit(-1)

    #hide sendPackets print outs from the end user
    if(sendPackets(s, args[1])):
        print("Upload finished")
        sys.stdout.flush()
        s.write(EOT)        # end of transmission
    else:
        # sys.stdout = stdout
        print("Error uploading test app")
        print("Upload Failed")
        s.close()
        sys.exit(-1)

    s.close()
    print("Serial connection closed")
    
except serial.serialutil.SerialException as err:
    print(f"Serial port {args[1]} encountered an error:" , err)
    print("Double check the serial port is available and retry.")
    print("Upload Failed")
    sys.exit(-1)