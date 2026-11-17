# PS2Birds Rio — modificações desta etapa (0.28.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/extract_audio_rio_music.py`: monta o `MUSIC.BIN` do Rio a partir do APK do usuário, com a mesma contagem e ordem de faixas do Classic.
- `docs/rio/MODIFICACOES_RIO_0.28.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `MUSIC.BIN` do Rio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.27.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- 16 faixas (9 com música do Rio, 7 silenciosas), 20.896.256 bytes; cabeçalho e alinhamentos conferem.
- A correspondência é por nome e não foi ouvida.

## O que falta
1. Ligar o `theme2` à faixa do armazém no runtime do Rio.
2. Sons por material das peças; conferir fundo e escala no PCSX2.
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
