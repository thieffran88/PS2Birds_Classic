# Modificações — Classic 1.6.0 (taxa de amostragem menor nos efeitos longos; ossos, frascos e estalactites)

## Adicionado
- **Efeitos longos com taxa de amostragem menor** (Classic e Rio): efeitos acima de 1,4 s ficam a 12000 Hz e acima de 0,6 s a 16000 Hz (o cabeçalho APCM leva a altura certa; a reprodução não muda de tom). O banco do Classic caiu de 958 KB para 652 KB e o do Rio de 944 KB para 625 KB, o que abriu espaço para os sons abaixo.
- **Sons próprios por tipo de peça** (só no Classic; nomes de definição dos Dino, frascos e estalactites):
  - ossos do Dino: `bone_destroy_1..3` (quebrar), `bone_collision_1..3` (batida forte), `bone_damage_1..3` (dano);
  - frascos e tubos de vidro (`EMPTY_VIAL*`, `BLACKBIRD_VIAL*`, `GLASS_PIPE*`): `vial_break_01..03`;
  - estalactites: `stalaktite_break_01..03`.
  Cada um com sorteio entre 3 gravações. Eventos novos em `game.h`; `sound_family()` em `game.cpp` escolhe a família pelo nome.
- `SOUNDS.BIN` do Classic: 86 efeitos (52 + 34 variantes). O sistema de variantes aceita nomes com número no formato do arquivo (`{}` ou `{:02d}`).

## Editado
- `tools/extract_audio.py` (`rate_for`, `CLASSIC_ONLY`, `CLASSIC_VARIANT_GROUPS`), `tools/rio/extract_audio_rio.py`, `ps2/src/audio_ids.h`, `game.h`, `game.cpp`, `main.c`, `ps2/tests/audio_assets_test.py` (86 efeitos). Versão do Classic: 1.6.0.

## Ainda de fora
- Os sons de joias (`jewel_break`), bolo (`birthday_cake`), palmeiras (`palm_leaves`), fole (`bellows`), canhão (`cannon_shot`), passos do Dino, TNT e fogos. Não conferi no ouvido; o limiar de impacto/dano é o mesmo das versões anteriores.
