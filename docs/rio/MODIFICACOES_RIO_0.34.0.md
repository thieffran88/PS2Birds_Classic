# PS2Birds Rio — modificações desta etapa (0.34.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/webp.py`: leitor de WebP com perda (VP8) e contêiner, em Python puro.
- `tools/rio/vp8l.py`: leitor do fluxo sem perda (VP8L) usado na transparência (ALPH).
- `tools/rio/vp8_tables.py`: tabelas numéricas do VP8 (probabilidades e quantização).
- `tools/rio/webp_check.py`: conferência com o Pillow (só desenvolvimento).
- `docs/rio/MODIFICACOES_RIO_0.34.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do leitor de WebP.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.33.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- As 100 folhas WebP do APK do Rio (11 sem transparência, 89 com) são decodificadas com diferença zero em relação à referência, pixel a pixel e em todos os canais (2 min 32 s no total).
- Isso destrava caixas de metal, partículas, temas e menus do Rio; falta ligar o leitor aos extratores.

## O que falta
1. Ligar o leitor de WebP aos extratores (banco de sprites, temas, interface).
2. `UI.BIN` e `MENU.BIN` do Rio; ligar tema e música no runtime; conferir no PCSX2.
3. Habilidade do azul pequeno (bomba de cachos).
4. Macacos, Luiz e chefes.
