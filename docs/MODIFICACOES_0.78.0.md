# Modificações completas — PS2Birds Classic 0.78.0

## Tela inicial navegável, com as opções do original (`main_menu.lua`)
- Cima/baixo escolhem entre Jogar e a engrenagem de opções (`BUTTON_OPTIONS`, canto inferior esquerdo). O escolhido fica maior e mais claro, com um anel amarelo atrás.
- **X na engrenagem abre, acima dela, como no `TOGGLE_OPTIONS` do original:**
  - o som (`BUTTON_SOUNDS_SMALL`, com o risco `BUTTON_OFF_SMALL` quando desligado);
  - o "sobre" (`BUTTON_ABOUT_SMALL`).
- A navegação segue a ordem na tela: Jogar, Sobre, Som, engrenagem.
- O fecha as opções; se já estão fechadas, volta ao jogo.

## Tela "Sobre" (`credits.lua`)
- Mostra a imagem `ABOUT_BIRDS_3` e o texto `TEXT_ABOUT` em português, lido do `TEXTS_BASIC.dat` do APK. A versão vira "v0.78.0 (PS2Birds Classic)".
- Uma linha final identifica o port como não oficial e lembra que usa o APK do jogador.
- O ou X voltam à tela inicial.

## Arte e dados
- **`extract_menu.py`:**
  - acrescenta os 5 sprites (67 ao todo);
  - lê o `TEXTS_BASIC.dat` (KA3D: blocos LDAT, LIDS e TXGP);
  - grava o `TEXT_ABOUT` ao fim do `MENU.BIN`, que passa à versão 2.
- **`extract_font.py`:** a `FONT_BASIC` ganha o "©" (141 glyphs; a página continua com 256×128).
- O teste `menu_assets_test.py` agora confere os 67 sprites e o texto "sobre".
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.78.0_menu_inicial.png`):
  - opções abertas e som desligado;
  - tela "Sobre";
  - volta com O.

## Limpeza
- Removidos os documentos da 0.77.0 (substituídos pelos da 0.78.0).
- Removidas da área temporária as pastas da 0.77.0, as capturas e os logs de teste.

## Versão
- Tela de pausa, "Sobre" e README: 0.78.0.
