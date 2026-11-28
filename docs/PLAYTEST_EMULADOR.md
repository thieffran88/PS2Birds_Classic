# Playtest no emulador (autoplay + `tools/emu_playtest.py`)

Uma rotina que abre fases no PCSX2 sem ninguém no controle, dá tiros e relata o que aconteceu. Serve para pegar travamentos, fases que não carregam ou não assentam, e para tirar capturas de cada episódio.

## Como funciona
- **No jogo (`ps2/src/main.c`):** se existir um `AUTOPLAY.TXT` junto dos arquivos de dados (no disco, ou na pasta do ELF), o jogo, depois de ~240 quadros de boot, abre a fase pedida, espera, dá os tiros e imprime no console linhas `AUTOPLAY ...`. Formato do arquivo: `level N` (número da fase, a partir de 1, na ordem do `CAMPAIGN.BIN`), `shots K` (padrão 3), `wait S` (segundos de espera entre os tiros, padrão 6). Sem o arquivo nada muda.
- **Os tiros** são uma lista fixa de ângulos e forças (32/75, 40/78, 26/72, 45/80, 36/76). Não é um solucionador: uma fase não vencida só mostra que carrega, assenta, aguenta os tiros e termina sem travar.
- **Na máquina de desenvolvimento (`tools/emu_playtest.py`):** para cada fase monta uma ISO de teste (ELF + dados + `AUTOPLAY.TXT`), roda o PCSX2 sem janela (`-batch -fastboot -logfile`), espera a linha `AUTOPLAY RESULT`, tira capturas da abertura e do fim e grava `report.md` e `report.json`.

## Uso
```
python3 tools/emu_playtest.py --game rio --elf PS2BIRDS_RIO.ELF --data PASTA_DOS_BIN \
        --pcsx2 pcsx2-qt --out saida [--levels episodes | 1,181,278] [--shots 3] [--timeout 300] [--display :91]
```
- `--levels episodes` (padrão) joga a primeira fase de cada episódio.
- Precisa de `pycdlib`, de um display X (por exemplo `Xvfb :91 -screen 0 1280x800x24`), do `import` do ImageMagick para as capturas e, opcionalmente, do `xdotool` (fecha o aviso de "sem dispositivo de áudio" do PCSX2 antes da primeira captura).
- A linha `AUTOPLAY` só aparece no log do PCSX2 com `-logfile` (a ferramenta já usa).

## Resultado de referência (Rio 0.58.0, uma fase de cada episódio, 3 tiros)
17 de 17 episódios correram até o fim sem travar. Fases que terminaram "LOST" com 3 tiros (essas fases têm 3 pássaros e a rotina dá 3 tiros): Jungle Escape 1, Airfield Chase 1, Rocket Rumble 1, High Dive 1, Timber Tumble 1, Hidden Harbor 1 e Treasure Hunt 1; as demais ficaram abertas com um pássaro sobrando. Nenhuma foi vencida (os tiros são fixos).

## Resultado no Classic (1.7.2)

22 de 22 fases (uma por episódio) correram até o fim. Vencidas com os tiros fixos: Poached Eggs 1-1, Mighty Hoax 4-1, Big Setup 9-1, Birdday 2-1, Surf and Turf 1-1, Short Fuse 1-1 e Tutorial 1. Siege 1 terminou LOST; as demais ficaram abertas.

## Fases escolhidas: `--levels spread`
Joga a fase do meio e a última de cada episódio (além das primeiras, de `--levels episodes`); também aceita uma lista (`--levels 315,458`).

Resultado (Classic 1.7.2 / Rio 0.58.1):
- **Classic:** 44 de 44 fases correram até o fim. Vencidas: Danger Above 7-8, Ham 'Em High 14-15, Birdday 6-8, Bad Piggies 2-8 e Flock Favorites 2-15.
- **Rio:** 33 de 34. Nenhuma vencida (tiros fixos).
- **Achado:** Rio fase 315 (Timber Tumble 20) não termina no emulador. É a fase mais pesada do Rio (436 objetos, 208 peças, 17 macacos). No PCSX2 cada passo de física passou a levar segundos e o jogo praticamente para; no host (mesmo código) a fase roda normal em ~1 s para 3000 passos. Não sei se é lentidão do emulador sem GPU (software) ou um problema real de desempenho no PS2: testei limitar a 4 passos por quadro e não mudou, então não incluí essa mudança. Registrado como pendência.
