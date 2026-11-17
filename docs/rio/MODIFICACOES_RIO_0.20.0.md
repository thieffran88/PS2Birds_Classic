# PS2Birds Rio — modificações desta etapa (0.20.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir. Nenhum código foi alterado: a etapa é de levantamento.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.20.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: inventário do desenho do `main.c` que ainda depende de nomes de sprite do Classic, com o plano para o Rio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.19.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Os 210 nomes de sprite do Classic que faltam ao Rio estão em 5 trechos do `main.c`: pássaros (23), porcos (17), tabela de blocos (147), terreno/peças fixas e estilingue.
- Plano: `#ifndef PS2BIRDS_RIO` nesses trechos e versões curtas para o Rio, sem mudar o caminho do Classic.

## O que falta
1. Implementar o desenho do Rio (pássaro vermelho, peça pela tabela, estilingue `R2_*`).
2. Decodificador de WebP.
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
