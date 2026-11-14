# Validação 0.37.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 85,2 MB, `CAMPAIGN.BIN` 4,5 MB, `SOUNDS.BIN` 0,4 MB, ISO 94,8 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.37.0" e lista 525 fases;
  - ISO de teste no Short Fuse 1-1: pássaro de choque no estilingue, tubos, frascos, ímã e cristais (`docs/pcsx2_0.37.0_fase_short_fuse_1-1.png`).
- A onda do pássaro de choque foi conferida no computador (`globe_test`); no emulador só a cena foi vista.
- Suíte completa aprovada: 60 verificações.
- Campanha: 501 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4, 6-1, 7-7, 7-14, Surf and Turf 2-15, 3-5, 3-14, Bad Piggies 2-7, 2-15, 3-5, 3-12 e Short Fuse 1-7, 1-15, 2-2, 2-4, 2-5, 2-7, 2-13, 3-3.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
