# PS2Birds Rio — modificações desta etapa (0.14.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado. O gerador do Rio continua recusando ISO; o Classic 0.93.0 publicado não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.14.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "gravidade, o que foi procurado".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.13.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- A gravidade do Rio não está em nenhum dado legível (scripts, JSON/XML do APK); no binário nativo só há `gravityX` e `gravityY`.
- Continua provisória em +20 (mesmo `physicsToWorld` do Classic); precisa de medição no jogo original ou leitura do código nativo.

## O que falta
1. Confirmar a gravidade.
2. `SPRITES.BIN` do Rio e WebP (caixa de metal).
3. Ligar as macros de save ao build do Rio e rodar no PCSX2.
4. Macacos, Luiz e chefes.
