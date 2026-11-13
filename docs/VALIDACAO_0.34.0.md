# Validação 0.34.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 66,1 MB, `CAMPAIGN.BIN` 3,1 MB, `SOUNDS.BIN` 0,4 MB, ISO 74,3 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.34.0" e lista 390 fases;
  - ISO de teste na Birdday 7-6: torres de pedra e vidro, cupcake e pássaros (`docs/pcsx2_0.34.0_fase_birdday_7-6.png`).
- Ao carregar, nenhuma peça do Birdday 7 cai sozinha; os balões sobem, como devem.
- Suíte completa aprovada: 54 verificações.
- Campanha: 381 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4, 6-1, 7-7 e 7-14.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
