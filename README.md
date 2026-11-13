# PS2Birds Classic — protótipo 0.30.0

Foco exclusivo no Classic. Inclui a primeira página completa de Poached Eggs
e a segunda e a terceira páginas completas (63 fases, todo o episódio Poached Eggs) Mighty Hoax, Danger Above, Big Setup, Ham 'Em High, Mine and Dine e Birdday 2 completos (300 fases) na ordem declarada pelo próprio APK, com sprites originais, pássaros
vermelho, azul, amarelo, preto e branco, porcos pequeno/médio/grande/capacete, pedra, madeira,
vidro, pedras circulares, baú, bloco sorridente, bola, pato e morango, seleção de fases, desbloqueio
sequencial e recordes persistentes no Memory Card 1. Ainda não é o port completo.

## Jogar

Abra a ISO no PCSX2 ou use o ELF num carregador homebrew. O ELF precisa do arquivo
`SPRITES.BIN` na mesma pasta (no PCSX2, com a opção HostFs ligada); a ISO já traz os dois.
Sem o `SPRITES.BIN`, o jogo abre e mostra um aviso, e só Poached Eggs tem texturas completas.
O console físico ainda não foi validado. Vídeo NTSC 640×448 em 16 bits com dithering, efeitos sonoros originais e salvamento no Memory Card 1.

- R3 liga/desliga os efeitos sonoros; a preferência vale para a sessão.
- Menu: cima/baixo escolhem a fase, esquerda/direita mudam três posições; X inicia se estiver desbloqueada.
- Partida: direcional/analógico esquerdo ajustam ângulo (−30° a 85°) e força (20 a 100% do esticamento); X lança.
- Pássaro azul: após lançar, pressione X novamente antes de colidir para dividir em três.
- Pássaro amarelo: após lançar, pressione X novamente antes de colidir para acelerar.
- Pássaro preto: X detona durante o voo ou após contato; detona automaticamente cerca de um segundo após contato.
- Pássaro branco: X após lançar e antes de colidir solta um ovo verticalmente; o pássaro recua e o ovo explode ao atingir um corpo ou o chão. Uma vez por lançamento.
- Segure Quadrado para ajuste fino; Triângulo reinicia; Start pausa/continua.
- Select abre/fecha a seleção de fases. O jogo pausa enquanto o menu está aberto.
- Vitória: X avança para a próxima fase; após 4-21 retorna à seleção.
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
OpenSSL e Pillow para os testes de conversão PNG. Instale Pillow com `python -m pip install -r tools/requirements-assets.txt`. A ISO usa genisoimage/mkisofs quando existir ou o pacote
Python `pycdlib` como alternativa. Box2D, audsrv e texturas/efeitos convertidos estão incluídos no
código-fonte; o build não precisa do APK nem de downloads.

## Regenerar dados e gráficos

```sh
python tools/build_campaign.py
python tools/extract_sprites.py /caminho/com.rovio.angrybirds.apk ps2/assets --game classic --sevenzip tools/py7z.py
python tools/build_save_icon.py
python tools/extract_audio.py /caminho/com.rovio.angrybirds.apk ps2/assets --adpenc "$PS2SDK/bin/adpenc"
```

As 84 fontes Lua decompiladas e métricas estão em ps2/data; o importador não
executa Lua. A extração de gráficos exige o APK original e OpenSSL; `tools/py7z.py`
substitui o 7-Zip usando o pacote `py7zr` (um 7z nativo também serve). O banco
de texturas 0 (Poached Eggs) fica incorporado no ELF; os bancos 1 a 14 ficam no `SPRITES.BIN`
e são lidos para um único buffer quando o episódio muda; o
frame buffer passou a 16 bits para abrir espaço na VRAM. As plataformas estáticas
usam máscaras originais com cor provisória; o preenchimento original ainda está pendente.

Os 20 efeitos usam 22050 Hz mono em ADPCM SPU2 e somam cerca de 377 KiB.
FFmpeg e adpenc são necessários apenas para regenerá-los; os dados convertidos
já estão incluídos. O audsrv é compilado das fontes incluídas. Se o módulo não
inicializar, o jogo continua com SOM INDISPONIVEL. Ainda não há música de fundo.

As ferramentas/perfis de outras séries permanecem no pacote, sem novos ports.
Consulte docs/MODIFICACOES_0.30.0.md e docs/VALIDACAO_0.30.0.md. Documentos das versões
anteriores são históricos. Saves da 0.6.0 à 0.8.0 são aproveitados e regravados no
formato versão 3 (288 bytes, até 64 fases). A seleção mostra três fases por página
e muda automaticamente conforme cima/baixo.

Raios, densidades, restituições e fatores de dano direto (azul no vidro 2,9;
amarelo na madeira 2,4) foram consultados nos dados recuperados. O fator de
aceleração do amarelo (1,8×), a abertura do leque do azul (±10°), o cálculo de
dano e as colisões de cenário (caixas) continuam aproximações. Plataformas estáticas que o pássaro já sobrepõe ao nascer ficam sem colisão até a separação.

Faltam as demais páginas e episódios, outros pássaros e habilidades, música e refinamento de áudio,
fidelidade do motor e testes no PS2 físico.

A segunda página inclui Level52, Level34, Level42, Level24, Level88, Level36, Level31, Level21, Level41, Level76, Level38, Level35, Level20 e Level26. O importador lê as tabelas completas de world, incluindo aliases e chaves calculadas; isso recupera objetos omitidos nas versões anteriores. O preto usa raios e parâmetros recuperados, com impulso radial, queda linear e temporização aproximados pelo port, sem oclusão.

WoodBlock7 e LightBlock7 usam colisão circular, conferida nas definições recuperadas; isso também corrige a peça de madeira da fase 1-3.

A física continua em passos de 120 Hz, usando o tempo transcorrido do relógio do PS2SDK (até 100 ms recuperados por quadro). Isso preserva o ritmo da partida e da mira quando uma cena renderiza abaixo de 60 FPS, sem garantir 60 FPS em cenas pesadas.

O save atual é o v6 (2.080 bytes, capacidade de 512 fases). Saves v1 a v5 de versões anteriores são migrados preservando recordes e desbloqueios. Um save de uma versão com menos fases continua válido: se a última fase daquela versão foi vencida, a próxima é liberada. Esta versão contém 300 fases jogáveis.

O branco usa raio/densidade/restituição recuperados. O ovo usa densidade/restituição e valores de explosão de EggGranade; raio inferido da largura do sprite, velocidades de queda/recuo e propagação radial são aproximações do port. Melancias e pneus usam discos sólidos simplificados, com o modelo de materiais existente; pneus não reproduzem a elasticidade/forma interna do original. O atlas do ovo vem de PNG e requer Pillow para regeneração. Há dez canais de efeitos: soltar o ovo e explodir usam canais diferentes.

## Novidades da 0.15.0

A segunda página fica completa com Level66, Level85, Level27, Level32, Level72, Level90 e Level96 (fases 2-15 a 2-21), na ordem de episodes.lua.

- Caixas de TNT (ExtraBlockTNT) explodem ao serem destruídas: raio de impulso 10, força 30000, raio de dano 5 e dano 150, valores recuperados. Uma caixa destruída por outra explosão detona 0,1 s depois (aproximação do port), produzindo reações em cadeia. Uma caixa que apenas cai para fora do mundo não explode.
- Novos objetos com sprites originais: porco de bigode (GrandpaPiglette, raio 2,2), madeira redonda WoodBlock8, pilar, diamante, cálice e banana.
- As máscaras de terreno (GROUND_BLOCK_01 a 05) são guardadas em meia resolução, com alfa filtrado 2×2, e desenhadas em escala 2. Como são formas chapadas tingidas em tempo de execução, isso liberou espaço para 12 sprites novos sem aumentar a VRAM: 144 sprites em três páginas de 512 de largura, os mesmos 2,5 MiB.
- Saves da 0.14.0 (35 fases) são aceitos: vencer a 2-14 libera a 2-15.

Trechos das definições recuperadas estão em `ps2/data/tnt_definitions_excerpt.lua`. Faltam a terceira página de Poached Eggs e os demais episódios.

## Novidades da 0.16.0

O episódio Poached Eggs fica completo: a terceira página traz Level43, Level77, Level28, Level29, Level87, Level18, Level91, Level49, Level45, Level75, Level51, Level30, Level79, Level40, Level59, Level58, Level95, Level82, Level22, Level89 e Level81 (fases 3-1 a 3-21), na ordem de episodes.lua.

- Rei porco (KingPiglette, raio 2,825, resistência 100) na fase final 3-21, com quadros guardados em meia resolução e desenhados em escala 2.
- Novos objetos com sprites originais: escada, pneu médio, dado, ovo roubado, rosquinha e a Estrade03.
- Limite por fase passou de 128 para 160 objetos; a 3-21 tem 135.
- Atlas de 152 sprites em quatro páginas (512×512, 512×512, 512×256, 512×128), 2,75 MiB; sobram 160 KiB de VRAM após os frame buffers.
- Saves da 0.15.0 (42 fases) são aceitos: vencer a 2-21 libera a 3-1.

- Não foi encontrado roteiro vencedor para 3-2, 3-19, 3-20 e 3-21 com a física aproximada do port; elas podem bloquear o avanço sequencial.

Trechos das definições estão em `ps2/data/page3_definitions_excerpt.lua` e a ordem da página em `ps2/data/episode_page3.decompiled.lua`.

## Novidades da 0.17.0

Começa o episódio Mighty Hoax: fases 4-1 a 4-21 (LevelP2_103 a LevelP2_95), na ordem de episodes.lua.

- Blocos novos de madeira, vidro e pedra: quadrado 4×4, prancha 10×1 e dois triângulos com colisão poligonal recuperada (retângulo e isósceles).
- Extras novos: presunto, ovo dourado e bandeira.
- Bancos de textura: as texturas ficam em dois bancos na memória principal (Poached Eggs e Mighty Hoax) e são reenviadas à VRAM quando a fase muda de episódio. A VRAM usada continua 2,75 MiB.
- No banco de Mighty Hoax, o piscar dos porcos e os estados intermediários de dano das peças grandes reutilizam quadros vizinhos.
- Save versão 4 (544 bytes, até 128 fases). Saves v3 da 0.16.0 são lidos; vencer a 3-21 libera a 4-1.

## Correções da 0.18.0

A investigação das fases sem vitória encontrou três erros de importação presentes desde as primeiras versões:

- Chão 5 unidades abaixo do original. O objeto Ground é uma laje de 10 unidades com centro em y=5; a superfície fica em y=0, onde todos os objetos das fases estão apoiados. Antes, o cenário caía 5 unidades ao iniciar e se desarranjava.
- Estilingue no lugar errado. O original posiciona o estilingue sobre o primeiro pássaro da fila (`setPositionByStartObject`), cerca de 7,6 unidades acima dele; o port usava `birdZeroPos`, que é só a posição da fila de espera. Em várias fases o estilingue fica no alto de uma plataforma, e o port lançava do chão.
- Velocidade de lançamento dependente do zoom. O original usa 46,25 unidades/s com o estilingue esticado ao máximo (`defaultForce = -925`, `physicsScale = 1/20`); o port dividia a força pela escala da câmera, de 23 a 152 unidades/s conforme a fase. A força agora é a porcentagem de esticamento.

Com isso, 82 das 84 fases têm vitória encontrada pela busca automática. Continuam sem roteiro 3-9 e 3-19; nelas a vitória depende de quedas e esmagamentos, cuja fórmula de dano fica no código nativo do jogo e não nos scripts Lua recuperados. Não existe regra de desbloqueio extra: o avanço é o do jogo original.

## Novidades da 0.30.0

- Stella (PinkBird): flutua e solta bolhas que levantam as peças, portada do `bubbles.lua` e do `blocks.lua`.
- Episódios Birdday 3 e 4 (30 fases); campanha com 330 fases, 324 com vitória encontrada.
- Peças automáticas usam o tamanho declarado no `blocks.lua` (corrige Mine and Dine e Birdday 2).

## Novidades da 0.29.0

- GlobeBird (pássaro que infla), portado do `puffer_bird.lua` e do `blocks.lua`.
- Episódio Birdday 2 (15 fases) com balões que flutuam (força do material `balloon`), bolos, velas e chapéus de festa.
- Junta "destruir anexo" (tipo 5) corrigida: a peça anexada some quando a outra ponta é destruída.

## Novidades da 0.28.0

- Episódio Mine and Dine (15-1 a 17-15, 45 fases): chapéus de mineiro, pás, picaretas, pedras preciosas, cordas finas e estalactites.
- Nova ferramenta `tools/add_definitions.py`: lê peças do `blocks.lua` e os sprites do APK e as adiciona sem editar o código do jogo.
- Save v6 com 512 posições; peças de densidade 0 fixas, como no original; junta "destruir anexo" (tipo 5).
- 274 das 285 fases com vitória encontrada pela busca automática.

## Novidades da 0.27.0

- Episódio Ham 'Em High completo (12-1 a 14-15, 45 fases), com chapéus de faroeste nos porcos.
- 237 das 240 fases com vitória encontrada pela busca automática.

## Novidades da 0.26.0

- Episódio Big Setup completo (9-1 a 11-15, 45 fases), com o Terence e os porcos de capacete.
- Polígonos livres do `blocks.lua` (capacetes), com a regra de vértices do `ObjectCreation.lua`.
- Escala por peça (`scaleX`/`scaleY`) aplicada como no original.
- 193 das 195 fases com vitória encontrada pela busca automática.

## Novidades da 0.25.0

- Texturas fora do ELF: os bancos 1 a 4 foram para o `SPRITES.BIN` (11,1 MB), lido do disco ou da pasta do ELF. O ELF caiu de 19,7 MB para 8,6 MB, liberando memória para os próximos episódios.
- Investigação da hipótese das peças "dormindo" no início da fase: testada, mantida como opção desligada.
- Levantamento do episódio Big Setup (próximo da campanha).

## Novidades da 0.24.0

- Danger Above 8-1 a 8-15: o episódio Danger Above fica completo (45 fases).
- Peças novas do `blocks.lua`: bola superelástica (ExtraTrampoline, restituição 5,5), tapete (BlockCarpet) e bola de aço (ExtraStrongBall).
- Correção: o chão cobria só x = −100 a 140, e fases com peças além disso (até x = 825 na 7-13) caíam no vazio. Agora cobre ±1000.
- Quinto banco de texturas (banco 4) para a página 8.
- 148 das 150 fases com vitória encontrada pela busca automática.

## Novidades da 0.23.0

- Danger Above 7-1 a 7-15 na ordem do `episodes.lua`, com limites de estrelas originais; as 15 têm roteiro vencedor.
- Peças novas do `blocks.lua`: bloco de terreno frágil (StaticFragileBlock06), bloco de terreno 06 e o balão-porco 01.
- Save v5 com 256 posições; saves v1 a v4 são lidos e migrados.
- Limite de 224 objetos por fase (a 7-2 tem 200).
- 133 das 135 fases com vitória encontrada pela busca automática.

## Novidades da 0.22.0

- Pássaro verde (bumerangue) portado do código Lua original: corpo e parâmetros do `blocks.lua`, algoritmo de voo do `gamelogic.lua`. Antes do toque, voo mais leve; o toque inverte a direção (até 50 u/s) e segura a queda, até o primeiro contato.
- Danger Above 6-1 a 6-15 na ordem do `episodes.lua`, com limites de estrelas originais.
- Peças novas: balões estáticos que estouram (`staticFragile`), trampolim (restituição 1,1) e o enfeite do pássaro verde.
- Quarto banco de texturas (banco 3) para Danger Above, com os quatro estados de dano das peças.
- 118 das 120 fases com vitória encontrada pela busca automática (a página 6 inteira).

## Novidades da 0.21.0

- Segunda página de Mighty Hoax (5-1 a 5-21) na ordem do `episodes.lua`, com limites de estrelas do `starLimits.lua`.
- Três peças novas lidas do `blocks.lua`: LightBlock8 (círculo de gelo pequeno), ExtraBlockFlag01 (bandeira) e ExtraApple (maçã).
- Terceiro banco de texturas (banco 2) para a página 5, na mesma VRAM dos outros.
- 103 das 105 fases com vitória encontrada pela busca automática; 3-20 e 5-16 (ambas só com pássaros explosivos) continuam sem roteiro.

## Novidades da 0.20.0

- Juntas do editor original (distância, solda e dobradiça), com soldas que se rompem sob força excessiva.
- Estrelas por fase com os limites de `starLimits.lua` (prata = 2, ouro = 3), mostradas na vitória e no menu.
- Levantamento do que falta no port em `docs/LACUNAS_PORT.md`.

## Correções da 0.19.0

A primeira execução no PCSX2 mostrou a fase 4-1 desmoronando ao carregar. A investigação encontrou mais duas diferenças em relação ao original:

- Tamanho físico das peças. `ObjectCreation.lua` reduz caixas, círculos e polígonos dinâmicos a 92% do sprite (`sizeFactorX/Y`, padrão 0,92); só objetos estáticos usam o sprite inteiro. O port usava 100%, então as peças nasciam sobrepostas e se empurravam.
- Materiais. Atrito e restituição vêm da tabela `materials` de blocks.lua (madeira 4,0/0,2, vidro 0,7/0,2, pedra 4,0/0,1, estático e chão 0,8/0, porco 0,7/0,05 ou 0,3 quando a definição declara, extras 0,7/0,4). O port usava 0,6/0,03 para tudo e 0,2 no chão.

Com as duas correções, 83 das 84 fases têm vitória encontrada pela busca automática, inclusive 3-9 e 3-19. A 3-20 continua sem roteiro: seus cinco pássaros são explosivos, e a propagação da explosão (queda com a distância e impulso) é uma aproximação do port.
