# PS2Birds Rio — modificações desta etapa (0.32.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/extract_font_rio.py`: monta o `FONT.BIN` do Rio a partir do APK do usuário (conjunto de fontes 864x480).
- `docs/rio/MODIFICACOES_RIO_0.32.0.md`.

## Editado
- `tools/extract_font.py`: `extract` aceita a pasta de tamanho e a lista de fontes (padrões iguais aos de antes).
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `FONT.BIN` do Rio.

## Excluído
- `docs/RELATORIO_PARA_LEIGOS.md` e `docs/rio/MODIFICACOES_RIO_0.31.0.md` (e as cópias do Drive): o relatório para leigos deve ficar na conversa, não em arquivo.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `FONT.BIN` do Rio: 19.648 bytes. O `FONT.BIN` do Classic continua idêntico (hash) e o teste das fontes passa.

## O que falta
1. `UI.BIN` e `MENU.BIN` do Rio; ligar tema e música no runtime; conferir no PCSX2.
2. Habilidade do azul pequeno (bomba de cachos).
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
