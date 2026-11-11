#!/usr/bin/env python3
"""Host helper (not part of the ELF): multi-bird beam search over tests/solver3.

Usage: beam.py LEVEL [WIDTH]. Keeps the WIDTH best prefixes after each bird and
prints the first winning sequence as 'WIN angle power special ...'.
"""
import subprocess,sys
from pathlib import Path
level=sys.argv[1];width=int(sys.argv[2]) if len(sys.argv)>2 else 3
solver=str(Path(__file__).resolve().parents[1]/'build/solver3')
beam=[[]]
for depth in range(8):
    candidates=[]
    for prefix in beam:
        args=[solver,level]+[str(v) for shot in prefix for v in shot]
        for line in subprocess.run(args,capture_output=True,text=True,check=True).stdout.split('\n'):
            if not line.strip():continue
            value,a,p,s=line.split();shot=(int(float(a)),int(float(p)),float(s))
            if float(value)>=1e12:
                print('WIN',' '.join(f'{x[0]} {x[1]} {x[2]}' for x in prefix+[shot]),int(float(value)-1e12));sys.exit(0)
            candidates.append((float(value),prefix+[shot]))
    if not candidates:break
    candidates.sort(key=lambda c:-c[0]);seen=[]
    for value,seq in candidates:
        if seq not in seen:seen.append(seq)
        if len(seen)==width:break
    beam=seen;print('depth',depth,'best',int(candidates[0][0]),candidates[0][1],flush=True)
print('none')
