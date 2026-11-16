# Modificações completas — PS2Birds Classic 0.68.0

## Sons originais da tela de fim de fase
- Os nomes vêm do `audio.lua` decompilado, traduzido à mão.
- Cinco efeitos novos extraídos do APK e convertidos para SPU2 ADPCM:
  - `star_1`, `star_2`, `star_3` (`sfx/star_1..3.ogg`): um para cada estrela que aparece;
  - `score_count` (`sfx/gamescorescreen_score_count_loop.ogg`, 0,36 s): repete enquanto os pontos sobem e para quando a contagem termina, como no `level_end.lua`;
  - `new_highscore` (`sfx/highscore.ogg`): toca junto com "NOVO RECORDE!".
- `SOUNDS.BIN` passa de 20 para 25 efeitos (498 KB, dentro do limite de 1 MB da SPU2).
- Cada estrela tem seu próprio canal de voz, para uma não cortar a anterior (são 0,8 s entre elas e os sons duram até 3,3 s).
- O teste `audio_assets_test.py` agora confere os 25 efeitos.
- Conferido no PCSX2: 1-1 vencida, 31000 pontos, 2 estrelas, "NOVO RECORDE", sem travamentos. Captura em `docs/pcsx2_0.68.0_fim_de_fase.png`.

## Limpeza
- Removidos os documentos e a captura da 0.67.0 (substituídos pelos da 0.68.0).
- Removidos da área temporária as capturas de teste, o log do emulador e as pastas da 0.67.0.

## Versão
- Tela inicial e README: 0.68.0.
