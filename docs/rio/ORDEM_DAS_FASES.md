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

## Importador de fases (`tools/rio/import_level.py`)

Converte uma fase do Rio na lista de objetos que o `build_campaign` grava no `CAMPAIGN.BIN`: tamanho pelo sprite (ou pela largura e altura da definição), escala da instância, material e resistência pelo catálogo, estilingue no pássaro 1 e gravidade como parâmetro (`--gravity`, padrão -10, **só um marcador**: o valor real ainda não foi obtido).

Resultado na `warehouse/Level190`: 21 dos 25 objetos convertem (20 de material comum e 1 gaiola), mais 4 pássaros e o estilingue em (108,81; -9,46). As 4 gaiolas restantes (`BLOCK_CAGE_2`, `_8`, `_12`, `_15`) não têm largura e altura na definição: são composições (`B_CAGE_*`), e o tamanho tem de vir da extensão da composição nos arquivos COMP, que o leitor já lê mas ainda não mede. A saída fica em `work/rio/` (fora do Git).
