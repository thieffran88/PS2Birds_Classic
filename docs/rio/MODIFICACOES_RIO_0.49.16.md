# Modificações do Rio — 0.49.16

O Classic continua na 1.0.0, sem mudança de comportamento (conferido: `UI.BIN` e `MENU.BIN` do Classic saem idênticos byte a byte depois da refatoração dos extratores).

## Adicionado
- `tools/rio/extract_ui_rio.py`: `UI.BIN` do Rio, mesma página 128×1024 PSMT8 e mesmos nomes do Classic (`ps2/src/ui.h`), com os quadros do próprio Rio: estrelas grandes e selo de recorde (`MENU_NATIVE_RESULT_1`), botões de menu, reiniciar, próxima fase, continuar e som (`BUTTONS_NATIVE_SHEET_1`), o macaco da fase perdida (`LEVEL_FAILED_MONKEY`), as partículas (madeira, pedra, vidro, penas vermelha e azul, fumaça, estrelas, nuvens de explosão) e as pontuações coloridas (`SCORE_10K_*`, `5K_GREEN`). 64 sprites, 132.672 bytes.
- `tools/rio/extract_menu_rio.py`: `MENU.BIN` do Rio com o mesmo formato e os mesmos identificadores do Classic: logo `MAIN_LOGO`, botão JOGAR em três fatias (`BTN_POPUP_*`), botões de fase (`LS_BUTTON_BG_1..7`, cadeado, `LS_STARS_*`, pontos de página), cartões de episódio (`BTN_EP_1..7`), setas (`ICON_NEXT`, espelhada à esquerda), opções, som, sobre e voltar. O que o Rio não tem (pássaros voando no menu, prêmios, caixas de pontos) entra como entrada vazia, que o menu não desenha.
- `docs/MODIFICACOES_RIO_0.49.16.md` (este arquivo).

## Editado
- `tools/rio/build_rio_data.py`: gera também `FONT.BIN` (`extract_font_rio.py`, que já existia mas não era chamado), `UI.BIN` e `MENU.BIN`.
- `tools/gerar_iso.py`: a ISO e a pasta de dados do Rio incluem `FONT.BIN`, `UI.BIN` e `MENU.BIN`; etapa "Sons, música e interface".
- `tools/extract_ui.py` e `tools/extract_menu.py`: a parte de empacotar virou `write_ui` / `pack_menu`, usadas também pelo Rio. A saída do Classic não mudou.
- `tools/rio/pack_sprites_rio.py`: o banco do Rio passa a declarar os mesmos quatro slots de textura do Classic (1 MiB, 1 MiB, 512 KiB, 256 KiB; os que o banco não usa ficam com largura 0). O menu envia as páginas para esses slots, e com só dois slots ele era recusado. Mesmo orçamento de VRAM (2.883.584 bytes). `ps2/src/rio/sprite_ids.h` regenerado.
- `ps2/src/main.c`: no Rio, a tela inicial não escreve "CLASSIC" sob o logo (o logo do Rio já traz o nome); versão do Rio 0.49.16.

## Conferido no emulador (capturas na pasta "Capturas")
- Tela inicial com o logo e o botão JOGAR do Rio; seleção de episódios com os cartões do Rio; seleção de fases com os botões do Rio; HUD com a fonte do Rio e os acentos ("Pontuação", "Recorde"); partículas e a pontuação "500" sobre as peças.
- Não conferi no emulador as telas de fim de fase e de pausa do Rio (o jogo usa o mesmo código do Classic com o `UI.BIN` do Rio; a captura da pausa não caiu na hora certa).

## Falta
- Os pássaros que cruzam o menu e os prêmios dos episódios do Rio (entradas vazias); penas das outras cores (amarela, preta etc. usam a vermelha ou a azul); o cartão do episódio 2 em diante mostra a arte do Rio na ordem de `BTN_EP_n`, que pode não ser a ordem das fases do port.
- Cenas e créditos (`CUTSCENE.BIN` do Rio): pendência 59.
