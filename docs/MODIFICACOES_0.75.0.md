# Modificações completas — PS2Birds Classic 0.75.0

## Pontos flutuantes (`gamelogic.lua`: `addFloatingScore` e `updateFloatingScores`)
- Os pontos aparecem onde foram ganhos, na fonte original `FONT_SCORE`:
  - **peça destruída:** o valor (500, ou o das peças frágeis) por 0,6 s, no tamanho `0,25 + pontos/3000` do original;
  - **porco:** 5000 em verde, por 0,9 s, no tamanho 1;
  - **pássaros que sobram:** 10000 em vermelho sobre cada um, 0,9 s, um a cada 0,5 s (`birdBuffTimer`).
- **Animação** do `updateFloatingScores`: cresce nos primeiros 0,25 s, fica parado e encolhe nos últimos 0,25 s.
- O original usa sprites coloridos para os 5000 e 10000; o port pinta a mesma fonte de verde e vermelho.
- A tela de vitória agora espera 1 s + 0,5 s por pássaro restante, para o bônus aparecer antes, como no original. A contagem dos pontos e as estrelas começam depois disso.
- **Código:**
  - novas funções `game_add_popup()` e `game_popup_scale()` e a lista `popups` no `Game`;
  - o tempo dos pontos avança mesmo depois do fim da fase.
- Novo teste `ps2/tests/popup_test.cpp`:
  - duração e tamanho de cada tipo;
  - crescer, parar e encolher;
  - espera do bônus;
  - uma vitória real na 1-1 gera o 5000 do porco, e todos os pontos somem em 2 s.
- **Conferido no PCSX2** (a captura `docs/pcsx2_0.75.0_pontos.png` mostra o 10000 e a vitória; o "500" foi visto na rodada anterior da mesma versão):
  - "500" sobre as peças;
  - "10000" sobre os pássaros que sobraram;
  - a tela de vitória abrindo depois.

## Gerador
- Sem mudança nos dados: os arquivos `.BIN` são os mesmos da 0.74.0; muda só o ELF.

## Limpeza
- Removidos os documentos da 0.74.0 (substituídos pelos da 0.75.0).
- Removidas da área temporária as pastas da 0.74.0, as capturas de teste e o log de testes.

## Versão
- Tela de pausa e README: 0.75.0.
