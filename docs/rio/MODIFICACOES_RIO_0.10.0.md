# PS2Birds Rio — modificações desta etapa (0.10.0, investigação)

Nenhum ELF, ISO ou kit novo foi gerado: o gerador do Rio continua recusando ISO até o runtime ser integrado, e o runtime do Classic (0.93.0) não mudou. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/rio_campaign.py`: grava um `CAMPAIGN.BIN` com fases do Rio (formato PBCP v9, mesmo escritor do Classic).
- `docs/rio/MODIFICACOES_RIO_0.10.0.md`.

## Editado
- `tools/rio/import_level.py` renomeado para `tools/rio/rio_import_level.py` (evita colidir com o importador do Classic) e gravidade provisória passou a +20, a convenção dos arquivos do Classic.
- `docs/rio/ORDEM_DAS_FASES.md`: seção do primeiro `CAMPAIGN.BIN` e referências aos novos nomes.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.9.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `CAMPAIGN.BIN` do Rio com a Level190: 1 fase, 25 objetos, 4 pássaros; relido sem erro em Python.
- Não carregado pelo runtime ainda; gravidade +20 é provisória.

## O que falta
1. Carregar o arquivo no runtime (teste em PC e PCSX2).
2. Calibrar a gravidade.
3. `SPRITES.BIN` do Rio e WebP (caixa de metal).
4. Save `/PS2BRIO`, macacos, Luiz e chefes.
