# Modificações — Classic 1.4.2 e Rio 0.53.2 (som do próximo pássaro)

## Adicionado
- Quando um pássaro termina o voo e o próximo da fila assume o estilingue (estado voa → mira, com pássaros sobrando), toca `bird next military a1`, o som de "próximo pássaro" do original (existe nos dois APKs).
- `SOUNDS.BIN`: Classic com 37 efeitos e Rio com 44.

## Editado
- `tools/extract_audio.py`, `tools/rio/extract_audio_rio.py`, `ps2/src/audio_ids.h` (`SFX_NEXT_BIRD`), `main.c`, `ps2/tests/audio_assets_test.py` (37 efeitos). Versões: Classic 1.4.2, Rio 0.53.2.

## Decisão
- O original tem três variantes (`a1`..`a3`) e sons de seleção por pássaro (`bird 01 select`..`05`, só no Classic). Entrou uma variante só: o jogo sorteia entre as variantes e o port ainda não tem sorteio de som. Não conferi no ouvido.
