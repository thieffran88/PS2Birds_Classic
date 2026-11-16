# Modificações completas — PS2Birds Classic 0.67.0

## Tela de fim de fase no ritmo do original
- Novo `ps2/src/level_end.c` / `level_end.h`. É a tradução à mão de `LevelComplete.update`, `updateScoreCounter` e `checkHighScore` (`level_end.lua`).
- **Contagem dos pontos:** sobe de 0 até a pontuação em min(pontos ÷ limite de prata × 2 − 0,5; 2,7) segundos, como no original.
- **Estrelas:** aparecem uma a uma, aos 0,4 s, 1,2 s e 2,0 s (estrela *i* quando tempo × 1,25 + 0,5 ≥ *i*). Só aparecem as estrelas conquistadas.
- **Novo recorde:** "NOVO RECORDE!" aparece 1 s depois do fim da contagem, só quando a pontuação supera o recorde anterior. Caso contrário, mostra o recorde atual.
- **Botões do original (Menu, Reiniciar, Próxima):**
  - X: próxima fase;
  - TRIÂNGULO: reiniciar;
  - O: volta ao menu de fases (novo).
- Conferido no PCSX2 (1-2 vencida: 41000 pontos, "NOVO RECORDE"); captura em `docs/pcsx2_0.67.0_fim_de_fase.png`.
- **Derrota:** o título agora é "FASE PERDIDA" (`MT_LEVEL_FAILED`), com TRIÂNGULO para reiniciar e O para o menu.
- Novo acessor `game_level_silver()` (limite de prata da fase, o `silverScore` do original).
- Novo teste `ps2/tests/level_end_test.cpp`: tempo da contagem, momento de cada estrela, limite de 2,7 s, selo de recorde e escurecimento do fundo (até 0,65).

## Limpeza
- Removidos os documentos da 0.66.0 (substituídos pelos da 0.67.0).
- Removidos da pasta de build os logs e programas de sonda da busca do Egg Defender (`ed_beam*.log`, `edprobe`, `settle_probe`, `solver`, `solver3`, `ct.log`, `elf.log`, ISO antiga). Também foi removida uma cópia antiga do kit na área temporária.

## Versão
- Tela inicial e README: 0.67.0.
