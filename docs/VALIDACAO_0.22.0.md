# Validação 0.22.0

- ELF e ISO compilados com ps2dev a partir de `make clean`.
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.22.0" e lista 120 fases;
  - ELFs de teste abertos direto na 6-5 e na 6-13 mostram as cenas completas, com os pássaros verdes na fila e os balões (`docs/pcsx2_0.22.0_fase_6-5.png` e `docs/pcsx2_0.22.0_fase_6-13.png`).
- Suíte completa aprovada: 33 verificações, incluindo `green_test` e o teste de procedência da página 6.
- Campanha: 118 vitórias com roteiros determinísticos; 3-20 e 5-16 sem roteiro.
- O voo do pássaro verde foi conferido no computador. No emulador só a cena foi vista; o toque não foi testado com controle. Sem teste em PS2 físico.
