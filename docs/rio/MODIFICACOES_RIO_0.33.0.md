# PS2Birds Rio — modificações desta etapa (0.33.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir. Nenhum código foi alterado: a etapa é de levantamento.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.33.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção "Interface e partículas do Rio".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.32.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- Os 11 sprites de interface do Classic têm equivalente no Rio, todos em PVR no conjunto 480x320 (legível hoje).
- 38 dos 53 nomes de partículas do Classic não existem no Rio; as partículas (penas, fumaça, estrelas) estão quase todas em WebP.
- O decodificador de WebP é a próxima peça de maior retorno.

## O que falta
1. Decodificador de WebP (partículas, temas, caixas de metal).
2. `UI.BIN` e `MENU.BIN` do Rio (adaptar `extract_ui.py` e `extract_menu.py`).
3. Habilidade do azul pequeno; ligar tema e música; rodar no PCSX2.
4. Macacos, Luiz e chefes.
