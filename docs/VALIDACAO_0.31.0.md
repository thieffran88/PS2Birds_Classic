# Validação 0.31.0

- Build numa pasta limpa com ps2dev: ELF 4,6 MB (sem dados do jogo), `SPRITES.BIN` 45,8 MB, `CAMPAIGN.BIN` 2,0 MB, `SOUNDS.BIN` 0,4 MB, ISO 52,9 MB.
- Gerador (`tools/gerar_iso.py`) a partir do APK suportado:
  - Linux: cerca de 20 s; arquivos idênticos aos do build.
  - Kit para Windows sob Wine 9: cerca de 45 s; ELF, `CAMPAIGN.BIN`, `SPRITES.BIN` e `SOUNDS.BIN` idênticos aos do Linux.
  - Fases lidas do bytecode iguais às lidas dos arquivos decompilados (330 de 330); texturas e `CAMPAIGN.BIN` byte a byte iguais aos do processo antigo.
- PCSX2 v2.9.148 (BIOS scph39001, Xvfb):
  - a ISO do build e a ISO gerada pelo kit iniciam, mostram "PROTOTIPO 0.31.0" e listam 330 fases, com som ligado;
  - sem os arquivos de dados, aparece a tela "ARQUIVOS DE DADOS AUSENTES";
  - ISOs de teste abertas na Mighty Hoax 4-1 (troca para o banco 1 em cerca de 3 s) e na Birdday 3-2 (`docs/pcsx2_0.31.0_fase_birdday_3-2.png`).
- O `deathSpawn` foi conferido no computador (`globe_test`): os cremes ficam escondidos e aparecem quando o bolo é destruído. No emulador só a cena foi vista.
- Suíte completa aprovada: 48 verificações; os testes de procedência rodaram com os scripts decompilados locais.
- Campanha: 324 vitórias com roteiros determinísticos; sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6 e Birdday 4-4.
- Pacote de código-fonte conferido numa pasta nova: sem os dados, `make test` pede para gerá-los; depois de `gerar_iso.py APK --dev`, 30 verificações aprovadas (as de procedência ficam puladas sem os scripts decompilados).
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico nem em Windows real (só Wine).
