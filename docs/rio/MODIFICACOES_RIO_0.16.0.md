# PS2Birds Rio — modificações desta etapa (0.16.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/extract_sprites_rio.py`: monta o banco de sprites do Rio a partir do APK do usuário (recorte nas folhas PVR/PNG e empacotamento com o empacotador do Classic).
- `docs/rio/MODIFICACOES_RIO_0.16.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do banco de sprites do primeiro episódio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.15.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Smugglers' Den (30 fases): 81 definições, 74 sprites pedidos, 68 empacotados em 1 página (1 MiB de VRAM).
- 6 ficaram de fora: `BLOCK_METAL_BOX_1/2/3`, `BLOCK_CHAINS`, `B_FRUIT_PINEAPPLE` (WebP/ETC1) e `EMPTY_1X1` (sem imagem).
- Os dados gerados ficam em `work/rio/`, fora do Git.

## O que falta
1. `sprite_ids.h` e `SPRITES.BIN` do Rio; ligar peça a sprite no runtime.
2. Decodificador de WebP (caixas de metal e outros).
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
