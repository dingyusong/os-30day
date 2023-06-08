#!/usr/bin/env python3

import sys
import os


if len( sys.argv ) != 5:
    print('error !Usage: ', sys.argv[0], 'BinFileName ImgFileName')
    sys.exit()


def write_to_img(boot_file, sys_file, dmg_file, sys_start):
    with open( boot_file, 'rb' ) as bin , open(sys_file, 'rb' ) as sin, open(dmg_file, 'rb' ) as din:

        boot_file_size = os.path.getsize(boot_file)
        print("boot_file Size is :", boot_file_size, "bytes") # 512 bytes
        if boot_file_size != 512:
            print("error! boot_file Size is not 512 bytes")
            sys.exit()

        sys_file_size = os.path.getsize(sys_file)
        print("sys_file Size is :", sys_file_size, "bytes")

        dmg_file_size = os.path.getsize(dmg_file)
        print("dmg_file Size is :", dmg_file_size, "bytes") # 1474560 bytes
        if dmg_file_size != 1474560:
            print("error! dmg_file Size is not 1474560 bytes")
            sys.exit()

        boot_buf = bin.read(boot_file_size)
        sys_buf = sin.read(sys_file_size)
        dmg_buf = din.read(dmg_file_size)

        out_buf = boot_buf + dmg_buf[boot_file_size:0x004200] + sys_buf + dmg_buf[0x004200+sys_file_size:]
        print("out_buf Size is :", len(out_buf), "bytes", len(sys_buf), len(dmg_buf)) # 1474560 bytes
        print("boot_file_size Size is :", len(dmg_buf[boot_file_size:0x004200]), "sectors") # 2880 sectors
    with open(dmg_file, 'wb' ) as fout:        
        fout.write(out_buf)

write_to_img(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
