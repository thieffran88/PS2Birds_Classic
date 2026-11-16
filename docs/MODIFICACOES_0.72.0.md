# Modificações completas — PS2Birds Classic 0.72.0

## Fontes originais do jogo
- Novo `tools/extract_font.py` e novo arquivo `FONT.BIN`, com as fontes bitmap do APK:
  - `FONT_BASIC`: HUD e menus, branca com contorno preto, nas letras do ASCII e nas acentuadas do português;
  - `FONT_SCORE`: algarismos dos pontos, branca com contorno laranja.
- **Leitura dos arquivos do APK:**
  - tabela de glyphs (`.dat`, formato KA3D): altura da linha, número de glyphs e, para cada um, caractere, x, y, largura, altura e altura acima da linha de base;
  - folhas `.pvr` em RGBA4444.
- **Formato no PS2:**
  - reduzidas a 44% e 32% das originais de 1024×768;
  - guardadas numa única página de 4 bits (PSMT4) de 256×128, com uma paleta de 16 cores por fonte (do branco à cor do contorno, em 3 níveis de transparência).
- **Memória de vídeo:**
  - no GS, uma página de 4 bits ocupa blocos de 128×128 pixels; uma folha mais alta passaria a 32 KB e não caberia, por isso a página tem altura máxima de 128 linhas;
  - a página (16 KB) e as duas paletas (no mesmo bloco de 8 KB, separadas por 256 bytes) ocupam exatamente os 32 KB que sobravam depois do `UI.BIN`.
- Novo módulo `ps2/src/font.c` / `font.h`:
  - lê texto UTF-8, alinha à esquerda, ao centro ou à direita e aplica a cor do texto ao branco da letra;
  - usa o teste de alfa contra o pontilhado do buffer de 16 bits;
  - letras acentuadas que a folha não tem (ex.: "Â") viram a letra sem acento.
- Todos os textos do jogo (HUD, menus, pausa, fim de fase) passam a usar a `FONT_BASIC`. Sem `FONT.BIN`, volta a fonte de pixels anterior.
- Novo teste `ps2/tests/font_assets_test.py`: cabeçalho, página de até 128 linhas, paletas, caracteres do português e glyphs sem sobreposição.

## HUD no formato do original (`GameHud.lua`)
- Removidas as faixas escuras de cima e de baixo, que o original não tem.
- Canto superior direito: "Pontuação", o número grande e "Recorde: N" menor (70% no original).
- Canto superior esquerdo: o nome da fase.
- Saíram da tela de jogo o ângulo, a força, a barra de força, a contagem de porcos e blocos, a versão e as dicas de controle. Dicas, versão, som e cartão de memória foram para a tela de pausa.
- Textos originais em português do `TEXTS_BASIC.dat` do APK: "Pontuação", "Recorde", "NÍVEL CONCLUÍDO!", "NÍVEL NÃO CONCLUÍDO!", "Novo recorde!" e "pausa".
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.72.0_hud_fontes.png`):
  - menu de episódios e fase em jogo com a HUD nova;
  - vitória com "NÍVEL CONCLUÍDO!";
  - tela de pausa.

## Gerador e documentação
- `gerar_iso.py` grava `FONT.BIN` na ISO e na pasta USB/HDD. Agora são sete arquivos.
- O kit para Windows inclui o `extract_font.py`. O README e o `LEIA-ME` foram atualizados.

## Limpeza
- Removidos os documentos da 0.71.0 (substituídos pelos da 0.72.0) e a captura do menu da 0.66.0, que mostrava a fonte antiga.
- Removidas da área temporária as pastas da 0.71.0, as prévias das folhas de fonte, as capturas de teste e o log de testes.

## Versão
- Tela de pausa e README: 0.72.0.
