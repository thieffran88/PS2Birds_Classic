# Modificações completas — PS2Birds Classic 0.77.0

## Tela de pausa no formato do original (`PausePage.lua`)
- **Painel translúcido à esquerda, com os botões originais em coluna:**
  - Continuar (`BUTTON_RESUME`, START) no alto;
  - Reiniciar (`BUTTON_RESTART`, TRIÂNGULO);
  - Menu (`BUTTON_MENU`, O);
  - embaixo, o som (`BUTTON_SFX`, R3), com o risco `BUTTON_OFF` por cima quando o som está desligado.
- O jogo continua visível, escurecido, atrás do painel.
- O título "pausa" (`MT_PAUSE` do pt_BR) aparece na fonte original.
- As dicas de controle, a versão e o estado do cartão de memória ficam numa faixa escura à direita.
- O na pausa abre o menu de fases (antes só funcionava na vitória e na derrota).
- **Arte:**
  - o `extract_ui.py` acrescenta `BUTTON_RESUME`, `BUTTON_SFX` e `BUTTON_OFF` à página de interface (agora 11 sprites, na mesma página de 256×256);
  - `BUTTON_SFX_OFF` não existe no APK; o original desenha o `BUTTON_OFF` por cima.
- O teste `ui_assets_test.py` agora confere os 11 sprites.
- Conferido no PCSX2 (captura em `docs/pcsx2_0.77.0_pausa.png`), junto com a tela de vitória, que segue igual.

## Correção no build
- O `Makefile` não recompilava um módulo quando só o cabeçalho dele mudava. Por isso, no primeiro teste, o `ui.o` antigo esperava 8 sprites e recusava o `UI.BIN` novo. Agora cada módulo (`ui`, `font`, `menu`, `menu_gfx`, `music`, `level_end`, `background`, `main`) depende dos seus cabeçalhos.

## Limpeza
- Removidos os documentos da 0.76.0 (substituídos pelos da 0.77.0).
- Removidas da área temporária as pastas da 0.76.0, as capturas e os logs de teste.

## Versão
- Tela de pausa e README: 0.77.0.
