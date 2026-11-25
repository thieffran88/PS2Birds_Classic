# Modificações — Classic 1.4.0 e Rio 0.53.0 (sons de impacto e de colisão)

Primeira entrega dos efeitos sonoros da lista de assets "Necessário". Antes, o jogo só tocava som quando uma peça quebrava; agora as peças e os pássaros também soam ao bater.

## Adicionado
- **Impacto dos blocos:** quando uma peça de madeira, vidro/gelo ou pedra sofre um choque forte (impulso normal acima de 6) e nada quebra naquele quadro, toca o som de colisão do material: `wood collision a1`, `ice light collision a1` e `rock collision a1`. Evento `GAME_EVENT_HIT_WOOD/GLASS/STONE` em `game.cpp`, tocado em `main.c`.
- **Colisão de cada pássaro (Classic):** na primeira batida do pássaro lançado toca o som do próprio pássaro (`bird 01..05 collision a1`, de `birds.lua`); o Rio já fazia isso.
- `SOUNDS.BIN`: Classic com 33 efeitos (498 KB para ~560 KB, de 1 MiB) e Rio com 40 (694 KB). Ids novos em `audio_ids.h`; lista `CLASSIC_ONLY` em `extract_audio.py`.

## Editado
- `tools/extract_audio.py`, `tools/rio/extract_audio_rio.py`, `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, `ps2/tests/audio_assets_test.py` (33 efeitos).
- Versões: Classic 1.4.0, Rio 0.53.0.

## Ainda de fora
- As demais variantes (a2..a6) e os sons por evento que o jogo ainda não dispara: dano (`damage`), rolagem (`rolling`), seleção de pássaro, sons de bloco de Dino/ossos, TNT e fogos. O limite é o 1 MiB de memória de som do SPU2 (sobram cerca de 400 KB no Classic e 330 KB no Rio) e cada som novo pede o gatilho no código.
- O limiar de 6 do impacto é uma calibração minha: ajuste `HIT_SOUND_IMPULSE` em `game.cpp` se tocar demais ou de menos.
