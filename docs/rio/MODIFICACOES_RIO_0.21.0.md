# PS2Birds Rio — modificações desta etapa (0.21.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- Caminho de desenho do Rio em `ps2/src/main.c` (`#ifdef PS2BIRDS_RIO`): pássaro e peças pela tabela `rio/definition_sprites.h`, sem terreno nem estilingue do Classic.
- `docs/rio/MODIFICACOES_RIO_0.21.0.md`.

## Editado
- `ps2/src/main.c`: blocos de desenho do Classic dentro de `#else` (sem mudança de código); estilingue do Classic fora da variante do Rio.
- `ps2/Makefile`: `GAME=rio` define `PS2BIRDS_RIO`.
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "o ELF do Rio compila e liga".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.20.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `make GAME=rio build/PS2Birds_Rio.elf` compila e liga; o ELF do Classic também (`make`).
- O ELF do Rio não foi testado no PCSX2; falta o gerador montar os demais bancos.

## O que falta
1. Estilingue (`R2_SLING_SHOT`) e fundos do Rio no banco; som, música e interface do Rio no gerador.
2. Decodificador de WebP.
3. Rodar no PCSX2.
4. Macacos, Luiz e chefes.
