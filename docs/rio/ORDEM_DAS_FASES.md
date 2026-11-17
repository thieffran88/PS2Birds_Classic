# Rio — ordem das fases e primeira fase alvo

## Ordem das fases (resolvida)

A tabela `levelOrder` é montada em código dentro de `gamelogic.lua`, então não pode ser lida como dados. As constantes de texto
da função que a monta listam os nomes das fases de cada pacote, na ordem da tabela. `tools/rio/level_order.py` extrai essa lista
e confere cada nome contra as pastas de `assets/data/levels`.

- **43 de 44 pacotes conferem exatamente**: cada pacote sai como uma sequência com todos os seus arquivos.
- **Exceção:** `plane2` sai com 15 de 16 fases. Falta `Level201`, que é a mesma fase já marcada na investigação como especial (Nigel_3, sem tabela de estrelas).
- Episódio 1 (Smugglers' Den): `warehouse` e `warehouse2`, 15 fases cada.
  - `warehouse`: Level190, 191, 192, 193, 194, 195, 196, 197, 198, LevelB1, Level64, 199, 38, 75, 135.
  - `warehouse2`: Level68, LevelB2, 73, 81, 80, 134, 76, 77, 78, LevelY2, 66, 82, 35, 107, 147.
- Para rodar: `python3 tools/gerar_iso.py --jogo rio --investigar APK` e depois `python3 tools/rio/level_order.py` (saída em `work/rio/analysis/level_order.json`, fora do Git).

## Primeira fase alvo: warehouse / Level190

- 4 pássaros `RedBird` na fila, tema `theme2`, `physicsToWorld` = 20.
- 30 objetos no mundo: caixas de papelão, caixas de metal, caixotes, madeira, concreto, chão e **5 gaiolas** (`BLOCK_CAGE_2`, `_8`, `_12`, `_15`, `_16`), todas com `levelGoal`.
- Nenhum porco. A vitória vem de libertar as gaiolas, o que confirma que o contador de porcos do Classic precisa virar um objetivo por fase.
- Câmera por plataforma (`windows`, `iphone`, `ipad`); a que serve ao port é a `iphone` (480x320, escala 0,4).

## Próximo passo

1. Importar as 30 definições da fase (`import_level.py` com perfil Rio), incluindo materiais e as 5 gaiolas.
2. Converter os sprites mínimos (caixas, gaiolas, pássaro vermelho, fundo do `theme2`) e montar `SPRITES.BIN` do Rio.
3. Generalizar o objetivo de vitória no runtime e gravar saves em `/PS2BRIO`.

## O que a Level190 precisa em texturas (investigado)

A fase usa 23 sprites (pássaro vermelho, caixas, caixotes, madeira, concreto, as gaiolas e os passarinhos presos). Resultado:

| Folha | Onde está | Formato | Dá para ler só com Python? |
| --- | --- | --- | --- |
| `INGAME_COMMON_BIRDS_1` (pássaro vermelho) | `ingame_assets` | PNG | sim |
| `INGAME_RIO1_BIRD_1` (pássaro vermelho, outra folha) | `ingame_assets` e `_sd` | PVR 1024x1002 e 509x508 | sim: RGBA4444 sem compressão |
| `INGAME_RIO1_BLOCKS_MISC_1` (caixas, caixotes) | `ingame_assets_sd` | PVR 439x410, RGBA4444 | sim |
| `INGAME_RIO1_BLOCKS_MISC_2` (gaiolas, passarinhos) | `ingame_assets_sd` | PVR 545x510, RGBA4444 | sim |
| `INGAME_COMMON_BLOCKS_MISC_1` (caixa de metal) | `ingame_assets` e `_sd` | **WebP (VP8X)**, nas duas resoluções | **não**: precisa de decodificador WebP |

Conclusões:
- A folha `_sd` do Rio é PVR RGBA4444, que se lê sem biblioteca. Isso cobre quase tudo da primeira fase.
- A única dependência de WebP é a **caixa de metal** (`BLOCK_METAL_BOX_1/2`) e a madeira `BLOCK_WOOD_10X1_1` (que também existe em PVR `INGAME_RIO1_BLOCKS_WOOD_1`). A caixa de metal é o que força decidir sobre WebP.
- Os nomes `B_CAGE_2/8/12/15` não são sprites de folha: são composições (`COMP`) das gaiolas, montadas a partir de `BLOCK_CAGE_*` e do passarinho preso. O leitor COMP já os lê estruturalmente, mas a montagem ainda falta.
- Dois objetos decorativos (`DECO_CHAINS`, `WAREHOUSE_PROPELLER_1`) não estão no cruzamento das 81 definições: são do tema, e podem ficar de fora da primeira versão.

## Origem do lançamento e gravidade (investigado)

- **Origem do lançamento:** as fases do Rio não têm `birdZeroPos`. Os pássaros da fila são objetos do próprio mundo, com `startNumber` de 1 em diante. O de número 1 fica mais alto (Level190: x=108,8 y=-9,46) e é o que está no estilingue; os outros esperam no chão atrás dele (y≈-5,4, x decrescente). Nas três fases conferidas (Level190, 191 e 68) o padrão se repete. Portanto a origem vem do pássaro 1 de cada fase, na escala `physicsToWorld` (20 nas fases vistas).
- **Gravidade:** não aparece em nenhum script Lua nem nas tabelas lidas (`gravity` só ocorre em `bubbleAntiGravity` e afins, dos power-ups). No código nativo (`libAngryBirdsRio.so`) as únicas ocorrências são `gravityX` e `gravityY`, que podem ser do acelerômetro. Em aberto: o valor do mundo precisa ser obtido do binário nativo ou calibrado por trajetória medida, nunca copiado do Classic sem conferir.

## Objetivos de vitória (`levelgoals.lua`, investigado)

A tabela traz 62 definições com `levelGoal`. Todas têm forma de caixa (exceto Luiz, polígono), material próprio e `strength` (resistência):

| Tipo | Definições | Resistência | Pontos |
| --- | --- | --- | --- |
| Gaiolas | `BLOCK_CAGE_1..16`, `R2_BLOCK_CAGE_1..16`, `B_CAGE_BLU_JEWEL` | 10 (a nº 5: 15) | `SCORE_5K_CAGE` |
| Macacos | 11 variantes `R2_Monkey*` e 8 `Monkey*` (em pé, na parede, pendurado; médio, "speedier", invertido) | 10 a 16 | `SCORE_5K_MONKEY` |
| Redes de golfinho e jaula do Nigel | `DOLPHIN_NET_1`, `NIGEL_CAGE` | 10 | `SCORE_5K_CAGE` |
| Chefes | `MonkeyBoss*` (500 a 1000), `BIRD_RIO_NIGEL*` (600, 1000, 1) e `_BOAT` (10000) | 1 a 10000 | — |
| Luiz | `Luiz` (polígono, sem resistência) | — | — |

Consequência para o runtime: gaiolas e macacos se comportam, para a contagem de vitória, como os porcos do Classic — uma peça com resistência e pontos que, ao ser destruída, diminui o número de alvos restantes. A primeira versão pode reaproveitar o contador de porcos (`pig_alive`) com um tipo de alvo "gaiola", trocando apenas sprite, material e som; as animações e estados dos macacos, o Luiz e os chefes entram depois.

## O runtime já aceita o alvo "gaiola" (conferido no código)

- O formato da fase em `CAMPAIGN.BIN` já traz, por fase, `gravity`, `sling_x`, `sling_y`, `ground_y` e `physics_to_world` (veja `tools/build_campaign.py`). O gerador do Rio só precisa preenchê-los: a origem do lançamento vem do pássaro 1 da fase; a gravidade continua em aberto.
- A vitória conta peças de tipo 4 (`kind==4`): `pig_alive` sobe na carga da fase e desce em `game.cpp` (linhas ~1168 e ~1261) quando a peça é destruída, com 5000 pontos. Fora do modo Egg Defender toda peça tipo 4 é alvo (`is_goal`).
- Portanto uma gaiola ou um macaco do Rio pode entrar como peça tipo 4, com a resistência e o material do `levelgoals.lua`. **Para a primeira fase não é preciso alterar o runtime**: basta o gerador do Rio emitir as gaiolas como tipo 4. O que muda é só a aparência (sprite e efeito de saída do passarinho) e, depois, o comportamento dos macacos.

## Importador de fases (`tools/rio/rio_import_level.py`)

Converte uma fase do Rio na lista de objetos que o `build_campaign` grava no `CAMPAIGN.BIN`: tamanho pelo sprite (ou pela largura e altura da definição), escala da instância, material e resistência pelo catálogo, estilingue no pássaro 1 e gravidade como parâmetro (`--gravity`, padrão -10, **só um marcador**: o valor real ainda não foi obtido).

Resultado na `warehouse/Level190`: 21 dos 25 objetos convertem (20 de material comum e 1 gaiola), mais 4 pássaros e o estilingue em (108,81; -9,46). As 4 gaiolas restantes (`BLOCK_CAGE_2`, `_8`, `_12`, `_15`) não têm largura e altura na definição: são composições (`B_CAGE_*`), e o tamanho tem de vir da extensão da composição nos arquivos COMP, que o leitor já lê mas ainda não mede. A saída fica em `work/rio/` (fora do Git).

### Atualização: composições medidas

O corpo de uma gaiola composta é o último filho da composição (deslocamento 0); o passarinho preso é o filho anterior. O importador usa o tamanho desse sprite: nas gaiolas pequenas 83x83 px, ou seja, 83/20 x 0,92 = 3,82 m, praticamente os 3,81 m que o `levelgoals.lua` declara para as gaiolas equivalentes `R2_*`. Com isso a `warehouse/Level190` converte **25 de 25 objetos** (20 de material comum e 5 gaiolas como alvos tipo 4), e **as 459 fases do Rio convertem sem nenhuma definição sem adaptador**. Isso só mostra que formas e tamanhos estão resolvidos; não valida física, sprites nem objetivos no PS2.

## Primeiro `CAMPAIGN.BIN` do Rio (`tools/rio/rio_campaign.py`)

`python3 tools/rio/rio_campaign.py SAIDA.BIN FASE.lua [...]` converte fases com o importador e grava com o mesmo escritor do Classic (`build_campaign.write_binary`, formato "PBCP" v9). Com a `warehouse/Level190` o arquivo sai com 1 fase, 25 objetos e 4 pássaros, e volta a ser lido sem erro pelo `read_binary`.

- Os arquivos do Classic guardam gravidade **+20** com `physicsToWorld` 20 (eixo y para baixo). O gravador do Rio usa o mesmo +20 como valor provisório; **ele não foi conferido no APK do Rio**.
- Pássaros: só o vermelho (tipo 0) tem equivalente no runtime. As fases com outros pássaros do Rio param com erro explícito.
- **Ainda não foi carregado pelo runtime** (nem no teste em PC nem no PCSX2): só a escrita e a releitura em Python foram conferidas. O arquivo fica em `work/rio/`, fora do Git e fora da ISO.
- Nomes: os scripts do Rio se chamam `rio_import_level.py` e `rio_campaign.py` para não colidir com `import_level.py` e `build_campaign.py` do Classic.

### Atualização: o runtime carrega a fase do Rio (teste em PC)

O `ps2/tests/rio_smoke.cpp` carregou o `CAMPAIGN.BIN` do Rio no `game.cpp` do Classic, sem alterá-lo: 1 fase, 25 objetos, 4 pássaros, **5 alvos tipo 4 (as gaiolas) e 20 outras peças**. Após assentar 2 s de simulação nada foi destruído e a pontuação seguiu zerada. Um tiro de teste (ângulo 0,7, força 0,8) libertou 1 gaiola: alvos 5 -> 4 e 5000 pontos, sem valores inválidos.

Isso confirma que o contador de alvos do Classic serve às gaiolas e que o formato carrega. **Não valida** a gravidade (+20 provisória), a fidelidade da física, nem sprites ou som; ainda não foi rodado no PCSX2.

## Save do Rio em `/PS2BRIO`

`ps2/src/save_mc.c` agora lê o diretório do cartão de memória da macro `PS2BIRDS_SAVE_DIR` (padrão `/PS2BIRDS`, o do Classic). Compilar com `-DPS2BIRDS_SAVE_DIR='"/PS2BRIO"'` faz as duas cópias do progresso, o `icon.sys` e o `save.icn` irem para `/PS2BRIO`, sem compartilhar progresso com o Classic. Conferido: `make save-test` passa com o padrão, e a compilação com `/PS2BRIO` produz só os caminhos do Rio.

Com o padrão o comportamento do Classic é o mesmo, mas o ELF recompilado difere do da 0.93.0 em 109 bytes (organização das constantes de texto no binário; o ELF da 0.93.0 foi reproduzido byte a byte com o código anterior). Por isso a mudança segue só no código-fonte: o ELF e a ISO do Classic entram no próximo fechamento do Classic, sem uma versão nova só para isto. A identidade do codec (`CLAS`) ainda precisa ser separada.

### Atualização: identidade do codec separada

`ps2/src/save_codec.c` guarda em todo save a identidade do jogo na macro `PS2BIRDS_SAVE_ID` (padrão `0x434c4153`, "CLAS", a do Classic; o valor dos arquivos já gravados não muda). O Rio compila com `-DPS2BIRDS_SAVE_ID=0x52494f31u` ("RIO1"). O `make save-test` passou a rodar `tests/save_id_test.c` com as duas identidades: cada uma lê o próprio arquivo e recusa o do outro jogo. Com o padrão o código gerado para o Classic é o mesmo (só a constante, de mesmo valor).

Com isso o save do Rio tem pasta (`/PS2BRIO`) e identidade próprias. Resta ligar essas duas macros ao build do Rio, quando houver um.

### Atualização: gravidade, o que foi procurado

Procurei `gravity` em todos os scripts recuperados, nos arquivos de dados e de configuração do APK e no binário nativo: nos scripts só aparece em nomes de power-ups (`bubbleAntiGravity`, `antiGravityMaxVelocity`); nos demais arquivos só em layouts de interface do Android; no nativo, só `gravityX` e `gravityY`. **O valor do mundo não está em nenhum dado legível do Rio.** O que se sabe: as fases do Rio e as do Classic usam o mesmo `physicsToWorld` (20) e o runtime aplica `b2World(0, gravidade)` com y para baixo. Por isso +20 continua sendo o valor provisório mais plausível, mas só uma medição no jogo original (trajetória de um tiro) ou a leitura do código nativo o confirma.

## Decisão: o Rio segue o mesmo esquema do Classic

Decidido com o autor do projeto: o Rio funciona como o Classic.
- **O repositório não leva dado do jogo.** Quem gera a ISO usa o próprio APK do Rio; o gerador (`tools/gerar_iso.py --jogo rio`) valida o APK pelo SHA-256 e cria os bancos de dados (campanha, sprites, sons, música, interface) a partir dele.
- **Só o executável (ELF) e o código-fonte são distribuídos**, sem textura, som ou fase do jogo original.
- **A gravidade fica em +20** (o valor dos arquivos do Classic), sem medição no jogo original, porque não há vídeo de referência; se alguém encontrar diferença na jogabilidade, é um parâmetro único (`--gravity`) para ajustar.
- **Build:** `make GAME=rio` gera `build/PS2Birds_Rio.elf` com a pasta de save `/PS2BRIO` e a identidade `RIO1`; o padrão (`GAME=classic`) segue igual. Conferido: o ELF do Rio compila e só tem caminhos `/PS2BRIO`.

Falta, para o Rio ter ISO: os bancos de sprites do Rio no gerador (a lista de sprites compilada em `ps2/src/sprite_ids.h` hoje é a do Classic; o Rio precisará da própria), a ligação dos demais bancos e a verificação no PCSX2.

## Banco de sprites do primeiro episódio (`tools/rio/extract_sprites_rio.py`)

`python3 tools/rio/extract_sprites_rio.py APK SAIDA [--levels warehouse warehouse2 ...]` monta o banco de sprites a partir do APK do usuário: lista as peças usadas pelas fases pedidas, pega os sprites folha (o corpo e o passarinho preso, no caso das gaiolas compostas), recorta cada um nas folhas PVR RGBA4444 (ou PNG) e empacota com o mesmo empacotador do Classic, gravando `sprites.rgba` e `manifest.json` em `work/rio/` (fora do Git).

Resultado para Smugglers' Den (`warehouse` e `warehouse2`, 30 fases): 81 definições, 74 sprites pedidos, **68 empacotados em 1 página (1 MiB de VRAM, de 2,75 MiB disponíveis)**. Seis ficaram de fora e são listados: `BLOCK_METAL_BOX_1/2/3`, `BLOCK_CHAINS` e `B_FRUIT_PINEAPPLE` (só existem em WebP ou ETC1) e `EMPTY_1X1` (peça sem imagem). Sem as caixas de metal, as fases que as usam ficam sem esse desenho até haver decodificador de WebP.

Falta: o `sprite_ids.h` do Rio (lista compilada no runtime), o formato "PBSP" do `SPRITES.BIN` e escolher como o runtime associa cada peça do Rio ao seu sprite.

## `SPRITES.BIN` e `sprite_ids.h` do Rio (`tools/rio/pack_sprites_rio.py`)

`python3 tools/rio/pack_sprites_rio.py BANCO SPRITES.BIN sprite_ids.h` grava, a partir do banco de `extract_sprites_rio.py`, o `SPRITES.BIN` no formato "PBSP" v2 que o runtime já lê (1 banco, 1 página, 68 sprites, 1.049.600 bytes, sem ícone de cartão por enquanto) e o `ps2/src/rio/sprite_ids.h`, que guarda só nomes e tamanhos (como o do Classic; nenhum pixel). A ordem é a do Classic (mais altos primeiro, depois o nome). Conferido: o cabeçalho tem "PBSP", versão 2 e 68 sprites, e o `sprite_ids.h` compila.

Pendência: o runtime inclui `"sprite_ids.h"` a partir de `ps2/src`, e uma inclusão entre aspas procura primeiro na pasta do próprio arquivo, então `-I` não troca o cabeçalho do Classic pelo do Rio. A variante `GAME=rio` terá de usar um nome de cabeçalho selecionado por macro (por exemplo `PS2BIRDS_SPRITE_IDS`) antes de gerar um ELF do Rio com esta lista.

### Atualização: lista de sprites escolhida por macro

`ps2/src/sprites.h` inclui a lista de sprites pela macro `PS2BIRDS_SPRITE_IDS` (padrão `"sprite_ids.h"`, a do Classic). `make GAME=rio` passa `-DPS2BIRDS_SPRITE_IDS='"rio/sprite_ids.h"'`, que é a lista do Rio, e continua com a pasta de save e a identidade próprias. É o único ponto do runtime que inclui a lista, então o Classic não muda.

O código de desenho (`main.c`, `menu.c` etc.) ainda usa os nomes de sprite do Classic (`SPR_PIG_...`, `SPR_BIRD_...`), que a lista do Rio não tem: o ELF do Rio **ainda não compila** por esse motivo, e ligar cada peça do Rio ao seu sprite é o próximo trabalho do runtime.

### Atualização: tabela peça-sprite do Rio

`tools/rio/pack_sprites_rio.py` também grava `ps2/src/rio/definition_sprites.h`, no formato da tabela do Classic (`definition_sprites.h`): 87 peças do primeiro episódio com o sprite do banco (nas gaiolas compostas, o corpo; o passarinho preso ainda não é desenhado). Peças cujo sprite só existe em WebP ficam fora e não são desenhadas. `main.c` inclui a tabela pela macro `PS2BIRDS_DEFINITION_SPRITES` (padrão `"definition_sprites.h"`), e `make GAME=rio` aponta para a do Rio.

Medido: com as duas listas do Rio, o `main.c` ainda dá **217 erros de compilação**, todos por nomes de sprite do Classic (`SPR_BIRD_*`, `SPR_PIG_*`, interface e efeitos) usados diretamente no desenho dos pássaros, porcos, fundo e interface. O desenho das peças do cenário já passa pela tabela; o que resta é a interface e o desenho do pássaro.

## Inventário do desenho que ainda depende do Classic (`ps2/src/main.c`)

Para compilar o ELF do Rio, os 210 nomes de sprite do Classic que faltam na lista do Rio se concentram em poucos blocos do `main.c`:

| Trecho (linhas) | Função | Nomes | O que o Rio faz |
| --- | --- | --- | --- |
| 173-180 | `bird()`: sprites dos pássaros (inclui o globo, o rosa e o azul-cinza) | 23 `SPR_BIRD_*` | usar `BIRD_RED_NORMAL` do banco (só o vermelho existe no runtime) |
| 182-199 | `pig()`: porcos do Classic | 17 `SPR_PIG_*` | o Rio não tem porcos: alvos são gaiolas, desenhadas pela tabela peça-sprite |
| 201-243 | tabela `block_sprites` (madeira, vidro, pedra por estado de dano) | 147 `SPR_BLOCK_*` | as peças do Rio vêm da tabela `definition_sprites.h` (um sprite por peça); falta decidir se há sprites de dano |
| 245-282 | máscaras de terreno e peças fixas, balões, TNT, extras | `SPR_GROUND_BLOCK_*`, `SPR_ESTRADE_*`, `SPR_HELMET_*` | terreno do Rio ainda não analisado |
| 590 e 669 | estilingue (`SPR_SLING_SHOT_01_BACK/FRONT`) | 2 | o Rio tem `R2_SLING_SHOT_01_BACK/FRONT`, a incluir no banco |

Plano: envolver esses blocos em `#ifndef PS2BIRDS_RIO` e escrever, para o Rio, versões curtas (pássaro vermelho, peça pela tabela, estilingue `R2_*`), sem tocar o caminho do Classic. A compilação do Classic e os testes (`make test`) são a verificação de que ele não muda.

### Atualização: o ELF do Rio compila e liga

`ps2/src/main.c` ganhou um caminho próprio para a variante do Rio (`#ifdef PS2BIRDS_RIO`, ligado por `make GAME=rio`): o terreno do Classic fica desligado, o pássaro e as peças são desenhados pela tabela `rio/definition_sprites.h`, os alvos (gaiolas) passam pela mesma tabela e o estilingue do Classic não é desenhado (o `R2_SLING_SHOT` ainda não está no banco). O caminho do Classic ficou dentro do `#else`, sem alteração de código.

Conferido: `make GAME=rio build/PS2Birds_Rio.elf` compila e liga (5.629.244 bytes); o Classic também compila e liga (`make`). **Nada disso foi rodado ainda**: o ELF do Rio não foi testado no PCSX2, e o jogo só terá o que desenhar depois de o gerador montar os bancos do Rio (som, música, interface e fundos ainda são do Classic ou não existem). Falta a validação do `make test` do Classic para esta etapa (ver o registro de modificações).
