# PS2Birds Rio — modificações desta etapa (0.9.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.9.0.md`.

## Editado
- `tools/rio/import_level.py`: usa o corpo da composição (último filho) para dimensionar gaiolas compostas.
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "composições medidas".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.8.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Level190: 25 de 25 objetos convertidos (5 gaiolas como alvos tipo 4).
- As 459 fases do Rio convertem sem definição sem adaptador (formas e tamanhos; não valida física nem jogo).
- O tamanho da gaiola (3,82 m) bate com o declarado nas gaiolas R2 (3,81 m).

## O que falta
1. Calibrar a gravidade.
2. `SPRITES.BIN` do Rio e WebP (caixa de metal).
3. Gravar o `CAMPAIGN.BIN` do Rio e o save `/PS2BRIO`.
4. Estados dos macacos, Luiz e chefes.
