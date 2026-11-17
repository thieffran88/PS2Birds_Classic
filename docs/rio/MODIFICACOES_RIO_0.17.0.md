# PS2Birds Rio — modificações desta etapa (0.17.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/pack_sprites_rio.py`: grava o `SPRITES.BIN` do Rio (formato PBSP v2) e o `sprite_ids.h` a partir do banco de sprites.
- `ps2/src/rio/sprite_ids.h`: lista de nomes e tamanhos dos 68 sprites do primeiro episódio (sem pixels, como o do Classic).
- `docs/rio/MODIFICACOES_RIO_0.17.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `SPRITES.BIN` e do `sprite_ids.h` do Rio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.16.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `SPRITES.BIN` do Rio: 1 banco, 68 sprites, 1.049.600 bytes; o cabeçalho confere e o `sprite_ids.h` compila.

## O que falta
1. Selecionar o `sprite_ids.h` por macro no runtime (`GAME=rio`) e ligar peça a sprite.
2. Decodificador de WebP (caixas de metal e outros).
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
