# Validação 0.25.0

- ELF (8,6 MB), `SPRITES.BIN` (11,1 MB) e ISO compilados com ps2dev a partir de `make clean`.
- PCSX2 v2.9.148 com BIOS scph39001, sob Xvfb:
  - a ISO inicia, mostra "PROTOTIPO 0.25.0" e lista 150 fases;
  - ISO de teste aberta direto na 8-15: o banco 4 é lido do disco e a cena aparece completa (`docs/pcsx2_0.25.0_banco4_do_disco.png`);
  - ELF de teste com o `SPRITES.BIN` na mesma pasta (HostFs ligado): mesma cena completa;
  - ELF sozinho: o jogo abre, mostra o aviso e usa as texturas do banco 0 (`docs/pcsx2_0.25.0_sem_sprites_bin.png`).
- Suíte completa aprovada: 37 verificações.
- Campanha: 148 vitórias com roteiros determinísticos; 3-20 e 5-16 sem roteiro.
- Partidas completas não foram jogadas no emulador. Sem teste em PS2 físico (a leitura do disco num console real ainda não foi conferida).
