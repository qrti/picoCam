# -*- coding: utf-8 -*-
# qrt@qland.de

import os
Import("env")

print(">>> post_script")

file = env.GetProjectOption("board_build.pio").strip()

if not os.path.isfile(file):
    print(file + " does not exist")
    exit(1)

(mode, ino, dev, nlink, uid, gid, size, atime, mtime_p, ctime) = os.stat(file)
(mode, ino, dev, nlink, uid, gid, size, atime, mtime_h, ctime) = os.stat(file + ".h")

if mtime_p != mtime_h:
    print(">>> modify date of pio file adapted")
    os.utime(file + ".h", (mtime_p, mtime_p))
