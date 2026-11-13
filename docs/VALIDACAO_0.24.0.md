# Validação 0.24.0

- ELF e ISO compilados com ps2dev a partir de `make clean` (ELF de 19,7 MB).
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.24.0" e lista 150 fases;
  - ELFs de teste abertos direto na 7-13 e na 8-15 mostram as cenas completas, com a 7-13 agora de pé sobre o chão (`docs/pcsx2_0.24.0_fase_7-13.png` e `docs/pcsx2_0.24.0_fase_8-15.png`).
- Suíte completa aprovada: 36 verificações.
- Campanha: 148 vitórias com roteiros determinísticos; 3-20 e 5-16 sem roteiro. Uma nova tentativa nessas duas, com o chão corrigido, foi interrompida antes de terminar.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico.
