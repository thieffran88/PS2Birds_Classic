# Validação 0.32.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 48,5 MB, `CAMPAIGN.BIN` 2,3 MB, `SOUNDS.BIN` 0,4 MB, ISO 55,9 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.32.0" e lista 345 fases;
  - ISO de teste na Birdday 6-2: três balões levando porcos pelas juntas prismáticas, bolos e roda (`docs/pcsx2_0.32.0_fase_birdday_6-2.png`).
- Juntas prismáticas, motor das dobradiças e peças sem colisão conferidos no computador (`joint_test`).
- Suíte completa aprovada: 50 verificações.
- Campanha: 338 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4 e Birdday 6-1.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
