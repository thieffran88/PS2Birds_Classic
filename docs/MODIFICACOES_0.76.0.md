# Modificações completas — PS2Birds Classic 0.76.0

## Textura de chão nas peças de terreno
- As peças de terreno fixas (`StaticBlock_01..06`, as máscaras `GROUND_BLOCK`) agora saem com a textura de chão do tema da fase (`texture` do `themes.lua`), como no original, e não mais numa cor laranja lisa.
- **Gerador (`extract_themes.py`):**
  - cada tema do `THEMES.BIN` (versão 3) ganha a sua textura: 256×256, lida do PVR RGB565 em `images/base` e guardada em RGBA5551;
  - os 37 temas têm textura própria.
- **No PS2:**
  - essas peças nunca se movem, então são pintadas uma vez na imagem de fundo da fase, quando a fase carrega;
  - para cada pixel, o `background.c` busca a máscara da peça (ainda na memória, junto das texturas da fase) e a textura do tema, ancorada na tela na escala de 1024×768;
  - assim não é preciso memória de vídeo, e essas peças deixam de ser desenhadas uma a uma a cada quadro.
- Ficam como antes:
  - as peças de terreno que quebram (`StaticFragileBlock`);
  - os palanques, a cama elástica e a tigela do ovo;
  - as peças com sprite próprio do `blocks.lua`.
- Nova função `sprites_frame_rgba()` (pixels de uma figura da fase atual).
- Novo campo `stage` em `background_prepare()`: a imagem é refeita ao trocar de fase.
- O teste `themes_assets_test.py` agora confere a textura de chão de cada tema.
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.76.0_terreno.png`): na 1-2, a base do estilingue e o terreno em "L" saem com a textura do tema.

## Seleção de fases — investigação
- As decorações laterais (pássaros e porcos) da seleção de fases não estão no APK: só existem as do Egg Defender (`LS_THEME_EGGDEFENDER*`). As outras são baixadas à parte pelo original. Ficam registradas como lacuna.

## Limpeza
- Removidos os documentos da 0.75.0 (substituídos pelos da 0.76.0).
- Removidos da área temporária as pastas da 0.75.0, os logs e as capturas de teste.

## Versão
- Tela de pausa e README: 0.76.0.
