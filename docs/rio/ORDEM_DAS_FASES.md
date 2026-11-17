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
