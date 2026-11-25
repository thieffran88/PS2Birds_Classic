# Modificações — Classic 1.4.1 e Rio 0.53.1 (sons de dano)

Continuação da 1.4.0 (sons de jogo da lista "Necessário").

## Adicionado
- **Dano das peças:** quando uma peça de madeira, vidro/gelo ou pedra sofre dano e continua de pé, toca o som de dano do material (`wood damage a1`, `light damage a1`, `rock damage a1`). Evento `GAME_EVENT_DMG_WOOD/GLASS/STONE` (`game.cpp`, no ponto em que o impacto vira dano), tocado em `main.c`.
- Prioridade dos sons do material: quebrar (som que já existia) > dano > colisão forte (1.4.0). Cada quadro toca no máximo um.
- `SOUNDS.BIN`: Classic com 36 efeitos e Rio com 43.

## Editado
- `tools/extract_audio.py` (`CLASSIC_ONLY`), `tools/rio/extract_audio_rio.py`, `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, `ps2/tests/audio_assets_test.py` (36 efeitos).
- Versões: Classic 1.4.1, Rio 0.53.1.

## Ainda de fora
- Rolagem (`rolling`), destruição de ossos e estátuas do Dino, seleção de pássaro, TNT e fogos, e as variantes a2 a a6. Não conferi no ouvido; o limiar é o mesmo do dano do jogo (impacto acima de 3).
