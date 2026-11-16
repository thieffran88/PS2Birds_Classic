# Modificações completas — PS2Birds Classic 0.79.0

## Pássaros voando na tela inicial (`main_menu.lua`, `animateBirds`)
- Como no original, pássaros são lançados de baixo da tela e atravessam a tela inicial em arco, puxados pela gravidade. Regras traduzidas à mão:
  - enquanto houver menos de 75% da lista no ar, há 1 chance em 5 por quadro de lançar outro;
  - camada sorteada de 3 a 5 (escala 0,2 × camada);
  - partida de 1,5 tela à esquerda até 0,75 tela à direita, abaixo do chão;
  - velocidade de 100 a 350 para o lado e de 150 a 400 para cima, proporcional à camada;
  - gravidade de 150 px/s²;
  - o pássaro some ao cair de novo abaixo da tela;
  - as camadas são desenhadas da mais distante para a mais próxima.
- Pássaros: vermelho, azul, amarelo, preto, branco/verde, o irmão grande e o bumerangue, tirados da folha `INGAME_BIRDS_1` do APK.
- Não incluídos: o grito ao tocar no pássaro, que depende de toque na tela, e as recompensas (ovos de ouro etc.).
- **Arte e dados:**
  - nova função `sheet_sprite()` em `extract_sprites.py`, que lê um sprite isolado de uma folha PNG ou `stream`;
  - o `extract_menu.py` grava os 7 pássaros no `MENU.BIN` (74 sprites);
  - o teste `menu_assets_test.py` foi atualizado.

## Correção: recorte (scissor) na área da tela
- Os pássaros nascem abaixo da tela, e o GS não recortava o que passava da linha 448: o desenho invadia a memória de vídeo depois do buffer de tela, e a tela inicial ficava preta.
- Agora o recorte do GS é ajustado a 640×448 a cada quadro (`gsKit_set_scissor`), o que também protege as fases.
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.79.0_passaros_menu.png`):
  - pássaros cruzando a tela inicial;
  - a fase 1-1 normal em seguida.

## Limpeza
- Removidos os documentos da 0.78.0 (substituídos pelos da 0.79.0).
- Removidas da área temporária as pastas da 0.78.0, as ISOs de teste, as capturas e os logs.

## Versão
- Tela de pausa, "Sobre" e README: 0.79.0.
