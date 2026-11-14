# Validação 0.38.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 87,9 MB, `CAMPAIGN.BIN` 4,8 MB, `SOUNDS.BIN` 0,4 MB, ISO 97,8 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.38.0" e lista 546 fases;
  - ISO de teste no Bird Island 1-1: estátua de pássaro, árvores floridas e arbustos (`docs/pcsx2_0.38.0_fase_bird_island_1-1.png`).
- Suíte completa aprovada: 62 verificações.
- Campanha: 520 vitórias com roteiros determinísticos; 26 sem roteiro (as 24 da 0.37.0 e Bird Island 1-10 e 1-15).
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
