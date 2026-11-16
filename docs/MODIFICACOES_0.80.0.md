# Modificações completas — PS2Birds Classic 0.80.0

## Partículas de impacto (folha `PARTICLES_1` do APK)
- **Peças:** quando uma peça quebra, saem estilhaços do material:
  - madeira (`WOOD_PARTICLE_1-3`);
  - vidro (`LIGHT_PARTICLE_1-5`);
  - pedra (`ROCK_PARTICLE_1-3`).
- **Porcos:** um porco derrotado solta uma nuvem de fumaça (`SMOKE_BUFF_1-3`).
- **Pássaros:** no primeiro choque, cada pássaro solta penas da sua cor (vermelho, azul, amarelo, preto, verde, irmão grande e bumerangue).
- As partículas giram, caem com gravidade e somem aos poucos, com até 128 ao mesmo tempo.

## Pontuações com a arte original
- O 5000 dos porcos usa o sprite verde `5K_GREEN`.
- O 10000 dos pássaros que sobraram usa o sprite `10K_<pássaro>` na cor do pássaro (vermelho, azul, amarelo, preto, branco, bumerangue).
- Os 500 das peças continuam com a fonte de pontos original.

## Página de interface em 8 bits
- O `UI.BIN` virou uma página PSMT8 de 128×1024 com uma paleta de 256 cores (CT32). Ocupa os mesmos 128 KB da página de 16 bits anterior, mas cabe mais que o dobro de sprites: 53 ao todo (11 de fim de fase e pausa, mais 42 partículas e pontuações).
- A paleta é reduzida por corte mediano (median cut) em Python puro, sem biblioteca de imagem, então o kit para Windows continua funcionando.
- **Arquivos alterados:** `extract_ui.py` (formato v2), `ui.c`/`ui.h` (envio manual em faixas, desenho girado com `ui_draw_rotated`) e `ui_assets_test.py`.
- **Correção encontrada no caminho:** a textura enviada à mão precisava do campo `TBW` (largura do buffer). Com ele em 0, a GS repetia as primeiras 64 linhas, e apareciam estrelas no lugar dos botões.

## Jogo
- O ponto flutuante (`game_add_popup`) guarda o material de origem: o tipo da peça, o porco ou o tipo do pássaro. É ele que escolhe o tipo de partícula e o sprite de pontuação.
- O teste `popup_test.cpp` foi atualizado.

## Conferido no PCSX2
- Captura em `docs/pcsx2_0.80.0_particulas.png`.
- Na fase 1-1 aparecem:
  - estilhaços de madeira e vidro;
  - os 500;
  - o 5000 verde no porco;
  - o 10000 vermelho no pássaro restante.
- Depois, a tela de vitória com estrelas e botões corretos.

## Limpeza
- Removidos os documentos da 0.79.0 (substituídos pelos da 0.80.0).
- Removidas da área temporária as pastas da 0.79.0, as ISOs de teste e as capturas.

## Versão
- Tela de pausa, "Sobre" e README: 0.80.0.
