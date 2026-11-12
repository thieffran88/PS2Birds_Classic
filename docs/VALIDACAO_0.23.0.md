# Validação 0.23.0

- ELF e ISO compilados com ps2dev a partir de `make clean`.
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.23.0" e lista 135 fases;
  - ELFs de teste abertos direto na 7-2 (200 objetos) e na 7-3 mostram as cenas completas (`docs/pcsx2_0.23.0_fase_7-2.png` e `docs/pcsx2_0.23.0_fase_7-3.png`).
  - Os blocos frágeis de 1×1 quase não aparecem, porque o port ainda não desenha a textura de tema.
- Suíte completa aprovada: 34 verificações, incluindo a migração de save v4 → v5 e o teste de procedência da página 7.
- Campanha: 133 vitórias com roteiros determinísticos; 3-20 e 5-16 sem roteiro.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico.
