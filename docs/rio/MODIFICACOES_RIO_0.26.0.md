# PS2Birds Rio — modificações desta etapa (0.26.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/sound_groups.py`: resolve os grupos de som das peças (nome do grupo -> variantes do perfil de som).
- `docs/rio/MODIFICACOES_RIO_0.26.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção "Sons da primeira fase".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.25.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- 52 grupos de som lidos; 25 dos 26 sons da primeira fase resolvidos (falta `ground_collision`). As variantes existem como MP3 no APK.

## O que falta
1. Converter os MP3 e a música para `SOUNDS.BIN` e `MUSIC.BIN`; mapear grupos e canais.
2. Conferir escala e linha do chão do fundo no PCSX2.
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
