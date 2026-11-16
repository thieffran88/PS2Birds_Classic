# Modificações completas — PS2Birds Classic 0.70.0

## Ambiência de cada tema dentro das fases
- Cada fase toca, em repetição, a ambiência do seu tema, como no original:
  - o tema vem do campo `theme` do arquivo da fase;
  - a música do tema vem de `themes.lua` (`music`), lido do APK só como dados;
  - o arquivo de cada música vem de `audio.lua`, traduzido à mão;
  - a repetição segue `gamelogic.lua` (`currentMusic`).
- Onze ambiências do APK:
  - `ambient_white_dryforest`, `ambient_green_jungleish`, `ambient_red_savannah`, `ambient_city`;
  - `ambient_construction`, `ab_cave_ambient`, `birthday_ambience`, `BeachAmbience`;
  - `ambience_waterfall`, `RainAmbience`, `ambience_blackbird_01`.
- 699 das 745 fases têm ambiência. As 46 do episódio Dino ficam sem música: a `dino_ambience` do original é baixada à parte e não está no APK.
- **Memória:**
  - `title_theme` e `level_complete` ficam sempre na memória;
  - só a ambiência do tema atual é carregada, lida do disco ao entrar numa fase de outro tema, nunca durante o jogo;
  - fases seguidas do mesmo tema não leem o disco de novo.
- `MUSIC.BIN` passa a ter 13 faixas (cerca de 28 MB).
- `CAMPAIGN.BIN` passa à versão 8: cada fase guarda a faixa da sua ambiência. Há uma função nova, `game_level_music()`.
- Ao vencer, a ambiência dá lugar à `level_complete`. Na fase seguinte, ela volta.
- O teste `music_test.cpp` agora também confere a ambiência lida sob demanda, a repetição e uma faixa inexistente.
- Os arquivos de teste da física ganharam o campo novo da fase.
- **Conferido no PCSX2:**
  - o console mostra "PS2Birds: ambience 2 playing" ao entrar na 1-1 e ao passar para a 1-2;
  - a 1-1 foi vencida e a 1-2 abriu normalmente;
  - o emulador aqui não tem saída de som, então a música não foi ouvida.

## Drive
- Ao enviar uma versão nova, os zips da versão anterior vão para a lixeira do Drive.

## Limpeza
- Removidos os documentos da 0.69.0 (substituídos pelos da 0.70.0).
- Removidas da área temporária as pastas da 0.69.0, os arquivos de música de teste, a captura e o log de testes.

## Versão
- Tela inicial e README: 0.70.0.
