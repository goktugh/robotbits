import struct

FORMAT = '>bhhhhB'

with open('telemetry.bin', 'rb') as f:
    while True:
        packet = f.read(10)
        bits = struct.unpack(FORMAT, packet)
        print("temp={} voltage={} current={} rpm={}"
            .format(bits[0], bits[1], bits[2], bits[4]))
