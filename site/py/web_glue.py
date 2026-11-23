"""Roda o gerador dentro do Pyodide (navegador ou Node): sem processos, sem ffmpeg, sem subprocess.

O gerador chama o ffmpeg (sons e músicas) e, no Rio, scripts de análise por subprocess. No Pyodide não existe subprocess, então este módulo troca
`subprocess.run` por uma versão que:
  - converte áudio procurando o resultado numa tabela preparada antes pelo JavaScript (ffmpeg.wasm): a conversão de áudio é assíncrona no navegador
    e o gerador é síncrono. Por isso a geração tem duas passadas pelo áudio: `plan_audio` só anota quais arquivos o gerador vai converter e `generate`
    usa as conversões prontas;
  - roda `python script.py` do Rio dentro do mesmo processo (runpy).
Nada do jogo original sai do aparelho: o APK é lido da memória do navegador e a ISO é montada na memória.
"""
import contextlib,hashlib,io,os,runpy,subprocess,sys,tempfile,wave,zipfile
from pathlib import Path

MODE='real'        # 'plan': anota os pedidos de conversão e devolve silêncio; 'real': usa CONVERTED
PLAN=[];CONVERTED={};SHA2PATH={}

def index_audio(apk):
    """sha1 do conteúdo -> caminho dentro do APK, para os arquivos de áudio."""
    SHA2PATH.clear()
    with zipfile.ZipFile(apk) as z:
        for n in z.namelist():
            if '/audio/' in n and n.endswith(('.ogg','.mp3')):SHA2PATH[hashlib.sha1(z.read(n)).hexdigest()]=n
    return len(SHA2PATH)

def _ffmpeg(cmd):
    args=[str(a) for a in cmd[1:]]
    inp=Path(args[args.index('-i')+1]);rate=int(args[args.index('-ar')+1]);out=Path(args[-1])
    raw='-f' in args and args[args.index('-f')+1]=='s16le'
    sha=hashlib.sha1(inp.read_bytes()).hexdigest();path=SHA2PATH[sha]
    if MODE=='plan':
        PLAN.append({'path':path,'rate':rate});pcm=bytes(400)
    else:pcm=bytes(CONVERTED[(path,rate)])
    if raw:out.write_bytes(pcm)
    else:
        with wave.open(str(out),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes(pcm)
    return subprocess.CompletedProcess(cmd,0,b'',b'')

def _script(cmd,kw):
    script=str(cmd[1]);env=kw.get('env') or {}
    old=dict(os.environ);os.environ.update({k:str(v) for k,v in env.items()})
    out=io.StringIO();err=io.StringIO();old_argv=sys.argv;sys.argv=[script]+[str(a) for a in cmd[2:]];code=0
    try:
        with contextlib.redirect_stdout(out),contextlib.redirect_stderr(err):
            try:runpy.run_path(script,run_name='__main__')
            except SystemExit as e:code=e.code if isinstance(e.code,int) else 0
            except Exception as e:code=1;print(type(e).__name__,e,file=sys.stderr)
    finally:
        sys.argv=old_argv;os.environ.clear();os.environ.update(old)
    text=kw.get('text')
    return subprocess.CompletedProcess(cmd,code,out.getvalue() if text else out.getvalue().encode(),err.getvalue() if text else err.getvalue().encode())

def fake_run(cmd,**kw):
    first=str(cmd[0])
    if os.path.basename(first).startswith('ffmpeg'):res=_ffmpeg(cmd)
    elif first==sys.executable:res=_script(cmd,kw)
    else:raise OSError('programa externo não disponível no navegador: '+first)
    if kw.get('check') and res.returncode:raise subprocess.CalledProcessError(res.returncode,cmd,res.stdout,res.stderr)
    return res

def install(root):
    """root: pasta que contém tools/, profiles/ e ps2/src/rio (o 'repositório' montado na memória)."""
    subprocess.run=fake_run
    os.environ['PS2BIRDS_PROCESSOS']='1'
    sys.path.insert(0,str(Path(root)/'tools'));sys.path.insert(0,str(Path(root)/'tools/rio'))

def plan_audio(game,apk):
    """Quais arquivos de áudio o gerador vai converter (caminho no APK e taxa): lista para o JavaScript."""
    global MODE
    MODE='plan';PLAN.clear();index_audio(apk)
    with tempfile.TemporaryDirectory() as d:
        d=Path(d)
        if game=='rio':
            import extract_audio_rio,extract_audio_rio_music
            extract_audio_rio.main(apk,d,'ffmpeg');extract_audio_rio_music.main(apk,d,'ffmpeg')
        else:
            import extract_audio
            extract_audio.extract(apk,d,'ffmpeg',None)
    MODE='real'
    seen=set();out=[]
    for j in PLAN:
        k=(j['path'],j['rate'])
        if k not in seen:seen.add(k);out.append(j)
    return out

def set_converted(items):
    """items: lista de (caminho, taxa, bytes PCM s16le mono)."""
    CONVERTED.clear()
    for path,rate,data in items:CONVERTED[(path,rate)]=data
