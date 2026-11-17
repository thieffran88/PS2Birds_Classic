# PS2Birds Rio — modificações desta etapa (0.19.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `ps2/src/rio/definition_sprites.h`: tabela peça-sprite do Rio (87 peças do primeiro episódio), gerada pelo `pack_sprites_rio.py`.
- `docs/rio/MODIFICACOES_RIO_0.19.0.md`.

## Editado
- `tools/rio/pack_sprites_rio.py`: também grava a tabela peça-sprite.
- `ps2/src/main.c`: a tabela vem da macro `PS2BIRDS_DEFINITION_SPRITES` (padrão `"definition_sprites.h"`).
- `ps2/Makefile`: `GAME=rio` passa a macro com `rio/definition_sprites.h`.
- `docs/rio/ORDEM_DAS_FASES.md`: seção da tabela peça-sprite e dos 217 erros que restam.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.18.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- O Classic compila normalmente com as duas macros (padrão inalterado).
- O `main.c` do Rio ainda dá 217 erros: nomes de sprite do Classic na interface e no desenho de pássaros e porcos.

## O que falta
1. Desenho do pássaro e da interface do Rio independentes dos nomes do Classic.
2. Decodificador de WebP.
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
