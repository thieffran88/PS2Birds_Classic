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
17 de 17 episódios correram até o fim sem travar. Fases que terminaram "LOST" com 3 tiros (4 pássaros: a rotina dá 3): Jungle Escape 1, Airfield Chase 1, Rocket Rumble 1, High Dive 1, Timber Tumble 1, Hidden Harbor 1 e Treasure Hunt 1; as demais ficaram abertas com um pássaro sobrando. Nenhuma foi vencida (os tiros são fixos).
