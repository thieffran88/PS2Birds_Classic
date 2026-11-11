# PS2Birds Classic — protótipo 0.9.0

Foco exclusivo no Classic. Inclui a primeira página completa de Poached Eggs
(21 fases) na ordem declarada pelo próprio APK, com sprites originais, pássaros
vermelho, azul e amarelo, porcos pequeno/médio/grande/capacete, pedra, madeira,
vidro, pedras circulares, baú e bloco sorridente, seleção de fases, desbloqueio
sequencial e recordes persistentes no Memory Card 1. Ainda não é o port completo.

## Jogar

Abra o ELF ou ISO no PCSX2 ou teste no carregador homebrew escolhido. O console
físico ainda não foi validado. Vídeo NTSC 640×448 em 16 bits com dithering, sem
áudio; salvamento no Memory Card 1.

- Menu: cima/baixo escolhem a fase; X inicia se estiver desbloqueada.
- Partida: direcional/analógico esquerdo ajustam ângulo e força; X lança.
- Pássaro azul: após lançar, pressione X novamente antes de colidir para dividir em três.
- Pássaro amarelo: após lançar, pressione X novamente antes de colidir para acelerar.
- Segure Quadrado para ajuste fino; Triângulo reinicia; Start pausa/continua.
- Select abre/fecha a seleção de fases. O jogo pausa enquanto o menu está aberto.
- Vitória: X avança para a próxima fase; após 1-21 retorna à seleção.
- L1 salva/tenta novamente no Memory Card 1. A vitória salva automaticamente.
- Use cartão PS2 previamente formatado pelo navegador do console; o jogo não formata.
- Aguarde o aviso de salvamento terminar antes de remover o cartão.
- Sem cartão, a partida continua com progresso somente da sessão.

## Ordem das fases

`profiles/classic_episode.json` reproduz a lista da primeira página de Poached
Eggs recuperada de `assets/data/scripts/episodes.lua` do APK: Level1, Level57,
Level53, Level3, Level6, Level2, Level4, Level5, Level7, Level8, Level9, Level13,
Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44.
As versões 0.2.0 a 0.8.0 assumiam que a fase 1-N era LevelN; saves dessas versões
são migrados para as novas posições ao serem lidos (ver `ps2/src/save_mc.c`).

## Compilar

```sh
export PS2DEV=/caminho/ps2dev
sh ps2/build.sh -j3
sh ps2/build.sh -j3 test
sh ps2/build.sh iso
```

Requisitos: pacote ps2dev fornecido (PS2SDK/gsKit), make, Python 3.9+, g++ e
OpenSSL para os testes. A ISO usa genisoimage/mkisofs quando existir ou o pacote
Python `pycdlib` como alternativa. Box2D e texturas convertidas estão incluídos no
código-fonte; o build não precisa do APK nem de downloads.

## Regenerar dados e gráficos

```sh
python tools/build_campaign.py
python tools/extract_sprites.py /caminho/com.rovio.angrybirds.apk ps2/assets --game classic --sevenzip tools/py7z.py
python tools/build_save_icon.py
```

As 21 fontes Lua decompiladas e métricas estão em ps2/data; o importador não
executa Lua. A extração de gráficos exige o APK original e OpenSSL; `tools/py7z.py`
substitui o 7-Zip usando o pacote `py7zr` (um 7z nativo também serve). As três
páginas de atlas usam 2,25 MiB (limite 2,5 MiB) e ficam incorporadas no ELF; o
frame buffer passou a 16 bits para abrir espaço na VRAM. As plataformas estáticas
usam máscaras originais com cor provisória; o preenchimento original ainda está pendente.

As ferramentas/perfis de outras séries permanecem no pacote, sem novos ports.
Consulte docs/MODIFICACOES_0.9.0.md e docs/VALIDACAO_0.9.0.md. Documentos das versões
anteriores são históricos. Saves da 0.6.0 à 0.8.0 são aproveitados e regravados no
formato versão 2 (160 bytes, até 32 fases). A seleção mostra três fases por página
e muda automaticamente conforme cima/baixo.

Raios, densidades, restituições e fatores de dano direto (azul no vidro 2,9;
amarelo na madeira 2,4) foram consultados nos dados recuperados. O fator de
aceleração do amarelo (1,8×), a abertura do leque do azul (±10°), o cálculo de
dano e as colisões de cenário (caixas) continuam aproximações. Plataformas
estáticas que o pássaro já sobrepõe ao nascer no estilingue ficam sem colisão
até a separação, porque algumas rampas são aproximadas por caixas giradas.

Faltam as demais páginas e episódios, outros pássaros e habilidades, áudio,
fidelidade do motor e testes no PS2 físico.
