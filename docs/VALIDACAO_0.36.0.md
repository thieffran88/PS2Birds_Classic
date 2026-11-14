# Validação 0.36.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 77,2 MB, `CAMPAIGN.BIN` 3,9 MB, `SOUNDS.BIN` 0,4 MB, ISO 86,2 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.36.0" e lista 480 fases;
  - ISO de teste no Bad Piggies 1-1: veículos dos porcos, sacos, galões e pneus (`docs/pcsx2_0.36.0_fase_bad_piggies_1-1.png`).
- Suíte completa aprovada: 58 verificações.
- Campanha: 464 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4, 6-1, 7-7, 7-14, Surf and Turf 2-15, 3-5, 3-14 e Bad Piggies 2-7, 2-15, 3-5, 3-12.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
