# Modificações — Classic 1.5.1 (sons dos porcos)

Só o Classic (no Rio os alvos são gaiolas, não porcos); o Rio continua na 0.54.0.

## Adicionado
- **Colisão e dano dos porcos:** uma pancada forte num porco toca `piglette collision` e um dano que o porco aguenta toca `piglette damage`, com sorteio entre 3 gravações cada (eventos `GAME_EVENT_HIT_PIG` e `GAME_EVENT_DMG_PIG`). O som do porco tem prioridade sobre o do material no mesmo quadro.
- **Grunhido:** enquanto a fase espera o próximo tiro e ainda há porco vivo, um porco grunhe de vez em quando (`piglette oink`, 3 gravações; a cada 6 a 12 s, sorteado). O original usa um temporizador de ociosidade por porco; o port usa um temporizador único.
- `SOUNDS.BIN` do Classic: 64 efeitos (40 base + 24 variantes), cerca de 0,9 MB de 1 MiB.

## Editado
- `tools/extract_audio.py` (`CLASSIC_ONLY`, `CLASSIC_VARIANT_GROUPS`), `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, `ps2/tests/audio_assets_test.py` (64 efeitos). Versão do Classic: 1.5.1.

## Ainda de fora
- `pig_singing`, `pig_snare`/`hi-hat` (os porcos "bateristas" do Piggy Farm), choques elétricos e poções; as variantes `a4` em diante. Não conferi no ouvido.
