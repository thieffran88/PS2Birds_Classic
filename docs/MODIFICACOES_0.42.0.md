# Modificações 0.42.0

- **Tutorial (página 64, 15 fases)**
  - Ordem do `episodes.lua` (Tutorial_red_niko a Tutorial_mayhem_niko), banco de texturas 60; fases sem limite de estrelas dão 3 estrelas em qualquer vitória.
  - 5 peças novas pela `add_definitions.py`: rocha de fazenda, taça, baú de tesouro, colete de laboratório e pote de ouro (meia resolução).
  - Cinco fases são tutoriais da loja de power-ups (mira, poção, estilingue real, terremoto, onda de choque); sem os power-ups no port, são jogadas como fases comuns.
  - Ao carregar, nenhuma das 15 fases perde porco.
- **Roteiros novos (fase por fase)**
  - Ham 'Em High extra 1, 2 e 3; Dino 2-15, 3-4 e 3-10 passam a ter roteiro vencedor.
- **Levantamento**
  - Os power-ups estão inteiros em Lua (`powerups_Powerup_*.lua`, `PowerupManager.lua`): portáveis sem código nativo. Registrado em `docs/LACUNAS_PORT.md`.
- **Testes**
  - `profile_test`: Tutorial (ordem, banco, peças, os cinco tutoriais de power-up).
  - `sprite_test`: 61 bancos, 713 quadros.
  - `campaign_test`: 709 fases.
  - Suíte com 69 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.42.0; README com 709 fases; captura do Tutorial 1; documentos e captura da 0.41.0 substituídos.
  - Entregues os quatro arquivos: ISO, ELF (com os .BIN), Gerador para Windows e Código-Fonte; os da 0.41.0 removidos da pasta de trabalho.
