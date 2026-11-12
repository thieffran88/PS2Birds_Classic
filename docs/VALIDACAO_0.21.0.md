# Validação 0.21.0

- ELF e ISO compilados com ps2dev a partir de `make clean`.
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.21.0" e lista 105 fases;
  - ELFs de teste abertos direto na 5-1 e na 5-16 mostram as cenas completas com as texturas do banco 2 (`docs/pcsx2_0.21.0_fase_5-1.png` e `docs/pcsx2_0.21.0_fase_5-16.png`).
  - O aviso de áudio nas capturas vem do ambiente, que não tem placa de som.
- Suíte completa aprovada, incluindo os novos testes da página 5 e do banco 2.
- Campanha: 103 vitórias com roteiros determinísticos; 3-20 e 5-16 sem roteiro.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico.
