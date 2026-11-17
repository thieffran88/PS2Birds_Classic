# PS2Birds Rio — modificações desta etapa (0.5.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.5.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção sobre origem do lançamento e gravidade.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.4.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Descobertas
- Origem do lançamento resolvida: o pássaro de `startNumber` 1 de cada fase marca o estilingue; não existe `birdZeroPos`.
- Gravidade: não está no Lua; no binário nativo só há `gravityX` e `gravityY`. Continua em aberto.

## O que falta
1. Obter ou calibrar a gravidade do mundo.
2. Montar o `SPRITES.BIN` do Rio (VRAM) e as composições das gaiolas.
3. Decidir sobre o WebP (caixa de metal).
4. Objetivo de vitória por gaiolas no runtime e save `/PS2BRIO`.
