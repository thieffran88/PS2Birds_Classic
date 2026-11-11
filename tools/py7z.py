#!/usr/bin/env python3
"""Stand-in for 7-Zip used by the extraction tools: `py7z.py x -so archive.7z`.
Requires the py7zr package (pip install py7zr); only reads archives, never executes game code."""
import sys,os,tempfile,py7zr
args=sys.argv[1:]
if len(args)<3 or args[0]!='x' or args[1]!='-so':sys.exit('usage: 7z x -so archive')
with tempfile.TemporaryDirectory(prefix='py7z-') as folder:
    with py7zr.SevenZipFile(args[2],'r') as z:
        names=[f.filename for f in z.list() if f.is_file];z.extractall(folder)
    for name in names:
        with open(os.path.join(folder,name),'rb') as f:sys.stdout.buffer.write(f.read())
