# PS2Birds Rio — modificações desta etapa (0.22.0, investigação)

Nenhum ELF, ISO ou kit do Classic foi publicado nesta etapa; o Classic 0.93.0 publicado não mudou. O gerador do Rio continua recusando ISO. Não havia ISO/ELF anteriores para excluir.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.22.0.md`.

## Editado
- `tools/rio/extract_sprites_rio.py`: lista `EXTRA` (estilingue `SLING_SHOT_01_BACK/FRONT`); banco do Rio com 70 sprites.
- `ps2/src/rio/sprite_ids.h`: regenerado com os 2 sprites do estilingue (70 nomes).
- `ps2/src/main.c`: o estilingue é desenhado igual no Classic e no Rio (removido o `#ifndef` provisório).
- `docs/rio/ORDEM_DAS_FASES.md`: atualização "estilingue no banco do Rio".

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.21.0.md` (e a cópia do Drive), substituído por este.
- Não havia outros arquivos sem relevância no repositório.

## Resultado
- ELF do Rio e ELF do Classic compilam e ligam. O Classic não mudou de código (o trecho do estilingue volta ao que era).
- `make test` completo do Classic não foi repetido nesta etapa (a única mudança de código é a volta do trecho do estilingue ao original).

## O que falta
1. Fundos do Rio no banco; som, música e interface do Rio no gerador.
2. Decodificador de WebP.
3. Rodar no PCSX2.
4. Macacos, Luiz e chefes.
