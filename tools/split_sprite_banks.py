#!/usr/bin/env python3
"""Split ps2/assets/sprites.rgba for the PS2 build.

Bank 0 (the Poached Eggs pages) stays inside the ELF so the game always boots
with textures; banks 1+ go to SPRITES.BIN, read from the disc or from the ELF's
folder when an episode needs them. Byte offsets inside SPRITES.BIN are the atlas
offsets of sprite_data.h minus the size of bank 0.
"""
import argparse,json
from pathlib import Path
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('manifest',type=Path);p.add_argument('pixels',type=Path);p.add_argument('bank0',type=Path);p.add_argument('rest',type=Path)
a=p.parse_args()
meta=json.loads(a.manifest.read_text());data=a.pixels.read_bytes()
pages=[pg for b in meta['banks'] for pg in b['pages']]
bank0=sum(pg['width']*pg['height']*4 for pg in meta['banks'][0]['pages'])
if meta['banks'][0]['pages'][0]['offset']!=0 or len(data)!=sum(pg['width']*pg['height']*4 for pg in pages):raise SystemExit('Unexpected atlas layout')
if len(meta['banks'])>1 and meta['banks'][1]['pages'][0]['offset']!=bank0:raise SystemExit('Bank 1 must follow bank 0')
a.bank0.parent.mkdir(parents=True,exist_ok=True);a.bank0.write_bytes(data[:bank0]);a.rest.write_bytes(data[bank0:])
print(f'bank 0: {bank0} bytes in the ELF; SPRITES.BIN: {len(data)-bank0} bytes')
