# PS2Birds Rio — modificações desta etapa (0.31.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir. Nenhum código foi alterado: a etapa é de levantamento e documentação.

## Adicionado
- `docs/RELATORIO_PARA_LEIGOS.md`: relatório do estado do projeto em linguagem simples, sem nomes de pastas ou arquivos (também enviado ao Drive).
- `docs/rio/MODIFICACOES_RIO_0.31.0.md`.

## Editado
- Nada além dos registros.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.30.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- No primeiro episódio (30 fases) só três pássaros são usados: vermelho (47 lançamentos, 42%), amarelo (34, 30%) e azul pequeno (31, 28%). Preto e branco não são necessários para a primeira versão jogável.
- Habilidades: o amarelo é `BOOST` (igual ao Classic); o azul pequeno do Rio é `CLUSTER_BOMB`, que o runtime ainda não distingue da divisão em três do Classic.
- `make test` do Classic passou na etapa anterior (saída 0).

## O que falta
1. `UI.BIN`, `FONT.BIN` e `MENU.BIN` do Rio; ligar tema e música no runtime; conferir no PCSX2.
2. Habilidade do azul pequeno (bomba de cachos).
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
