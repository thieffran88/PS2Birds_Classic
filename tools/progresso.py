"""Acompanhamento da geração da ISO: etapas numeradas, relógio, sinal de vida e um painel no navegador.

- No console: cada etapa mostra "[3/9] Sons e música ..." e, enquanto ela trabalha, uma linha de sinal de vida a cada
  poucos segundos ("ainda trabalhando: 2 min 10 s nesta etapa"), para saber que o programa não travou.
- Em arquivo: tudo que aparece fica também em GERAR_ISO_LOG.txt, com a hora de cada linha.
- No navegador (opção --painel): uma página local (só neste computador, endereço 127.0.0.1) com a barra de etapas,
  o tempo, o "último sinal de vida" e o registro detalhado, atualizada a cada segundo. Não usa internet nem bibliotecas.
Tudo com a biblioteca padrão do Python."""
import http.server,json,socketserver,sys,threading,time,webbrowser
from pathlib import Path

def fmt(seconds):
    seconds=int(seconds);m,s=divmod(seconds,60);h,m=divmod(m,60)
    return (f'{h} h ' if h else '')+(f'{m} min ' if m or h else '')+f'{s} s'

PAGE='''<!doctype html><html lang="pt-BR"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Gerador de ISO - acompanhamento</title>
<style>
:root{--bg:#10151f;--card:#1a2232;--tx:#e8edf5;--mu:#9aa7bd;--ok:#3ecf8e;--wa:#f5b942;--er:#ff6b6b;--ac:#5aa9ff}
@media (prefers-color-scheme:light){:root{--bg:#f3f5f9;--card:#fff;--tx:#18202e;--mu:#5b6678;--ok:#12805c;--wa:#9a6700;--er:#c62828;--ac:#1565c0}}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--tx);font:16px/1.45 system-ui,Segoe UI,Arial,sans-serif;padding:16px}
main{max-width:980px;margin:0 auto;display:grid;gap:14px}.card{background:var(--card);border-radius:12px;padding:16px}
h1{font-size:1.3rem;margin:0 0 4px}.mu{color:var(--mu)}#banner{font-size:1.15rem;font-weight:600}
.bar{height:14px;background:rgba(128,140,160,.25);border-radius:8px;overflow:hidden;margin:10px 0}.bar i{display:block;height:100%;background:var(--ac);width:0;transition:width .4s}
ol{margin:0;padding-left:1.4em}li{padding:2px 0}li.done{color:var(--ok)}li.run{color:var(--ac);font-weight:600}li.wait{color:var(--mu)}
pre{margin:0;max-height:46vh;overflow:auto;font:13px/1.45 ui-monospace,Consolas,monospace;white-space:pre-wrap;word-break:break-word}
.row{display:flex;flex-wrap:wrap;gap:18px}.row div{min-width:140px}.big{font-size:1.5rem;font-weight:700}
</style></head><body><main>
<div class="card"><h1>Gerador de ISO</h1><div id="banner">Conectando...</div><div class="bar"><i id="bar"></i></div>
<div class="row"><div><div class="mu">Tempo total</div><div class="big" id="total">-</div></div><div><div class="mu">Nesta etapa</div><div class="big" id="stage">-</div></div>
<div><div class="mu">Último sinal de vida</div><div class="big" id="beat">-</div></div></div></div>
<div class="card"><b>Etapas</b><ol id="steps"></ol></div>
<div class="card"><b>Registro detalhado</b><pre id="log"></pre></div></main>
<script>
let stick=true;const log=document.getElementById('log');log.onscroll=()=>{stick=log.scrollTop+log.clientHeight>=log.scrollHeight-24};
function t(s){s=Math.floor(s);const m=Math.floor(s/60),h=Math.floor(m/60);return (h?h+' h ':'')+(m%60||h?(m%60)+' min ':'')+(s%60)+' s'}
async function tick(){try{const r=await fetch('/estado?'+Date.now());const e=await r.json();
 document.getElementById('total').textContent=t(e.total);document.getElementById('stage').textContent=e.current?t(e.stage_time):'-';
 document.getElementById('beat').textContent='há '+t(e.beat_age);
 const b=document.getElementById('banner');let cls='var(--ok)',txt='Em execução. O programa está trabalhando.';
 if(e.error){cls='var(--er)';txt='Erro: '+e.error}else if(e.done){cls='var(--ok)';txt='Concluído! '+(e.result||'')}else if(e.beat_age>90){cls='var(--wa)';txt='Sem sinais há '+t(e.beat_age)+'. Pode ser uma etapa longa; se passar de 10 minutos, feche e tente de novo.'}
 b.style.color=cls;b.textContent=txt;
 document.getElementById('bar').style.width=(100*e.finished/e.count)+'%';
 document.getElementById('steps').innerHTML=e.steps.map((s,i)=>'<li class="'+s.state+'">'+s.name+(s.state=='done'?' - '+t(s.time):s.state=='run'?' - em andamento':'')+'</li>').join('');
 log.textContent=e.log.join('\\n');if(stick)log.scrollTop=log.scrollHeight;
 if(!e.done&&!e.error)setTimeout(tick,1000);else setTimeout(tick,4000)}catch(x){document.getElementById('banner').textContent='Aguardando o programa...';setTimeout(tick,2000)}}
tick();
</script></body></html>'''

class Progress:
    """Etapas, registro e sinal de vida; opcionalmente grava um arquivo e serve o painel no navegador."""
    def __init__(self,steps,logfile=None,panel=False,heartbeat=10,echo=print):
        self.names=list(steps);self.state=['wait']*len(steps);self.times=[0.0]*len(steps);self.current=None
        self.t0=time.time();self.t_step=self.t0;self.beat=self.t0;self.lines=[];self.done=False;self.error=None;self.result=''
        self.lock=threading.Lock();self.echo=echo;self.heartbeat=heartbeat;self.file=None;self.server=None
        if logfile:
            self.file=open(logfile,'w',encoding='utf-8',buffering=1)
        self._stop=threading.Event();threading.Thread(target=self._pulse,daemon=True).start()
        if panel:self._start_panel()
    # ---- registro
    def write(self,text,echo=True):
        """Uma linha de registro (também usada para os avisos dos extratores)."""
        for line in str(text).splitlines() or ['']:
            stamp=time.strftime('%H:%M:%S');entry=f'{stamp}  {line}'
            with self.lock:
                self.lines.append(entry);self.beat=time.time()
                if len(self.lines)>4000:del self.lines[:1000]
                if self.file:self.file.write(entry+'\n')
            if echo:self.echo(line)
    def step(self,i,detail=None):
        """Começa a etapa i (0, 1, ...); a anterior termina."""
        now=time.time()
        with self.lock:
            if self.current is not None:self.state[self.current]='done';self.times[self.current]=now-self.t_step
            self.current=i;self.state[i]='run';self.t_step=now
        self.write(f'[{i+1}/{len(self.names)}] {self.names[i]} ...'+(f' {detail}' if detail else ''))
    def finish(self,result=''):
        now=time.time()
        with self.lock:
            if self.current is not None:self.state[self.current]='done';self.times[self.current]=now-self.t_step
            self.current=None;self.done=True;self.result=result
        self.write(f'Concluído em {fmt(now-self.t0)}. {result}')
    def fail(self,message):
        self.error=message;self.write('ERRO: '+message)
    def close(self):
        self._stop.set()
        if self.file:self.file.close()
        if self.server:
            time.sleep(3)   # deixa o navegador ver o resultado final
            self.server.shutdown()
    # ---- sinal de vida
    def _pulse(self):
        while not self._stop.wait(self.heartbeat):
            if self.current is not None and not self.done and not self.error:
                self.write(f'   ... ainda trabalhando: {fmt(time.time()-self.t_step)} nesta etapa, {fmt(time.time()-self.t0)} no total',echo=True)
    # ---- painel
    def _snapshot(self):
        now=time.time()
        with self.lock:
            return {'total':now-self.t0,'stage_time':now-self.t_step,'beat_age':now-self.beat,'current':self.current,'count':len(self.names),
                    'finished':sum(1 for s in self.state if s=='done'),'done':self.done,'error':self.error,'result':self.result,
                    'steps':[{'name':n,'state':s,'time':self.times[i]} for i,(n,s) in enumerate(zip(self.names,self.state))],'log':self.lines[-400:]}
    def _start_panel(self):
        outer=self
        class H(http.server.BaseHTTPRequestHandler):
            def do_GET(self):
                if self.path.startswith('/estado'):body=json.dumps(outer._snapshot()).encode('utf-8');kind='application/json'
                else:body=PAGE.encode('utf-8');kind='text/html; charset=utf-8'
                self.send_response(200);self.send_header('Content-Type',kind);self.send_header('Cache-Control','no-store');self.send_header('Content-Length',str(len(body)));self.end_headers();self.wfile.write(body)
            def log_message(self,*a):pass
        socketserver.TCPServer.allow_reuse_address=True
        self.server=socketserver.ThreadingTCPServer(('127.0.0.1',0),H);self.server.daemon_threads=True
        threading.Thread(target=self.server.serve_forever,daemon=True).start()
        self.url='http://127.0.0.1:%d/'%self.server.server_address[1]
        self.write(f'Painel de acompanhamento: {self.url}')
        opened=False
        try:opened=bool(webbrowser.open(self.url))
        except Exception:pass
        if not opened:self.write('Abra o endereço acima no navegador para acompanhar.')

class Tee:
    """Substitui sys.stdout para que os avisos dos extratores também entrem no registro e no painel."""
    def __init__(self,progress,real):self.p=progress;self.real=real;self.buf=''
    def write(self,s):
        self.buf+=s
        while '\n' in self.buf:
            line,self.buf=self.buf.split('\n',1)
            if line.strip():self.p.write('   '+line,echo=False)
        return self.real.write(s)
    def flush(self):self.real.flush()
    def isatty(self):return False
