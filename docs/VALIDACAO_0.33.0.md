# Validação 0.33.0

- Build numa pasta limpa com ps2dev: ELF 4,7 MB (sem dados do jogo), `SPRITES.BIN` 63,5 MB, `CAMPAIGN.BIN` 2,8 MB, `SOUNDS.BIN` 0,4 MB, ISO 71,3 MB.
- Kit para Windows sob Wine: ELF e os três arquivos de dados idênticos aos do build.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO inicia, mostra "PROTOTIPO 0.33.0" e lista 375 fases;
  - ISO de teste na Birdday 5-3: régua dobrável, calculadora, post-its e cenário de papelão desenhados (`docs/pcsx2_0.33.0_fase_birdday_5-3.png`).
- Ao carregar, nenhuma peça do Birdday 5 cai sozinha; algumas estruturas altas se acomodam vários metros antes do tiro, como em fases de outros episódios (física aproximada).
- Suíte completa aprovada: 52 verificações.
- Campanha: 368 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6, Birdday 4-4 e Birdday 6-1.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
