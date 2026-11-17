# PS2Birds Rio — modificações desta etapa (0.6.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.6.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção "Objetivos de vitória" (62 definições de `levelGoal`: gaiolas, macacos, redes, chefes e Luiz, com resistência e pontuação).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.5.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Descobertas
- Gaiolas e macacos têm resistência 10 a 16 e valem 5000 pontos; contam para a vitória como os porcos do Classic. Isso permite uma primeira versão com poucas mudanças no runtime.
- Chefes têm resistência de 500 a 10000 e exigem estados próprios (etapa posterior).

## O que falta
1. Gravidade (calibrar).
2. `SPRITES.BIN` do Rio e composições das gaiolas.
3. WebP (caixa de metal).
4. Tipo de alvo "gaiola" no runtime e save `/PS2BRIO`.
