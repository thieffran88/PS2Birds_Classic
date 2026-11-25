# Modificações — Classic 1.5.0 e Rio 0.54.0 (variantes de som sorteadas)

## O que é
Os sons de material do original têm várias gravações (`wood destroyed a1`, `a2`, `a3`...) e o jogo sorteia uma a cada vez. O port tocava sempre a `a1`.

## Adicionado
- **Sorteio entre variantes** para 9 grupos, nos dois jogos: quebrar (madeira, vidro/luz, pedra), colisão forte (madeira, gelo/vidro, pedra) e dano (madeira, luz, pedra). Cada grupo tem 3 gravações (`a1` a `a3`; as colisões têm até `a4` no original, o port usa 3).
- `SOUNDS.BIN`: Classic com 55 efeitos (37 + 18 variantes) e Rio com 62 (44 + 18). As variantes ocupam ids depois dos outros, na ordem de `VARIANT_GROUPS` (`extract_audio.py`), espelhada em `audio_variants` (`audio_ids.h`); `audio_play` sorteia com um LCG simples, e pedir uma variante diretamente também funciona.

## Editado
- `tools/extract_audio.py`, `tools/rio/extract_audio_rio.py`, `ps2/src/audio_ids.h`, `ps2/src/audio.c`, `ps2/tests/audio_assets_test.py` (55 efeitos). Versões: Classic 1.5.0, Rio 0.54.0.

## Ainda de fora
- Sons de eventos que o jogo não dispara (rolagem, TNT, fogos, água, macacos, Dino/ossos...) e as variantes a4 em diante. O banco ainda tem folga de 1 MiB, mas cada grupo novo pede o gatilho no código.
- Não conferi no ouvido (nem o sorteio nem o volume).
