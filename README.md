# PS2Birds Classic — protótipo 0.94.4

Port não oficial do Angry Birds Classic para PlayStation 2 (homebrew). São 745 fases do
Tutorial e dos episódios Poached Eggs, Mighty Hoax, Danger Above, Big Setup, Ham 'Em High, Mine and Dine,
Birdday 2 a 7, Surf and Turf, Bad Piggies, Short Fuse, Bird Island, Piggy Farm, Dino, Flock Favorites, Golden Eggs e Mighty Feathers (fases clássicas e, em andamento, o modo Egg Defender), com física Box2D, os dez pássaros usados nesses episódios, efeitos sonoros e
recordes no Memory Card. Ainda não é o port completo: veja `docs/LACUNAS_PORT.md`.

**Este repositório não contém os arquivos do jogo original** (imagens, sons, fases, fontes). Eles
são convertidos, na hora, a partir do APK que o próprio jogador possui, pelo Gerador de ISO.
O repositório contém código próprio, tabelas de parâmetros e lógica reescrita para trabalhar
com o APK do jogador (por exemplo, `profiles/`).

> **Aviso legal.** Projeto de fã, não oficial, sem vínculo com a Rovio Entertainment nem com a
> Sega. *Angry Birds* e os personagens são marcas e obras dos seus donos. Nada daqui dá direito
> sobre o jogo original: para usar, é preciso ter o APK de forma legítima.

## Gerar a ISO

Escolha o seu caso:

- **Windows:** veja abaixo.
- **Android (sem computador):** veja o passo a passo em [`docs/GERADOR_ANDROID.md`](docs/GERADOR_ANDROID.md).
- **Linux/macOS:** veja o fim desta seção.

**Angry Birds Rio (experimental):** o mesmo kit gera uma ISO do Rio (opção 2 do menu, APK `com.rovio.angrybirdsrio.apk`): primeiro episódio e parte da selva, sem menus próprios ainda. No Linux: `python3 tools/gerar_iso.py com.rovio.angrybirdsrio.apk --jogo rio --elf PS2Birds_Rio.elf` (o ELF do Rio sai de `make GAME=rio`). Detalhes em `docs/rio/`.

### Windows

1. Baixe o kit `PS2Birds_Classic_<versão>_Gerador_Windows.zip` e extraia numa pasta.
2. Copie para a pasta o APK original `com.rovio.angrybirds.apk` (a versão suportada é
   conferida pelo SHA-256 que está em `profiles/supported_apk.json`).
3. Execute `GERAR_ISO.bat`, escolha o jogo e acompanhe pela página que abre no navegador.
   Em alguns minutos saem:
   - `SLUS_000.10.PS2Birds_Classic.iso` — para o PCSX2, OPL ou gravação (o começo do nome é o
     ID do disco, que o OPL exige);
   - `apps\PS2Birds\` — `PS2BIRDS.ELF`, os arquivos de dados e um `title.cfg`, para USB/HDD
     (copie a pasta `apps` para a raiz).

O kit traz o próprio Python (pacote oficial para Windows), o ffmpeg (para ler os sons Ogg) e
as ferramentas deste repositório. Nada é instalado e nenhum script do jogo é executado: as
fases são lidas como dados direto do bytecode Lua (`tools/lua51_data.py`).

### Linux/macOS

Com Python 3.9+, `pycdlib` e ffmpeg:

```sh
python3 tools/gerar_iso.py com.rovio.angrybirds.apk --elf PS2BIRDS.ELF
```

Gera `SLUS_000.10.PS2Birds_Classic.iso` e a pasta `apps/PS2Birds` na pasta atual
(`--painel` abre o acompanhamento no navegador; `--pasta`, `--saida` e `--id` trocam os padrões).

## Jogar

A ISO tem `PS2BIRDS.ELF` e nove arquivos de dados: `CAMPAIGN.BIN` (fases), `SPRITES.BIN`
(texturas e ícone), `SOUNDS.BIN` (efeitos), `MUSIC.BIN` (músicas), `UI.BIN` (interface), `FONT.BIN` (fontes), `MENU.BIN` (menus), `THEMES.BIN` (fundos das fases) e `CUTSCENE.BIN` (cutscenes em quadrinhos). Para rodar o ELF de USB/HDD, copie os dez
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
python3 tools/make_android_kit.py ps2/build/PS2Birds_Classic.elf Gerador_Android.zip
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

## Licença

O código deste repositório (`tools/`, `ps2/src`, `ps2/tests`, `docs/`) está sob a **licença MIT**
(arquivo `LICENSE`, © 2026 ThiefFran). A licença cobre só o código próprio: ela não dá nenhum
direito sobre o jogo original nem sobre o conteúdo da Rovio.

### Licenças de terceiros

- **Box2D** (`ps2/vendor/box2d`): MIT, © Erin Catto.
- **audsrv** (`ps2/vendor/audsrv`): do PS2SDK, com as licenças que acompanham a pasta (`COPYING.LIB`, `LICENSE-AFL-2.0.txt`).
- **PS2SDK e gsKit**: usados só para compilar; não são distribuídos aqui.
- **Kit do Windows** (anexo das Releases, não faz parte do repositório): traz o Python para Windows e o ffmpeg (pelo pacote `imageio-ffmpeg`), cada um com a licença própria do projeto de origem.

