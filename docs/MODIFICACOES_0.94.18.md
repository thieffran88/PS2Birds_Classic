# Modificações — 0.94.18 (Classic) e 0.49.8 (Rio)

## Editado
- **Porco machucado em Mine and Dine** (`tools/extract_sprites.py`): os bancos de textura 11 a 13 trocavam o quadro do porco machucado (`PIG_*_02`) pelo normal para caber na memória de vídeo. A conta estava errada: nos bancos 11 e 12 a quarta página de textura (até 512 × 128) estava vazia e comporta os seis quadros; no banco 13 comportam cinco (só o do avô de bigode, o maior, ainda não cabe). Agora esses porcos mostram o rosto de machucado. Do banco 14 em diante nada muda.
- `ps2/tests/sprite_test.py` (contagem de sprites dos bancos 11 a 13: 122, 129 e 122) e `docs/LACUNAS_PORT.md`.
- Visto no emulador, na fase Mine and Dine 15-12 com os porcos com 40% da vida: olhos de machucado (capturas na pasta `Capturas` do Drive).

## Adicionado
- `docs/MODIFICACOES_0.94.18.md` e `docs/rio/MODIFICACOES_RIO_0.49.8.md`.

## Excluído
- Notas da versão anterior.

## Ainda falta
- O avô de bigode machucado no banco 13 e todos os porcos machucados do banco 14 em diante.
