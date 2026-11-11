import hashlib,json,math,struct
from pathlib import Path
root=Path(__file__).resolve().parents[1]/'assets'
meta=json.loads((root/'audio_manifest.json').read_text());bank=(root/'sounds.adp').read_bytes()
assert len(meta['sounds'])==20 and len(bank)==meta['bytes']<=meta['spu_budget']==1048576
assert hashlib.sha256(bank).hexdigest()==meta['bank_sha256']
previous=0
for s in meta['sounds']:
 assert s['offset']%64==0 and s['offset']>=previous and s['channel'] in range(meta['voice_channels'])
 data=bank[s['offset']:s['offset']+s['bytes']];previous=s['offset']+s['bytes']
 assert len(data)==s['bytes'] and len(data)%16==0 and data[:4]==b'APCM'
 assert data[5]==1 and data[6]==0 and 1800<struct.unpack_from('<I',data,8)[0]<2000
 assert hashlib.sha256(data).hexdigest()==s['adpcm_sha256']
 # Independent SPU2 ADPCM decoder: test the actual encoded blocks contain a finite, non-silent signal.
 h1=h2=0;samples=[];coefficients=[(0,0),(60,0),(115,-52),(98,-55),(122,-60)]
 for offset in range(16,len(data),16):
  block=data[offset:offset+16];shift=block[0]&15;filter_=block[0]>>4
  assert shift<=12 and filter_<5
  a,b=coefficients[filter_]
  for value in block[2:]:
   for nibble in [value&15,value>>4]:
    signed=nibble if nibble<8 else nibble-16
    value_=max(-32768,min(32767,((signed<<12)>>shift)+((h1*a+h2*b+32)>>6)))
    h2=h1;h1=value_;samples.append(value_)
 assert len(samples)>=s['frames'] and max(map(abs,samples))>100
 assert math.sqrt(sum(x*x for x in samples)/len(samples))>10
print('Audio assets OK: 20 original effects, hashes/alignment/APCM headers, mono/no loop, independent ADPCM signal decode, SPU budget.')

by_name={s["id"]:s for s in meta["sounds"]}
assert meta["voice_channels"]==10 and by_name["DROP_EGG"]["channel"]!=by_name["EXPLOSION"]["channel"]
