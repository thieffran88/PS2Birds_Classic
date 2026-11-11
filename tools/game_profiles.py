"""Shared, explicit format profiles; declared keys are not runtime compatibility."""
import json,lzma,subprocess,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
PROFILES=json.loads((ROOT/'profiles/games.json').read_text())
ROVIO_LZMA=b'\x89LZMA\r\n\x1a\n'

def decode_payload(data,game='classic',sevenzip=None):
    if game not in PROFILES:raise ValueError('Unknown game profile: '+game)
    if data.startswith(b'\x1bLua'):return data
    if data.lstrip().startswith(b'{'):
        try:
            parsed=json.loads(data.decode('utf-8'))
        except (UnicodeDecodeError,ValueError):pass
        else:
            if isinstance(parsed,dict):return data
    if not data.startswith((ROVIO_LZMA,b'7z\xbc\xaf\x27\x1c')):
        result=subprocess.run(['openssl','enc','-aes-256-cbc','-d','-K',PROFILES[game]['aes_key_hex'],'-iv','0'*32],input=data,capture_output=True,check=False)
        if result.returncode:raise ValueError('AES failed: wrong profile, unsupported version or damaged input')
        data=result.stdout
    if data.startswith(ROVIO_LZMA):data=lzma.decompress(data[len(ROVIO_LZMA):],format=lzma.FORMAT_ALONE)
    if data.startswith(b'7z\xbc\xaf\x27\x1c'):
        if not sevenzip:raise ValueError('Standard 7z payload requires --sevenzip')
        with tempfile.TemporaryDirectory(prefix='ps2birds-payload-') as folder:
            archive=Path(folder)/'payload.7z';archive.write_bytes(data)
            result=subprocess.run([sevenzip,'x','-so',str(archive)],capture_output=True,check=False)
            if result.returncode:raise ValueError('7-Zip extraction failed')
            data=result.stdout
    return data
