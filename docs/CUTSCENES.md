# Cutscenes — como funcionam e como acrescentar cenas

O `cutscenes.lua` do APK descreve cada cena como uma lista de ações. O gerador lê esse arquivo
**como dados** (`tools/lua51_data.py`: só constantes, tabelas e contas entre números; nenhuma
função do jogo é executada) e monta cada cena a partir da composição `CUTSCENES.compo.json`.
Nada do jogo fica no repositório.

## O que o script informa (e o gerador lê sozinho)

| Ação do script | Uso |
| --- | --- |
| `setreferencesize` | largura e altura da imagem (a cena é ajustada à altura da tela, `fitheight`) |
| `createsprite` (`background`) | a imagem da cena (uma composição) |
| `playsound` | a faixa de música |
| `scroll` | início, duração e curva (`cubic_in_out`) da rolagem: da esquerda para a direita (`fitheight`) ou de baixo para cima (`fitwidth`) |
| `set_bg_colour` | a cor de fundo atrás das peças |
| `end` | quando a cena termina |

Outras ações (os olhos do Rei Porco, efeitos sonoros avulsos) são ignoradas.

## Cenas incluídas (29)

| Cena no script | Quando toca no port |
| --- | --- |
| `pack1_intro` … `pack8_intro` (menos a `pack3_intro`, abaixo) | ao entrar na primeira fase do episódio sem nenhuma pontuação: Poached Eggs, Mighty Hoax, Big Setup, Ham 'Em High, Mine and Dine, Birdday, Surf and Turf |
| `pack3_intro` | abertura de Danger Above, com rolagem **vertical**: começa embaixo (os pássaros) e sobe até os porcos na cesta de balões |
| `world1_complete` … `world3_complete` | ao sair da vitória que completa Poached Eggs 1, 2 e 3 |
| `world4_complete`, `world5_complete` | Mighty Hoax 4 e 5 |
| `world6_complete` … `world8_complete` | Danger Above 6, 7 e 8 |
| `world9_complete` … `world11_complete` | Big Setup 9, 10 e 11 |
| `world12_complete` … `world14_complete` | Ham 'Em High 12, 13 e 14 |
| `world15_complete` … `world17_complete` | Mine and Dine 15, 16 e 17 |
| `birdday3_complete` | Birdday 3 (pelo nome da cena) |
| `worldS1_complete` … `worldS3_complete` | Surf and Turf 1, 2 e 3 |

X ou Start pulam a cena. A música é a da cena (`birds_intro`, `birds_boss` ou `birds_outro`).
A tabela de gatilhos está em `ps2/src/main.c` (`episode_scene` e `world_scene`); uma cena que não
está no `CUTSCENE.BIN` é simplesmente pulada.

## O que ficou de fora

- **Egg Defender** (`rmf_classic_intro`, `rmf_complete`, `eggdefender_intro`, `eggdefender_complete`): painéis em grade com animações de escala e posição.
- **Short Fuse** (`blackbird_*`), **Piggy Farm** (`*_farm`) e **Dino** (`*_dino`): as imagens estão em WebP (e a saída do Dino nem é uma composição), e o gerador, que roda só com Python puro no Windows, não decodifica esse formato. Também não estão ligadas a fases.
- **Birdday** `world18_complete` e `world19_complete`, e `pack9_intro` com `world20` a `world22`: não dá para saber a que episódio ou mundo do port correspondem, então não têm gatilho.
- **Animações dentro da cena:** os olhos do Rei Porco abrindo e o "oink" no fim das cenas de encerramento.
- **Largura:** o script às vezes declara a imagem um pouco mais larga do que o desenho cobre. O gerador usa a largura real, para não rolar uma faixa vazia para dentro da tela.

## Como acrescentar uma cena

1. Acrescente o nome dela na lista `SCENES` de `tools/extract_cutscene.py`. Imagem, tamanho,
   música, rolagem e fim vêm do próprio script; a faixa de música precisa estar na lista `MUSIC`
   de `tools/extract_audio.py` (acrescente uma linha se não estiver).
2. Ligue a cena a um momento do jogo na tabela `episode_scene` ou `world_scene` de
   `ps2/src/main.c`.
3. Gere os dados de novo (`GERAR_ISO.bat`, ou `python3 tools/gerar_iso.py APK --dev` no
   desenvolvimento).

## Detalhes técnicos

- Cada cena fica na RAM (cerca de 1 a 3 MB) só enquanto toca, e não ocupa memória de vídeo: o
  jogo copia as 640 colunas visíveis para o quadro a cada quadro, como nos fundos das fases.
- Formato do `CUTSCENE.BIN`: "PBCS", versão 5, número de cenas e, por cena (72 bytes), o nome (24 bytes),
  largura, altura, início da rolagem (ms), duração da rolagem (ms), fim (ms), posição dos dados,
  faixa de música, opções (bit 0: rolagem vertical; bit 1: começa pelo fim), destino da primeira rolagem
  (milésimos do percurso total) e, para cenas com duas rolagens, início, duração e destino da segunda; depois as imagens em
  RGBA5551. Uma cena horizontal tem 448 linhas e pelo menos 640 de largura; uma vertical tem 640 de largura.
- A transparência dos sprites do APK vem na escala do PS2 (0 a 128) e é convertida na composição.

## Cenas acrescentadas na 0.93.4 e 0.94.2

- Short Fuse (blackbird_intro, middle_1, middle_2, end), Bad Piggies (pack9_intro e world20/21/22_complete),
  Piggy Farm e Dino (abertura e final), Birdday (world18/19_complete), Egg Defender e Mighty Feathers
  (abertura e final de cada). As folhas vêm em WebP, lido pelo decodificador próprio (`tools/webp.py`, com `vp8l.py`
  para o modo sem perdas). Com isso estão no Port as 47 cenas do roteiro (`cutscenes.lua`).
- **Painéis animados** (Egg Defender, Mighty Feathers; opção bit 2 do `CUTSCENE.BIN`): cada painel é uma camada com
  entrada por escala e posição (cubicOut ou bounceOut) e, na abertura do Mighty Feathers, o desvanecer dos painéis;
  o jogo compõe as camadas por software a cada quadro, sobre o preto, ajustadas à tela como o `fittogrid` do original.
- **Piscar do Rei Porco** (opção bit 3): os olhos trocados nas cenas de fim de mundo (Poached Eggs 3, Mighty Hoax 5,
  Danger Above, Big Setup, Ham Em High, Mine and Dine, Birdday e Surf and Turf) são sprites sobre a imagem, trocados
  nos tempos do roteiro. No Mighty Hoax 5 o rei vivo fica fora da imagem, e o olho também (como no original).
- Rolagens parciais e duplas (pack4_intro, pack8_intro, blackbird_intro) seguem o destino do original.
- Gatilhos: a abertura de cada episódio na primeira fase, o final ao fechar o mundo; Birdday 7 tem abertura própria;
  Mighty Feathers e Egg Defender abrem na primeira fase e fecham ao completar as 15 fases.
- O brilho da árvore (blackbird_intro) usa o sprite `BACKGROUND_TREE_GLOW`, que **não existe** no APK: o original
  também não mostra nada ali.
- Dino: `useReferenceSize` foi lido como "o destino é uma fração da imagem ajustada à altura"; na abertura a rolagem
  vai até 75% da imagem; no final (destino x 0,5) a imagem cabe quase toda na tela e fica parada. É a leitura mais
  provável, não confirmada contra o original.

## Aberturas (0.94.2)

Antes do menu: `splash_rovio` (3 s) e `splash_loading` (2 s), no mesmo `CUTSCENE.BIN`, como imagens paradas. X ou Start pulam.
