# Validação 0.35.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 70,8 MB, `CAMPAIGN.BIN` 3,5 MB, `SOUNDS.BIN` 0,4 MB, ISO 79,4 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.35.0" e lista 435 fases;
  - ISO de teste no Surf and Turf 1-1 (`docs/pcsx2_0.35.0_fase_surf_and_turf_1-1.png`).
- Suíte completa aprovada: 56 verificações.
- Campanha: 423 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4, 6-1, 7-7, 7-14 e Surf and Turf 2-15, 3-5, 3-14.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
