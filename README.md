# PS2Birds Classic — protótipo 0.62.0

Port não oficial do Angry Birds Classic para PlayStation 2 (homebrew). São 745 fases do
Tutorial e dos episódios Poached Eggs, Mighty Hoax, Danger Above, Big Setup, Ham 'Em High, Mine and Dine,
Birdday 2 a 7, Surf and Turf, Bad Piggies, Short Fuse, Bird Island, Piggy Farm, Dino, Flock Favorites, Golden Eggs e Mighty Feathers (fases clássicas e, em andamento, o modo Egg Defender), com física Box2D, os dez pássaros usados nesses episódios, efeitos sonoros e
recordes no Memory Card. Ainda não é o port completo: veja `docs/LACUNAS_PORT.md`.

**Este repositório não contém nada do jogo original.** Fases, texturas, sons e o ícone do
cartão de memória são convertidos a partir do APK que o jogador possui, pelo Gerador de ISO.

## Gerar a ISO (Windows)

1. Baixe o kit `PS2Birds_Classic_<versão>_Gerador_Windows.zip` e extraia numa pasta.
2. Copie para a pasta o APK original `com.rovio.angrybirds.apk` (a versão suportada é
   conferida pelo SHA-256 que está em `profiles/supported_apk.json`).
3. Execute `GERAR_ISO.bat`. Em menos de um minuto saem:
   - `PS2Birds_Classic.iso` — para o PCSX2, OPL ou gravação;
   - `PS2Birds_Classic_USB\` — `PS2BIRDS.ELF` e os arquivos de dados, para USB/HDD.

O kit traz o próprio Python (pacote oficial para Windows), o ffmpeg (para ler os sons Ogg) e
as ferramentas deste repositório. Nada é instalado e nenhum script do jogo é executado: as
fases são lidas como dados direto do bytecode Lua (`tools/lua51_data.py`).

No Linux/macOS, com Python 3.9+ e ffmpeg:

```sh
python3 tools/gerar_iso.py com.rovio.angrybirds.apk --saida PS2Birds_Classic.iso --pasta USB --elf PS2BIRDS.ELF
```

## Jogar

A ISO tem `PS2BIRDS.ELF` e três arquivos de dados: `CAMPAIGN.BIN` (fases), `SPRITES.BIN`
(texturas e ícone) e `SOUNDS.BIN` (efeitos). Para rodar o ELF de USB/HDD, copie os quatro
arquivos para a mesma pasta. Sem os dados, o jogo mostra uma tela de aviso. Vídeo NTSC
640×448; o console físico ainda não foi validado.

- Menu: cima/baixo escolhem a fase; esquerda/direita mudam três posições; X inicia.
- Partida: direcional/analógico ajustam ângulo (−30° a 85°) e força; X lança; Quadrado
  segurado dá ajuste fino; Triângulo reinicia; Start pausa; Select abre a seleção de fases.
- Habilidades (X depois do lançamento): azul divide em três; amarelo acelera; preto explode;
  branco solta um ovo; verde (bumerangue) faz a curva; GlobeBird infla; Stella solta bolhas.
- Dino: quebrar uma gaiola acrescenta o pássaro preso ao fim da fila.
- Veículos dos porcos (Mighty Feathers): a arma de bolhas atira uma bolha a cada 2,5 s; o sugador puxa as peças à frente, destrói as que encostam nele e prende o pássaro que o acerta (a habilidade do GlobeBird ou do pássaro de choque o destrói).
- Short Fuse: o frasco laranja infla e estoura os porcos próximos; o rosa os prende em bolhas; o azul os clona.
- Tutoriais de power-up: o Estilingue Real lança 30% mais rápido; no Terremoto, L2 sacode a fase (uma vez); na Poção, L2 durante a mira aumenta o próximo pássaro; na Onda de Choque, L2 troca o pássaro do estilingue por um pássaro de choque (uma vez cada).
- L1 salva no Memory Card 1 (a vitória salva sozinha); R3 liga/desliga o som.

## Compilar (desenvolvimento)

Requisitos: ps2dev (PS2SDK, gsKit), make, Python 3.9+, g++ e ffmpeg.

```sh
python3 tools/gerar_iso.py /caminho/com.rovio.angrybirds.apk --dev   # gera ps2/assets e ps2/data
export PS2DEV=/caminho/ps2dev
sh ps2/build.sh -j3          # ELF + arquivos de dados em ps2/build
sh ps2/build.sh test         # testes no PC
sh ps2/build.sh iso          # ISO de desenvolvimento
python3 tools/make_windows_kit.py ps2/build/PS2Birds_Classic.elf Gerador_Windows.zip
```

`ps2/assets` e `ps2/data` são gerados do APK e ficam fora do Git. Os testes de procedência
(que comparam o port com scripts decompilados do jogo) só rodam se o desenvolvedor tiver
esses scripts em `ps2/data`; sem eles, aparecem como pulados.

## Organização

- `ps2/src` — o jogo (motor, física, desenho, som, save). Não embute dados do jogo original.
- `tools` — conversão do APK: `gerar_iso.py` (tudo de uma vez), `lua51_data.py` (fases),
  `extract_sprites.py`/`png_decode.py`/`mini7z.py`/`aes_cbc.py` (texturas),
  `extract_audio.py`/`spu_adpcm.py` (sons), `build_campaign.py`, `pack_sprites.py`,
  `make_iso.py` e `make_windows_kit.py`.
- `profiles` — parâmetros do port: peças, materiais, ordem dos episódios, limites de
  estrelas, tamanhos dos sprites e o APK suportado.
- `ps2/tests` — testes e o buscador de soluções usado para validar as fases.
- `docs` — histórico de modificações, lacunas, validação e dependências.

Angry Birds é marca da Rovio Entertainment. Este projeto não é afiliado à Rovio.
