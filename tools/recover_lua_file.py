#!/usr/bin/env python3
"""Recover one script from a compatible PC/mobile release without executing it."""
import argparse,shutil
from pathlib import Path
from game_profiles import PROFILES
from inspect_apk import recover_lua

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path);parser.add_argument('output',type=Path)
    parser.add_argument('--game',choices=sorted(PROFILES),required=True)
    parser.add_argument('--sevenzip',default=shutil.which('7z') or shutil.which('7zz'))
    args=parser.parse_args()
    if args.source.resolve()==args.output.resolve():parser.error('Output must differ from source')
    payload=recover_lua(args.source.read_bytes(),args.sevenzip,args.game)
    args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_bytes(payload)
    print(f'Recovered Lua chunk: {args.output}; this does not establish runtime compatibility')
if __name__=='__main__':main()
