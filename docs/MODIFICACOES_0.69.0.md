# Modificações completas — PS2Birds Classic 0.69.0

## Música do jogo
- Novo arquivo de dados `MUSIC.BIN`, gerado do APK pelo `tools/extract_audio.py`: PCM 16 bits mono a 22050 Hz, cerca de 3 MB.
- Duas músicas, com os nomes e usos do original (`audio.lua`, `gamelogic.lua` e `level_end.lua`, traduzidos à mão):
  - **`title_theme`** (68 s): toca em repetição enquanto os menus estão abertos. É o `currentMainMenuSong` padrão do original.
  - **`level_complete`** (2,9 s): toca uma vez ao vencer a fase, como no `LevelComplete.onEntry`.
- Novo módulo `ps2/src/music.c` / `music.h`:
  - na inicialização, carrega o `MUSIC.BIN` inteiro na memória, para não ler o disco durante o jogo (no console, ler o CD a cada quadro travaria a imagem);
  - a cada quadro, envia blocos de 2 KB ao audsrv sem bloquear a tela.
- A música para ao entrar numa fase e ao reiniciar depois da vitória.
- R3 (ligar/desligar o som) também liga e desliga a música.
- Sem `MUSIC.BIN`, o jogo funciona normalmente, só que sem música.
- Novo teste `ps2/tests/music_test.cpp`: confere o cabeçalho, o formato (22050 Hz mono 16 bits), a repetição da `title_theme`, a `level_complete` tocando uma vez, o som desligado e o arquivo ausente.
- Conferido no PCSX2: o registro do console mostra "PS2Birds: music loaded" e a 1-1 é jogada e vencida normalmente. O emulador aqui não tem saída de som, então a música não foi ouvida.

## Gerador e documentação
- `gerar_iso.py` grava `MUSIC.BIN` na ISO e na pasta USB/HDD. O README e o `LEIA-ME` do kit foram atualizados: agora são cinco arquivos.

## Limpeza
- Removidos os documentos da 0.68.0 (substituídos pelos da 0.69.0).
- Removidas da área temporária as pastas da 0.68.0, os arquivos de música de teste, a captura e o log do emulador.

## Versão
- Tela inicial e README: 0.69.0.
