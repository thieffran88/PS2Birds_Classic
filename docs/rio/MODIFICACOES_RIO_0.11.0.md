# PS2Birds Rio — modificações desta etapa (0.11.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `ps2/tests/rio_smoke.cpp`: teste de fumaça de desenvolvedor que carrega um `CAMPAIGN.BIN` do Rio (fora do `make test`).
- `docs/rio/MODIFICACOES_RIO_0.11.0.md`.

## Editado
- `ps2/tests/AUXILIARES.md`: seção do teste de fumaça do Rio.
- `docs/rio/ORDEM_DAS_FASES.md`: resultado do teste em PC.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.10.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- O runtime do Classic carrega a Level190 do Rio sem alteração: 25 objetos, 4 pássaros, 5 alvos (gaiolas), 20 peças.
- Um tiro libertou uma gaiola (5000 pontos). Gravidade +20 provisória; PCSX2 ainda não testado.

## O que falta
1. Calibrar a gravidade.
2. `SPRITES.BIN` do Rio e WebP (caixa de metal).
3. Rodar no PCSX2 e criar o save `/PS2BRIO`.
4. Macacos, Luiz e chefes.
