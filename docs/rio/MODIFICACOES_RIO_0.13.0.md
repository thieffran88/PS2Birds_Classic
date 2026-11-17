# PS2Birds Rio — modificações desta etapa (0.13.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado. O gerador do Rio continua recusando ISO; o Classic 0.93.0 publicado não mudou (as alterações de save só entram no próximo fechamento do Classic). Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `ps2/tests/save_id_test.c`: confere que um save de um jogo é recusado pelo outro.
- `docs/rio/MODIFICACOES_RIO_0.13.0.md`.

## Editado
- `ps2/src/save_codec.c`: identidade do jogo na macro `PS2BIRDS_SAVE_ID` (padrão "CLAS"; Rio "RIO1").
- `ps2/Makefile`: `make save-test` compila e roda o teste de identidade com as duas identidades.
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "identidade do codec separada".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.12.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- O save do Rio tem pasta (`/PS2BRIO`) e identidade (`RIO1`) próprias, sem mudar os saves existentes do Classic.
- `make save-test` passa, incluindo a recusa cruzada entre os dois jogos.

## O que falta
1. Calibrar a gravidade.
2. `SPRITES.BIN` do Rio e WebP (caixa de metal).
3. Ligar as duas macros ao build do Rio e rodar no PCSX2.
4. Macacos, Luiz e chefes.
