# PS2Birds Rio — modificações desta etapa (0.12.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado. O gerador do Rio continua recusando ISO; o Classic 0.93.0 publicado não mudou (a alteração abaixo só entra no próximo fechamento do Classic). Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.12.0.md`.

## Editado
- `ps2/src/save_mc.c`: o diretório do save é a macro `PS2BIRDS_SAVE_DIR` (padrão `/PS2BIRDS`); o Rio compila com `/PS2BRIO`.
- `docs/rio/ORDEM_DAS_FASES.md`: seção "Save do Rio".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.11.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `make save-test` passa com o padrão; a compilação com `/PS2BRIO` usa só caminhos do Rio.
- O ELF recompilado difere do da 0.93.0 em 109 bytes (constantes de texto); o do código anterior foi reproduzido byte a byte. Por isso não houve release do Classic nesta etapa.

## O que falta
1. Separar a identidade do codec do save (`CLAS`) do Rio.
2. Calibrar a gravidade.
3. `SPRITES.BIN` do Rio e WebP (caixa de metal).
4. Rodar no PCSX2; macacos, Luiz e chefes.
