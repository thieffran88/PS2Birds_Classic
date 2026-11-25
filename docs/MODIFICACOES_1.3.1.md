# Modificações — Classic 1.3.1 (música do Egg Defender)

O Rio continua na 0.50.3. Primeira entrega da lista de assets "Necessário".

## O que é
Nas fases do Egg Defender o original troca a música do cenário por `eggDefenderMusic1..5` (`EggDefender.lua`, `setMusic("eggDefenderMusic"..musicIntensityLevel)`): começa na intensidade 1 e sobe a cada nova onda de veículos, até a 5. Os arquivos são `music/level_0`, `level_1`, `level_2`, `level_3_1` e `level_4_2`. O port tocava o ambiente do tema.

## Adicionado
- 5 faixas no `MUSIC.BIN` (21 faixas; `MUSIC_EGG`..+4 em `music.h`), laçadas como as demais.
- `game_music_override()`: nas fases com ondas devolve a faixa da intensidade da onda atual (1 a 5); nas outras, -1 (continua o ambiente do tema).
- `docs/MODIFICACOES_1.3.1.md` (este arquivo).

## Editado
- `tools/extract_audio.py` (lista `MUSIC`), `ps2/src/main.c` (usa `game_music_override` antes do ambiente do tema; versão 1.3.1), `ps2/tests/campaign_test.cpp` (confere a faixa de cada fase).

## Fora (decisão)
- `music/game_complete`: o `audio.lua` a declara, mas nenhum script do APK a toca. Continua sem uso.
- `music/AB_Xmas_2016`, `SSvsAB_remix` e `funky_theme` seguem Desnecessário (evento/promoção).
