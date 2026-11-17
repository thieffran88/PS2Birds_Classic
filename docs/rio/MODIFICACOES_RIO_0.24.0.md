# PS2Birds Rio — modificações desta etapa (0.24.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Nenhum código foi alterado: a etapa é de levantamento. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.24.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção "as camadas do `theme2` são sprites simples" e o que falta decidir (linha do chão e escala).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.23.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- As 3 camadas do tema da primeira fase são sprites únicos de largura 1024 (683, 129 e 311 de altura), não composições. O fundo do Rio é mais simples de montar que o do Classic.
- Em aberto: linha do chão (vem de `Ground.y` da fase) e fator de escala (câmeras `iphone` 480x320 e `ipad` 1024x768).

## O que falta
1. `extract_themes_rio.py` e o `THEMES.BIN` do Rio; `background.c` do Rio.
2. Decodificador de WebP.
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
