# Modificações do Rio — 0.50.2

Macacos completos. O Classic continua 1.0.0, sem mudança.

## Por que estavam incompletos
No Rio o macaco (`MonkeyStanding`, `MonkeyGrabbingWall`, `MonkeyHangingRoof` e as variantes) não é uma imagem só: o sprite da definição é uma **composição** de cauda, corpo, cabeça, rosto (e mão ou tufo, nos que agarram, penduram e no chefe). O gerador desenhava só o **último filho da composição**, que é o rosto. Por isso apareciam só as cabecinhas.

## Editado
- `tools/rio/extract_sprites_rio.py`: cada composição usada por uma peça de fase passa a ser desenhada num sprite só (`sd_compositions`, `BAKED`): filhos na ordem da composição, com o pivô de cada filho em (dx, dy), composição "fonte sobre destino" e o pivô do sprite na origem da composição. As composições do conjunto SD aparecem em arquivos com deslocamentos SD e HD (o dobro); vale a de menores deslocamentos. Os filhos que nenhuma peça usa sozinha saem do banco (o banco total ficou em 218 sprites, 9,2 MB, com mais sprites inteiros no lugar das partes).
- `tools/rio/pack_sprites_rio.py`: a tabela de peças usa o sprite da composição pelo próprio nome (o antigo "último filho" fica só como alternativa).
- `ps2/src/rio/sprite_ids.h` e `definition_sprites.h` regenerados; versão do Rio 0.50.2.

## Conferido no emulador (capturas na pasta "Capturas")
- "BEACH VOLLEY 1": macacos com corpo, cauda e orelhas, nas torres.

## Falta
- Só mudou a imagem. O comportamento dos macacos (olhar, piscar, agarrar, pular e o objetivo de fase) continua como peça comum (pendência R5).
- As outras composições usadas por peças (por exemplo as gaiolas com os pássaros) passam pelo mesmo caminho, mas não conferi uma a uma: pendência R7.
