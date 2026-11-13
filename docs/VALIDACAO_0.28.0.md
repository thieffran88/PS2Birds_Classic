# Validação 0.28.0

- ELF (9,6 MB), `SPRITES.BIN` (34,9 MB) e ISO (44,6 MB) compilados com ps2dev numa pasta limpa.
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.28.0" e lista 285 fases;
  - ISO de teste aberta na 15-2: banco 11 lido do disco, ilha flutuante com porcos de chapéu e ferramentas (`docs/pcsx2_0.28.0_fase_15-2.png`).
- Suíte completa aprovada: 43 verificações.
- Campanha: 274 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7 e 8 fases grandes de Mine and Dine (busca em andamento).
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico; o desempenho do Box2D com ~300 peças num PS2 real não foi medido.
