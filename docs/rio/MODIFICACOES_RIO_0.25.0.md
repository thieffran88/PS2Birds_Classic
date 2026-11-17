# PS2Birds Rio — modificações desta etapa (0.25.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/extract_themes_rio.py`: monta o `THEMES.BIN` do Rio (formato PBTH v3) a partir do APK do usuário.
- `docs/rio/MODIFICACOES_RIO_0.25.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `THEMES.BIN` do Rio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.24.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- `theme2` gerado: 1 tema, 960.512 bytes; cabeçalho e tamanhos conferem com o formato.
- A aparência e a escala do fundo (campo 4 = 1,5) não foram comparadas com o jogo original.

## O que falta
1. Conferir a escala e a linha do chão do fundo no runtime (PCSX2).
2. Decodificador de WebP (caixas de metal e temas com folhas WebP).
3. Som, música e interface do Rio no gerador; rodar no PCSX2.
4. Macacos, Luiz e chefes.
