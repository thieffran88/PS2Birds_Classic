# PS2Birds Rio — modificações desta etapa (0.4.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/crop_sprites.py`: recorta sprites pelo nome, usando os retângulos dos arquivos `.dat` e a folha legível (PVR da pasta `ingame_assets_sd` ou PNG). Grava RGBA e um `.json` com tamanho e pivô em `work/rio/sprites/` (fora do Git).
- `docs/rio/MODIFICACOES_RIO_0.4.0.md`.

## Editado
- Nada além deste registro.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.3.0.md` (e a cópia do Drive), substituído por este.

## Resultado
- 17 dos 19 sprites pedidos da Level190 foram recortados e conferidos numa imagem de teste: pássaro vermelho, 4 peças de gaiola, 4 caixas de papelão, 2 caixotes, madeira 10x1 e 5 passarinhos presos.
- Os 2 que faltam são `BLOCK_METAL_BOX_1/2` (a caixa de metal), que só existe em WebP.
- Os recortes ficam em `work/`, que o `.gitignore` já exclui: nenhum dado do jogo entra no repositório.

## O que falta
1. Montar o `SPRITES.BIN` do Rio a partir desses recortes (ajustando ao orçamento de VRAM).
2. Decidir sobre o WebP (caixa de metal).
3. Montar as composições das gaiolas (`B_CAGE_*`).
4. Origem do lançamento e gravidade.
5. Objetivo de vitória por gaiolas no runtime e save `/PS2BRIO`.
