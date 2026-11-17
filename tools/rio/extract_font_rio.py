"""FONT.BIN for the Rio build, from the player's own APK, with the Classic's font extractor. The Rio has
no 1024x768 size set, so it reads the 864x480 one; the Classic scales its 1024x768 glyphs by 0.44 (HUD font)
and 0.32 (score font), and the same glyph size on 864 px wide originals is 0.44 x 1024/864 = 0.52 and 0.38.
usage: extract_font_rio.py APK OUT.BIN"""
import sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools'))
import extract_font
RATIO=1024/864
FONTS=[('FONT_BASIC',0.44*RATIO,extract_font.CHARSET),('FONT_SCORE',0.32*RATIO,None)]
def main(apk,out):
    extract_font.extract(apk,out,'864x480',FONTS);print('FONT.BIN do Rio:',Path(out).stat().st_size,'bytes')
if __name__=='__main__':main(*sys.argv[1:3])
