import serial

ser = serial.Serial('/dev/tty.usbserial-A9015RB1', 115200)
#ser = serial.Serial('/dev/ttyUSB0', 115200)



"""
    array is an array of 3 tuples (R, G, B)
"""
def write(array):
    towrite = ""
    for LED, (R,G,B) in enumerate(array):
        R = roundClamp(R)
        G = roundClamp(G)
        B = roundClamp(B)
        towrite += "%d %d %d %dn"%(LED, R, G, B)
    towrite += "W"
    ser.write(towrite)
    print "writing: "+towrite

def clear():
    ser.write('C')

"""
    colors go from 0 to 1023
"""
def writeAll(R,G,B):
    R = roundClamp(R)
    G = roundClamp(G)
    B = roundClamp(B)
    write([(R,G,B)]*2)

def roundClamp(value):
    return min(1023, max(0, round(value)))


if __name__ == "__main__":
    import sys
    if len(sys.argv) == 2 and sys.argv[1] == "clear":
        clear()
    elif len(sys.argv) == 4:
        writeAll(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
