# PS2Birds Rio — modificações desta etapa (0.7.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.7.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção "O runtime já aceita o alvo gaiola".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.6.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Descobertas
- `CAMPAIGN.BIN` já tem gravidade, estilingue, chão e escala por fase; o gerador do Rio só preenche os campos.
- Peças tipo 4 contam para a vitória (5000 pontos cada): as gaiolas da primeira fase entram assim, sem mudar o runtime.

## O que falta
1. Calibrar a gravidade.
2. `SPRITES.BIN` do Rio e composições das gaiolas.
3. WebP (caixa de metal).
4. Gerador do Rio emitindo as fases e save `/PS2BRIO`.
