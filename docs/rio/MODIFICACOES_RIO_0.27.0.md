# PS2Birds Rio — modificações desta etapa (0.27.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `tools/rio/extract_audio_rio.py`: monta o `SOUNDS.BIN` do Rio a partir do APK do usuário, com os mesmos números de efeito do Classic.
- `docs/rio/MODIFICACOES_RIO_0.27.0.md`.

## Editado
- `docs/rio/ORDEM_DAS_FASES.md`: seção do `SOUNDS.BIN` do Rio.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.26.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- 25 efeitos, 535.664 bytes (limite de 1 MiB); cabeçalho e alinhamentos conferem. Mesmos números do Classic, então o runtime não muda.
- Não foi ouvido; as escolhas por nome (menu = `ui_select`, derrota, alvo destruído = quebra de gaiola) são aproximações.

## O que falta
1. `MUSIC.BIN` do Rio (música da fase) e sons por material das peças.
2. Conferir fundo e escala no PCSX2.
3. Decodificador de WebP.
4. Macacos, Luiz e chefes.
