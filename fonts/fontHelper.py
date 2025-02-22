with open("convert.txt", 'r') as file:
    for line in file:
        linesplit = line.strip().split()
        ibits = linesplit[0][:-1]
        ibits = ibits.ljust(18, '0')
        
        obits = ibits[:4] + \
                ', 0x' + ibits[4:6] + \
                ', 0x' + ibits[6:8] + \
                ', 0x' + ibits[8:10] + \
                ', 0x' + ibits[10:12] + \
                ', 0x' + ibits[12:14] + \
                ', 0x' + ibits[14:16] + \
                ', 0x' + ibits[16:18] + \
                ', ' + " ".join(linesplit[-3:])
        print(obits)
