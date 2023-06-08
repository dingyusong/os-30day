#!/usr/bin/env python3

import sys


if len( sys.argv ) != 3:
    print('Usage: ', sys.argv[0], 'BinFileName ImgFileName')
    sys.exit()


infile = sys.argv[1]
outfile = sys.argv[2]

def makeimg(infile, outfile):
    with open( infile, 'rb' ) as fin , open( outfile, 'rb' ) as fin2:
        bufbin = fin.read(512)
        fin2.seek(512)
        bufbin2 = fin2.read(1474560 - 512)
        
    with open( outfile, 'wb' ) as fout:        
        fout.write(bufbin + bufbin2)

makeimg(infile, outfile)

# makeimg("./ipl.bin", "./floppy.img")
# makeimg("ipl.bin", "floppy.img")
# makeimg("/Users/dingyusong/mine/github/os-30day/projects/03_day/harib00d/mac/ipl.bin", "/Users/dingyusong/mine/github/os-30day/projects/03_day/harib00d/mac/floppy.img")