# Modificações — Classic 1.5.2 (sons dos pássaros especiais)

Só o Classic; o Rio continua na 0.54.0 (já tinha esses sons).

## O que faltava
No Classic só tocavam os lançamentos e as habilidades dos pássaros vermelho, azul, amarelo, preto e branco; o boomerang (verde), o grande (Big Brother), o rosa e o globo (baiacu) lançavam e ativavam em silêncio ou com o som do vermelho.

## Adicionado
- **Lançamento:** boomerang (`bird_06_flying`), pássaro grande (`bigbrother_fly`) e rosa (`pink_launch_01`).
- **Habilidade ao toque:** boomerang (`boomerang_activate`), grande (`bigbrother_yell`), globo (`Globe_Bird_Special_Activation_1`) e rosa (`pink_activate_01`).
- `main.c` usa a mesma regra de som nos dois jogos (o Rio só diferencia o vermelho; o rosa do Rio usa o lançamento do vermelho, porque o banco do Rio está quase cheio).
- `SOUNDS.BIN` do Classic: 71 efeitos (47 base + 24 variantes).

## Editado
- `tools/extract_audio.py` (`CLASSIC_ONLY`), `ps2/src/audio_ids.h`, `main.c`, `ps2/tests/audio_assets_test.py` (71 efeitos). Versão do Classic: 1.5.2.

## Ainda de fora
- Colisão do pássaro rosa (`pink_collision_*`), `pink_special_*`, Dino (passos, ossos), TNT, fogos e as variantes de lançamento/ativação. Não conferi no ouvido.
