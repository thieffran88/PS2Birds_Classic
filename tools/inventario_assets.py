#!/usr/bin/env python3
"""Lista enumerada dos assets dos APKs do Classic e do Rio com a situação no port: Incluído, Parcial, Necessário ou Desnecessário.
Só lê os APKs (os arquivos que o port gera e as tabelas em ps2/src); não executa script do jogo.
uso: inventario_assets.py APK_CLASSIC APK_RIO DADOS_RIO_DIR SAIDA.txt"""
import json,re,struct,sys,zipfile,collections
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path[:0]=[str(ROOT/'tools'),str(ROOT/'tools/rio')]
from game_profiles import decode_payload
import build_campaign,extract_ui,extract_menu,extract_audio
def status_ratio(n,total,full=0.85,low=0.10):
    if total==0:return 'Fora'
    r=n/total
    return 'Incluído' if r>=full else 'Parcial' if r>=low else 'Fora'
def base(n):return re.sub(r'[ _]*(a?\d+|[a-z]\d+)$','',n.rsplit('/',1)[-1].rsplit('.',1)[0],flags=re.I).strip().lower()

# Fora -> Necessário (faz sentido incluir ao port) ou Desnecessário (não faz sentido incluir).
UI_SOM=r'purchase|coin|gem|ticket|league|reward|claim|confetti|challenger|promote|demote|leaderboard|sticker|notification|shuffle|star_bar|get_stars|use_stars|level_unlock|menu_button|button_radio|^ui_|/ui_|abp_|intro_comic|scratchy|major_|minor_|accordion|day_end|masterleague|massive|daily|bonus_level_unlocked|chapter_completed|hintbox|offer|star_\d_coins|unlock_box|alarm_egg|bubble_appears|con_used|firework'
def necessidade(sec,name):
    n=name.lower();rio='RIO' in sec
    if 'EFEITOS SONOROS' in sec:
        if re.search(UI_SOM,n):return 'Desnecessário','som de loja, moedas, recompensas, ligas ou menu; o port não tem essas telas'
        return 'Necessário','som de jogo (colisão, dano, pássaro, bloco) que o port ainda não usa'
    if 'MÚSICA' in sec:
        if 'game_complete' in n:return 'Desnecessário','declarada no audio.lua, mas nenhum script do APK a toca'
        if re.search(r'xmas|ssvsab|funky',n):return 'Desnecessário','música de evento ou promoção'
        return 'Necessário','música de fase ou ambiente de episódio que o port tem'
    if 'FONTES' in sec or n.startswith('truetype'):
        if re.search(r'chinese|_ja|_ru|_ko|_zh|noout|\.font|truetype',n):return 'Desnecessário','idioma sem texto no port ou fonte do sistema Android'
        return 'Desnecessário','o port usa só FONT_BASIC e FONT_SCORE; as telas dessas fontes não existem no port'
    if 'texts_update' in n or re.search(r'shader|módulo android|dados |logos de abertura',n):return 'Desnecessário','específico do Android, rede, anúncios, compras, atualização do app ou logos de empresas'
    if 'FASES' in sec:
        if 'ugc' in n:return 'Desnecessário','fases criadas por jogadores (precisam de servidor)'
        if re.search(r'eagledemo|powerupschool',n):return 'Desnecessário','demonstração da loja / escola de power-ups'
        return 'Necessário','fases jogáveis offline'
    if 'TEMAS' in sec or 'CENAS' in sec:
        if rio:return 'Necessário','tema ou cena das fases do Rio 2 (entra com os pacotes de fases)'
        return 'Desnecessário','configuração de tema/cena sem arte própria'
    if 'backgrounds_ge' in n:return 'Desnecessário','fundo dos instrumentos musicais do Golden Eggs (teclado, acordeão, rádio), que o port não tem'
    if re.search(r'shop|iap|store|purchase|achiev|channel|daily|forced_update|crosspromo|xpromo|gamecenter|gift|popup|reward|splash|mattel|ugc|localiz|korea|id_sheet|ml_|league|sticker|tournament|powerup_reminder|hud_ingame|igc_|hint|tutorial|golden_eggs_sheet|levelselection|menu_native|menu_elements|menu_toons|menu_parts|buttons_|misc_sheet|gradient|lighting',n):
        return 'Desnecessário','tela de loja, rede, conquistas, ligas, promoções ou menu que o port não tem (ou refaz com arte própria)'
    if rio and re.search(r'nye|maunas|amazon|forest|smallfish|rio2|rio_2|r2_',n):return 'Necessário','arte do Rio 2 (entra com as fases do Rio 2)'
    if re.search(r'theme|ground|parallax|backgrounds',n):return 'Desnecessário','cenário já montado em THEMES.BIN; o port usa o cenário composto'
    if re.search(r'ingame|pigs|birds|blocks|consumable|macaws|particles|awnings|boats|cutscene|egg_?defender|black',n):
        return 'Necessário','arte de jogo (pássaros, blocos, porcos, cutscenes) que o port ainda não tem'
    return 'Desnecessário','arte de interface que o port não usa'
class Lista:
    def __init__(s):s.lines=[];s.n=0;s.count=collections.Counter()
    def sec(s,t):s.sec_name=t;s.lines+=['',t,'-'*len(t)]
    def add(s,name,st,detail=''):
        if st=='Fora':
            st,why=necessidade(getattr(s,'sec_name',''),name);detail=why+('; '+detail if detail else '')
        s.n+=1;s.count[st]+=1;s.lines.append(f'{s.n:5d}. {name} ({st})'+(f' - {detail}' if detail else ''))
def names_in_header(path,pat,size,base_off,count_off):
    return []
def sprite_names(h):return set(re.findall(r'#define SPR_(\w+)',Path(h).read_text()))
def classic(L,apk):
    z=zipfile.ZipFile(apk);names=z.namelist()
    port=sprite_names(ROOT/'ps2/src/sprite_ids.h')|{n for _,_,n in extract_ui.UI}|set(extract_ui.PARTICLES)|{n for _,n,_ in extract_menu.SPRITES}
    camp=build_campaign.read_binary(ROOT/'ps2/assets/CAMPAIGN.BIN');scripts={l['script'] for l in camp['levels']}
    # --- Fases
    L.sec('CLASSIC - FASES (por pacote de fases; entre parênteses, fases no port / fases no APK)')
    packs=collections.defaultdict(list)
    eps=decode_payload(z.read('assets/data/scripts/episodes.lua'),'classic')
    for n in names:
        m=re.match(r'assets/data/levels/([^/]+)/([^/]+)\.lua$',n)
        if m and not m.group(2).endswith('.scripts'):packs[m.group(1)].append(m.group(2))   # *.scripts.lua are the waves of the Egg Defender stages, not stages
    for pk,lv in sorted(packs.items()):
        ref=[x for x in lv if re.search(rb'[^A-Za-z0-9_]'+re.escape(x.encode())+rb'\x00',eps)] or lv   # stages the episode lists; the others are never shown by the game
        inc=sum(1 for x in ref if x in scripts);st='Incluído' if inc==len(ref) else 'Parcial' if inc else 'Fora'
        extra=f'; {len(lv)-len(ref)} fase(s) do arquivo que o jogo não lista nos episódios' if len(ref)<len(lv) else ''
        L.add('Pacote de fases '+pk,st,f'{inc}/{len(ref)} fases'+extra)
    # --- Folhas de sprites
    L.sec('CLASSIC - FOLHAS DE SPRITES (quadros no port / quadros da folha)')
    sheets={}
    for n in names:
        if n.endswith('.sheet.json'):
            try:fr=[f['filename'] for f in json.loads(decode_payload(z.read(n),'classic'))['frames']]
            except Exception:continue
            sheets.setdefault(n.rsplit('/',1)[-1][:-11],set()).update(fr)
    cut_used={s['name'] for n in names if n.endswith('.compo.json') and 'CUTSCENE' in n for c in json.loads(decode_payload(z.read(n),'classic'))['compo'] for s in c['sprites']}   # pieces the cutscene compositions place
    d0=open(ROOT/'ps2/assets/CUTSCENE.BIN','rb').read();cnt0=struct.unpack_from('<I',d0,8)[0]
    for sh,fr in sorted(sheets.items()):
        inc=len(fr&port)
        if sh.startswith('EGGDEFENDER_CUTSCENES_'):L.add('Folha '+sh,'Incluído',f'painéis das cenas do Egg Defender, montados no CUTSCENE.BIN ({cnt0} cenas)');continue
        if sh.startswith('CUTSCENE'):
            u=len(fr&cut_used);L.add('Folha '+sh,status_ratio(u,len(fr)),f'{u}/{len(fr)} quadros entram nas cenas já montadas no CUTSCENE.BIN ({cnt0} cenas); o resto não é usado por nenhuma cena');continue
        L.add('Folha '+sh,status_ratio(inc,len(fr)),f'{inc}/{len(fr)} quadros')
    # --- Outras imagens (sem .sheet.json)
    L.sec('CLASSIC - OUTRAS IMAGENS (sem tabela de quadros)')
    used=set(sheets)
    seen=set()
    for n in sorted(names):
        if n.startswith('assets/data/images/') and n.endswith(('.png','.pvr','.webp','.7z')):
            b=n.rsplit('/',1)[-1];b=re.sub(r'\.(png|pvr|webp|stream\.7z|7z)$','',b)
            if b in used or b in seen:continue
            seen.add(b);L.add('Imagem '+b,'Fora','nenhum quadro é usado pelo port')
    # --- Sons
    L.sec('CLASSIC - EFEITOS SONOROS (arquivos .ogg)')
    man=json.load(open(ROOT/'ps2/assets/audio_manifest.json'));srcs={s['source'] for s in man['sounds']};grp={base(s) for s in srcs}
    for n in sorted(names):
        if n.startswith('assets/data/audio/sfx/') and n.endswith('.ogg'):
            L.add(n[len('assets/data/audio/'):],'Incluído' if n in srcs else 'Parcial' if base(n) in grp else 'Fora','' if n in srcs else ('variante de um som usado' if base(n) in grp else ''))
    L.sec('CLASSIC - MÚSICA E AMBIENTES (arquivos .ogg)')
    mm=json.load(open(ROOT/'ps2/assets/music_manifest.json'));msrcs={t['source'] for t in mm['tracks']}
    for n in sorted(names):
        if n.startswith('assets/data/audio/music/') and n.endswith('.ogg'):L.add(n[len('assets/data/audio/'):],'Incluído' if n in msrcs else 'Fora')
    other=[n for n in names if n.startswith('assets/data/audio/') and n.endswith('.ogg') and '/sfx/' not in n and '/music/' not in n]
    for n in sorted(other):L.add(n[len('assets/data/audio/'):],'Fora')
    # --- Fontes
    L.sec('CLASSIC - FONTES')
    fonts=sorted({re.sub(r'\.(dat|pvr|png|json)$','',n.rsplit('/',1)[-1]) for n in names if n.startswith('assets/data/fonts/')})
    for f in fonts:L.add('Fonte '+f,'Incluído' if f in('FONT_BASIC','FONT_SCORE') else 'Fora','' if f in('FONT_BASIC','FONT_SCORE') else 'o port usa só FONT_BASIC e FONT_SCORE')
    for n in sorted(names):
        if n.endswith('.ttf'):L.add('TrueType '+n.rsplit('/',1)[-1],'Fora','fonte do sistema Android')
    # --- Cenas, temas, textos
    L.sec('CLASSIC - CENAS (cutscenes.lua) E TEMAS (themes.lua)')
    d=open(ROOT/'ps2/assets/CUTSCENE.BIN','rb').read();cnt=struct.unpack_from('<I',d,8)[0];have={d[12+72*i:12+72*i+24].rstrip(b'\0').decode() for i in range(cnt)}
    import lua51_data
    cs=lua51_data.load(decode_payload(z.read('assets/data/scripts/cutscenes.lua'),'classic'))
    for k in sorted(cs):L.add('Cena '+str(k),'Incluído' if k in have else 'Fora')
    th=lua51_data.load(decode_payload(z.read('assets/data/scripts/themes.lua'),'classic'))
    td=open(ROOT/'ps2/assets/THEMES.BIN','rb').read();tc=struct.unpack_from('<I',td,8)[0];thave={td[20+40*i:20+40*i+32].rstrip(b'\0').decode() for i in range(tc)}
    for k in sorted(th):L.add('Tema '+str(k),'Incluído' if k in thave else 'Fora')
    L.sec('CLASSIC - TEXTOS, VEÍCULOS DO EGG DEFENDER E OUTROS')
    for n in sorted(names):
        if n.startswith('assets/data/localization/'):
            if 'TEXTS_UPDATE' in n:L.add(n[len('assets/data/'):],'Fora','verificação de atualização do app (3 frases); o port não tem atualização')
            else:L.add(n[len('assets/data/'):],'Parcial','o port lê só o texto pt_BR da tela "sobre"; os demais textos estão escritos no código')
    veh=collections.defaultdict(list)
    for n in names:
        m=re.match(r'assets/data/vehicles/([^/]+)/',n)
        if m:veh[m.group(1)].append(n)
    for k,v in sorted(veh.items()):
        stage=k in {s.rsplit('_W',1)[0] for s in scripts}|{s for s in scripts};L.add('Veículos '+k,'Incluído' if any(s.startswith(k) for s in scripts) else 'Fora',f'{len(v)} arquivos')
    sh=[n for n in names if n.startswith('assets/data/shaders/') and n.endswith(('.fx','.ps','.vs','.sh'))]
    L.add('Shaders do Android (GLES2): %d arquivos'%len(sh),'Fora','o PS2 não usa shaders do Android; a mistura e a cor são feitas pelo GS')
    mods=collections.Counter()
    for n in names:
        m=re.match(r'assets/files/(modules/[^/]+|[^/]+)',n)
        if m:mods[m.group(1)]+=1
    for k,v in sorted(mods.items()):L.add('Módulo Android '+k,'Fora',f'{v} arquivos (anúncios, compras, rede)')
    for k in ('config','certificates'):
        c=[n for n in names if n.startswith('assets/data/%s/'%k)]
        if c:L.add('Dados '+k+' (%d arquivos)'%len(c),'Fora','rede e compras do Android')
def rio(L,apk,data):
    z=zipfile.ZipFile(apk);names=z.namelist();data=Path(data)
    port=sprite_names(ROOT/'ps2/src/rio/sprite_ids.h')
    import extract_ui_rio,extract_menu_rio,extract_cutscene_rio
    port|=set(extract_ui_rio.RIO_UI.values())|{extract_ui_rio.particle(n) for n in extract_ui.PARTICLES}|{e[1] for e in extract_menu_rio.S if e[1]}|set(extract_menu_rio.CARD_FRAMES)|{'CUTSCENE_IMAGE_%d'%i for _,_,im in extract_cutscene_rio.SCENES for i in im}
    camp=build_campaign.read_binary(data/'CAMPAIGN.BIN');scripts={l['script'] for l in camp['levels']}
    L.sec('RIO - FASES (por pacote; fases no port / fases no APK)')
    packs=collections.defaultdict(list)
    for n in names:
        m=re.match(r'assets/data/levels/([^/]+)/([^/]+)\.lua$',n)
        if m:packs[m.group(1)].append(m.group(2))
    for pk,lv in sorted(packs.items()):
        inc=sum(1 for x in lv if x in scripts);L.add('Pacote de fases '+pk,'Incluído' if inc==len(lv) else 'Parcial' if inc else 'Fora',f'{inc}/{len(lv)} fases')
    L.sec('RIO - FOLHAS DE SPRITES (.dat; quadros no port / quadros da folha)')
    inv=json.loads((ROOT/'work/rio/analysis/dat_inventory.json').read_text());sheets={}
    for p in inv['parsed']:
        nm=p['path'].rsplit('/',1)[-1][:-4]
        sheets.setdefault(nm,set()).update(f['name'] for f in p.get('frames') or [])
    comps=json.loads((ROOT/'work/rio/analysis/comp_inventory.json').read_text());cn=set()
    for p in comps['parsed']:cn.add(p['path'].rsplit('/',1)[-1][:-4])
    for sh,fr in sorted(sheets.items()):
        inc=len(fr&port);L.add('Folha '+sh,status_ratio(inc,len(fr)),f'{inc}/{len(fr)} quadros')
    for c in sorted(cn-set(sheets)):L.add('Composições '+c,'Parcial','desenhadas inteiras no port quando uma peça de fase as usa (macacos, gaiolas); o resto não')
    L.sec('RIO - OUTRAS IMAGENS (sem tabela de quadros)')
    seen=set()
    for n in sorted(names):
        if n.startswith('assets/data/images/') and n.endswith(('.png','.pvr','.webp')):
            b=re.sub(r'\.(png|pvr|webp)$','',n.rsplit('/',1)[-1])
            if b in sheets or b in seen:continue
            seen.add(b);L.add('Imagem '+b,'Fora')
    L.sec('RIO - EFEITOS SONOROS (.mp3)')
    man=json.load(open(data/'audio_manifest.json'));srcs={e['source'] for e in man['effects'] if e.get('source')};grp={base(s) for s in srcs}
    for n in sorted(names):
        if n.startswith('assets/data/audio/sfx/') and n.endswith('.mp3'):
            L.add(n[len('assets/data/audio/'):],'Incluído' if n in srcs else 'Parcial' if base(n) in grp else 'Fora','' if n in srcs else ('variante de um som usado' if base(n) in grp else ''))
    L.sec('RIO - MÚSICA E AMBIENTES (.mp3)')
    mm=json.load(open(data/'music_manifest.json'));msrcs={t['source'] for t in mm['tracks'] if t.get('source')}
    for n in sorted(names):
        if n.startswith('assets/data/audio/music/') and n.endswith('.mp3'):L.add(n[len('assets/data/audio/'):],'Incluído' if n in msrcs else 'Fora')
    L.sec('RIO - FONTES')
    fonts=sorted({re.sub(r'\.(dat|pvr|png|json)$','',n.rsplit('/',1)[-1]) for n in names if n.startswith('assets/data/fonts/')})
    for f in fonts:L.add('Fonte '+f,'Incluído' if f in('FONT_BASIC','FONT_SCORE') else 'Fora','' if f in('FONT_BASIC','FONT_SCORE') else 'o port usa só FONT_BASIC e FONT_SCORE')
    L.sec('RIO - TEMAS (themes.lua), CENAS E TEXTOS')
    import lua51_data
    th=lua51_data.load(decode_payload(z.read('assets/data/scripts/themes.lua'),'rio'))
    td=(data/'THEMES.BIN').read_bytes();tc=struct.unpack_from('<I',td,8)[0];thave={td[20+40*i:20+40*i+32].rstrip(b'\0').decode() for i in range(tc)}
    for k in sorted(th):L.add('Tema '+str(k),'Incluído' if k in thave else 'Fora')
    cd=(data/'CUTSCENE.BIN').read_bytes();cc=struct.unpack_from('<I',cd,8)[0]
    for i in range(cc):L.add('Cena '+cd[12+72*i:12+72*i+24].rstrip(b'\0').decode(),'Incluído','7 animações do CutsceneScene.lua')
    L.add('Logos de abertura (Rovio, Fox, Blue Sky) e créditos longos','Fora')
    for n in sorted(names):
        if n.startswith('assets/data/localization/'):
            if 'TEXTS_UPDATE' in n:L.add(n[len('assets/data/'):],'Fora','verificação de atualização do app (3 frases); o port não tem atualização')
            else:L.add(n[len('assets/data/'):],'Parcial','o port lê só o texto "sobre"; os demais textos estão no código')
    sh=[n for n in names if n.startswith('assets/data/shaders/') and n.endswith(('.fx','.ps','.vs','.sh'))]
    L.add('Shaders do Android (GLES2): %d arquivos'%len(sh),'Fora','o PS2 não usa shaders do Android')
def main(ac,ar,data,out):
    L=Lista();classic(L,ac);rio(L,ar,data)
    head=['PS2Birds - LISTA DE ASSETS DO CLASSIC E DO RIO','Estado em 24/11/2026. Versões: Classic 1.4.2, Rio 0.53.3.','',
      'Critérios (entre parênteses em cada item):','  Incluído - completo no port (todos os arquivos ou quadros que o jogo usa estão lá).',
      '  Parcial  - o port tem boa parte, mas não está completo.','  Necessário    - fora do port, mas faz sentido incluir (fases, arte, sons e músicas de jogo).','  Desnecessário - fora do port e não faz sentido incluir (loja, rede, anúncios, ligas, UGC, outros idiomas, Android).',
      'Folhas de sprites: Incluído com 85% ou mais dos quadros no port, Parcial de 10% a 84%, abaixo disso Necessário ou Desnecessário (conforme o conteúdo).',
      'Pacotes de fases: Incluído com todas as fases, Parcial com algumas, Necessário ou Desnecessário com nenhuma.',
      'Sons: Incluído quando o arquivo é usado; Parcial quando é variante (a2, a3...) de um som usado; Necessário ou Desnecessário nos demais.',
      'Gerada por tools/inventario_assets.py a partir dos APKs e dos arquivos do port.']
    tot=['','RESUMO: '+', '.join(f'{k} {v}' for k,v in sorted(L.count.items()))+f' (total {L.n})']
    Path(out).write_text('\n'.join(head+tot+L.lines)+'\n',encoding='utf-8');print(tot[1])
if __name__=='__main__':main(*sys.argv[1:5])
