# Ferramentas e dependências utilizadas

Arquivos fornecidos pelo usuário na pasta Ferramentas do Google Drive:

- `ps2dev-ubuntu-latest.tar.gz`: GCC 15.2.0 para mips64r5900el-ps2-elf,
  PS2SDK, gsKit e ports. SHA-256:
  `c8e5dedccf62084d476894e88cd0451b57f1ed288e741eb3c1263010aaffc028`.
- `PS2 Emulador.zip`: PCSX2 v2.9.148 Linux x64 AppImage e BIOS para teste local.
  SHA-256: `5b0a4f2644ac3da1772facac6d6fb538ffdd693d627c4cd2261753da0c39d850`.

O ELF usa PS2SDK (kernel, SIF, libpad, libc/libm), gsKit e dmaKit. Esses projetos
mantêm suas licenças e créditos próprios:

- https://github.com/ps2dev/ps2sdk
- https://github.com/ps2dev/gsKit
- https://github.com/ps2dev/ps2toolchain
- https://github.com/PCSX2/pcsx2

Preparação no computador: Python, OpenSSL, Java/unluac do ZIP de referência,
7-Zip, GNU make e genisoimage. Xvfb e xdotool foram usados somente para validar a
execução gráfica neste ambiente. A biblioteca Lua incluída no SDK não é vinculada
ao protótipo 0.1.0.

## Box2D adicionado em 0.2.0

- Box2D 2.4.1: https://github.com/erincatto/box2d/tree/v2.4.1
- Fontes include/src e licença MIT em ps2/vendor/box2d.
- Arquivo oficial v2.4.1: SHA-256 d6b4650ff897ee1ead27cf77a5933ea197cbeef6705638dd181adc2e816b23c2.
- Nenhuma alteração nos arquivos da biblioteca; adaptação no núcleo do jogo e no build.

## Salvamento 0.6.0

PS2SDK libmc e módulos MCMAN/MCSERV presentes no ROM do console.
Formato do ícone consultado em https://github.com/ticky/ps2iconsys (MIT);
o gerador é próprio e usa o sprite RedBird do atlas existente.
BIOS, SDK, emulador e cartões de teste não integram o pacote de fontes.

## Ferramentas 0.9.0

- O mesmo `ps2dev-ubuntu-latest.tar.gz` (SHA-256 `c8e5dedc…0aaffc028`) foi obtido da
  release oficial em https://github.com/ps2dev/ps2dev/releases e conferido byte a byte
  com o hash registrado acima.
- `py7zr` (pip) por trás de `tools/py7z.py`, substituindo o executável 7-Zip na extração
  dos streams de sprites. `pycdlib` (pip) como gerador de ISO quando genisoimage não existe.
- `unluac.jar` do ZIP de referência (Java 21) para decompilar os 13 novos Level*.lua; a
  saída para Level8.lua foi comparada com a versão guardada e é idêntica.
- APK Classic SHA-256 `8cdea1ef39a2a5c3bad5ac27050b491dbdf68050754241390cf9f0d3d24979b8`,
  igual ao do diagnóstico inicial.

## Áudio 0.10.0

- PS2SDK audsrv: https://github.com/ps2dev/ps2sdk, fontes EE/IOP incluídas em
  ps2/vendor/audsrv, com GNU Library GPL v2 e avisos AFL v2 preservados.
- Makefile IOP adaptado ao SDK instalado; chamada opcional EE de detecção de
  reboot omitida por compatibilidade (o port não reinicia o IOP).
- O módulo audsrv.irx e o objeto EE são recompilados dessas fontes; não é preciso
  baixar módulo binário. LIBSD é carregado do ROM do console, sem redistribuir BIOS.
- FFmpeg e adpenc do SDK são ferramentas de preparação. As 16 fontes Ogg são do
  APK fornecido; hashes e parâmetros constam em ps2/assets/audio_manifest.json.
- Nenhuma captura, BIOS, cartão de teste, SDK completo ou emulador integra o ZIP.

## Gerador de ISO 0.31.0

- O jogo não embute mais dados do APK; o gerador (`tools/gerar_iso.py`) os converte no
  computador do jogador. Leitores próprios em Python puro substituem as ferramentas
  externas: AES-256-CBC (`tools/aes_cbc.py`, no lugar do OpenSSL), 7z LZMA
  (`tools/mini7z.py`, no lugar do 7-Zip/py7zr), PNG (`tools/png_decode.py`, no lugar do
  Pillow), bytecode Lua 5.1 só de dados (`tools/lua51_data.py`, no lugar do unluac/Java) e
  codificador SPU2 ADPCM (`tools/spu_adpcm.py`, no lugar do adpenc).
- Conferência: as 330 fases lidas do bytecode são idênticas às lidas dos arquivos
  decompilados; texturas e `CAMPAIGN.BIN` saem byte a byte iguais aos do processo antigo.
- Kit para Windows (`tools/make_windows_kit.py`): Python 3.12.10 oficial do NuGet
  (https://www.nuget.org/packages/python, licença PSF), pycdlib 1.22.0 (LGPL, grava a
  ISO) e imageio-ffmpeg 0.8.1 (BSD; traz o executável do ffmpeg, LGPL/GPL, para decodificar
  os sons Ogg). Testado com Wine 9 neste ambiente: mesmos arquivos que no Linux.
- Pillow, OpenSSL e 7-Zip continuam opcionais só para alguns testes de desenvolvimento.
