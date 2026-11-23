# Modificações — 0.94.9

Versão da **conferência das animações das cenas**.

## Editado
- **Largura das cenas** (`tools/extract_cutscene.py`): a imagem de uma cena é a composição inteira. Eu a cortava na largura de `setreferencesize`, e em algumas cenas o desfecho ficava depois do corte: no **final do Mighty Hoax (mundo 5)** não aparecia o rei verdadeiro atrás do arbusto de papelão (nem o piscar do olho dele) e no **final do Bad Piggies 3** não aparecia o rei de olhos roxos. Agora a cena rola até o fim da composição, como o roteiro pede (alvo 1,0 do sprite). A regra vale quando a composição é mais larga que a referência e não é mais alta que ela (Farm e Dino ficam como antes, porque guardam decoração muito maior). Também mudam levemente a abertura do Mighty Hoax, do Big Setup (o primeiro alvo 0,822 do roteiro coincide exatamente com a largura de referência), Ham Em High, Mine and Dine, Bad Piggies, Short Fuse e os finais 11, 14 e 17.
- `docs/CUTSCENES.md`, README e versão 0.94.9.

## Adicionado
- `docs/MODIFICACOES_0.94.9.md`, `docs/VALIDACAO_0.94.9.md` e `docs/rio/MODIFICACOES_RIO_0.48.0.md`.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.8 (substituídos) e a documentação 0.47.0 do Rio.

## Resultado da conferência
- `tween`, `tweenPosition` e `tweenScale` só aparecem nas 4 cenas de painéis; `createsprite` e `deletesprite` só nos 8 finais com os olhos do rei e na abertura do Short Fuse. Tudo isso já estava feito; não há outra animação no roteiro.
- O brilho da árvore do Short Fuse (`BACKGROUND_TREE_GLOW`) não existe no APK, então o original também não mostra nada.
