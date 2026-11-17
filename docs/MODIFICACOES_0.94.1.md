# Modificações — 0.94.1

Versão das **telas de abertura**: ao ligar, o jogo passa pelas duas telas que o Classic mostra antes do menu.

## Adicionado
- **Tela da Rovio** (3 s): fundo claro, logo da Rovio no centro e linha de direitos autorais embaixo (`splashes.lua`, `RovioSplash`).
- **Tela de carregamento do Angry Birds** (2 s): arte do jogo cobrindo a tela e o texto "Carregando..." embaixo (`AngryBirdsSplash`).
- X ou Start pulam cada tela. As imagens são extraídas do APK do jogador (`splash_rovio` e `splash_loading` dentro do `CUTSCENE.BIN`).

## Editado
- `tools/extract_cutscene.py` (monta as duas telas), `ps2/src/main.c` (sequência na inicialização), `docs/CUTSCENES.md`, README e versão 0.94.1.

## Excluído
- Da pasta do Drive e do repositório: ISO, ELF, gerador, código-fonte e documentos da 0.94.0 (substituídos).

## Observações
- A terceira abertura do script (Chillingo/Clickgamer) não é usada pelo jogo e a imagem não existe no APK: não foi feita.
- O tamanho do logo foi ajustado à tela do PS2 (o original usa 70% pelo layout de 1024x768); há uma tela preta curta enquanto o disco carrega a primeira imagem.
- Os créditos já existiam (menu principal); não há créditos rolando antes do menu.
- Testado: extração, `make test` e a ISO no emulador (a tela da Rovio aparece na inicialização).
