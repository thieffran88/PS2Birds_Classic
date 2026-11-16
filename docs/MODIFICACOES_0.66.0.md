# Modificações completas — PS2Birds Classic 0.66.0

## Menu de fases no formato do original
- Novo `ps2/src/menu.c` / `menu.h`, montado a partir de `episode_selection.lua` e `level_selection.lua` (traduzidos à mão).
- **Tela de episódios:** 19 episódios, formados pelos títulos das fases (ex.: "POACHED EGGS 1-1" → "POACHED EGGS"). Cada um mostra fases concluídas/total e estrelas/máximo, ou BLOQUEADO.
- **Grade de fases:** 5 × 3 por página (15 botões), como no original. Cada célula mostra o rótulo ("1-1", "6"), as estrelas e o cadeado. Embaixo ficam os pontinhos de página e uma linha com o título e o RECORDE.
- **Controles:**
  - SELECT abre a grade do episódio da fase atual;
  - direcional move o cursor;
  - X entra ou joga;
  - O volta;
  - L2/R2 trocam a página.
- Fases bloqueadas não abrem.
- Conferido no PCSX2: captura em `docs/pcsx2_0.66.0_menu_fases.png`.
- Novo teste `ps2/tests/menu_test.cpp`: 19 episódios somando 745 fases, rótulos, navegação, páginas, cadeados e volta.

## Egg Defender: busca mais fina
- O `ed_solver` agora tenta esperas de 0, 1,5, 3 e 6 s, com e sem habilidade.
- **Egg Defender 8** (7 tiros) e **Egg Defender 14** (3 tiros) vencidas; entraram no teste da campanha.
- Fases sem roteiro vencedor: de 73 para 71.
- 1, 2, 3, 4, 5, 7, 11, 13 e 15 ainda perdem na grade atual.

## Limpeza
- Removidos os documentos da 0.65.0 (substituídos pelos da 0.66.0) e a captura da 0.64.0.

## Versão
- Tela inicial e README: 0.66.0.
