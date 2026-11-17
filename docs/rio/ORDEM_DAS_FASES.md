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
