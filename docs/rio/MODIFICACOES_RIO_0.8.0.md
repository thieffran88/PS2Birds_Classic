# PS2Birds Rio — modificações desta etapa (0.8.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/import_level.py`: importador de fases do Rio (objetos, materiais, estilingue, gravidade como parâmetro).
- `docs/rio/MODIFICACOES_RIO_0.8.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do importador e do resultado na Level190.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.7.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Level190: 21 de 25 objetos convertidos; 4 pássaros; estilingue no pássaro 1.
- Faltam 4 gaiolas compostas: o tamanho depende da extensão das composições (COMP).

## O que falta
1. Medir as composições COMP das gaiolas (`B_CAGE_*`).
2. Calibrar a gravidade.
3. `SPRITES.BIN` do Rio e WebP (caixa de metal).
4. Gravar o `CAMPAIGN.BIN` do Rio e o save `/PS2BRIO`.
