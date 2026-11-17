# PS2Birds Rio — modificações desta etapa (0.15.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `make GAME=rio`: variante do runtime com `/PS2BRIO` e identidade `RIO1` (saída `build/PS2Birds_Rio.elf`).
- `docs/rio/MODIFICACOES_RIO_0.15.0.md`.

## Editado
- `ps2/Makefile`: variável `GAME` (padrão `classic`, sem mudança).
- `docs/rio/ORDEM_DAS_FASES.md`: decisão "o Rio segue o mesmo esquema do Classic" (sem dado do jogo no repositório, gravidade +20, build por variante).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.14.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- O ELF do Rio compila e contém só os caminhos `/PS2BRIO`.
- Decisão registrada: mesmo esquema do Classic; gravidade +20 sem medição.

## O que falta
1. Bancos de sprites do Rio no gerador e `sprite_ids.h` próprio.
2. Ligar os demais bancos (som, música, interface) ao gerador do Rio.
3. Rodar no PCSX2.
4. Macacos, Luiz e chefes.
