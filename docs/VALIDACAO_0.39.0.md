# Validação 0.39.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 101,5 MB, `CAMPAIGN.BIN` 5,7 MB, `SOUNDS.BIN` 0,4 MB, ISO 112,4 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.39.0" e lista 591 fases;
  - ISO de teste no Piggy Farm 1-1: tratores, feno, trigo e peças espelhadas (`docs/pcsx2_0.39.0_fase_piggy_farm_1-1.png`).
- Suíte completa aprovada: 64 verificações.
- Campanha: 564 vitórias com roteiros determinísticos; 27 sem roteiro (as 26 da 0.38.0 e Piggy Farm 1-8).
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
