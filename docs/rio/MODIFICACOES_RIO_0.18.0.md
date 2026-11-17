# PS2Birds Rio — modificações desta etapa (0.18.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.18.0.md`.

## Editado
- `ps2/src/sprites.h`: a lista de sprites vem da macro `PS2BIRDS_SPRITE_IDS` (padrão `"sprite_ids.h"`, a do Classic).
- `ps2/Makefile`: `GAME=rio` passa a macro com `rio/sprite_ids.h`.
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "lista de sprites escolhida por macro".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.17.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- O Classic compila com a mudança. O ELF do Rio ainda não compila: o código de desenho usa nomes de sprite do Classic que a lista do Rio não tem.

## O que falta
1. Ligar cada peça do Rio ao seu sprite no runtime (desenho independente dos nomes do Classic).
2. Decodificador de WebP (caixas de metal e outros).
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
