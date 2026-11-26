# Modificações — Rio 0.56.0 (sons da água)

O Classic continua na 1.6.1.

## Adicionado
- **Respingos:** quando o pássaro entra ou sai da água toca `bird_enter_water` / `bird_exit_water`, e quando outra peça cruza a superfície toca `object_enter_water` / `object_exit_water`; cada um com 3 gravações sorteadas.
- **Superfície da água:** `LevelEffectWater.lua` guarda a água em blocos marcadores (`BLOCK_INDESTRUCTIBLE_WATER_LEVEL`, `AMAZON_RIVER_WATER_LEVEL_*`); a linha d'água da fase é a do marcador mais alto. O port só usa isso para o som: não há amortecimento nem flutuação.
- Os primeiros 0,5 s da fase são silenciosos (o desmoronar inicial das peças não conta como respingo).
- `SOUNDS.BIN` do Rio: 85 efeitos (52 + 33 variantes), cerca de 0,75 MB de 1 MiB.

## Editado
- `tools/rio/extract_audio_rio.py` (`RIO_ONLY`, `RIO_VARIANT_GROUPS`), `ps2/src/audio_ids.h`, `game.h` (eventos `GAME_EVENT_WATER_*`), `game.cpp` (`water_setup`/`water_check`, só no Rio), `main.c`. Versão do Rio: 0.56.0.

## Ainda de fora
- Física da água (amortecimento e flutuação), bolhas e partículas do respingo, correntes, Nigel e o chefe dos macacos. Não conferi no ouvido, nem se a linha d'água dos marcadores bate com o desenho do mar em todos os temas.
