# -*- coding: utf-8 -*-
# qrt@qland.de

import os
Import("env")

print(">>> pre_script")

file = env.GetProjectOption("board_build.pio").strip()

if not os.path.isfile(file):
    print(file + " does not exist")
    exit(1)

(mode, ino, dev, nlink, uid, gid, size, atime, mtime_p, ctime) = os.stat(file)
    
if os.path.isfile(file + ".h"):
    (mode, ino, dev, nlink, uid, gid, size, atime, mtime_h, ctime) = os.stat(file + ".h")

    if mtime_p != mtime_h:
        print(">>> force assembly of pio file")
        os.remove(file + ".h")
