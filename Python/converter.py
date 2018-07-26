

def intToByteArray(i):
    i = i % 4294967296
    n4 = i % 256
    i = i / 256
    n3 = i % 256
    i = i / 256
    n2 = i % 256
    n1 = i / 256
    return [int(n1),int(n2),int(n3),int(n4)]


def byteArrayToInt(byteArray, index, bigEndian):
    result = 0
    if bigEndian == True:
        for i in range(index, index+4):
            result = result * 256 + int(byteArray[i])
    else:
        for i in range(index+3, index-1, -1):
            result = result * 256 + int(byteArray[i])
    return result

def byteArrayToShort(byteArray, index, bigEndian):
    result = 0
    if bigEndian == True:
         for i in range(index, index+2):
            result = result * 256 + int(byteArray[i])
    else:
        for i in range(index+1, index-1, -1):
            result = result * 256 + int(byteArray[i])

    return result