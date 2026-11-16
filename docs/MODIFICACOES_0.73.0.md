# Modificações completas — PS2Birds Classic 0.73.0

## Menus com a arte original
- Novo `tools/extract_menu.py` e novo arquivo `MENU.BIN` (2,8 MB), com 62 sprites do APK:
  - o logotipo `MENU_LOGO` e o botão Jogar (`PLAY_BUTTON_BG_LEFT/MIDDLE/RIGHT`);
  - os 19 cartões de episódio (arte + título), montados pelas composições do `LEVELSELECTION.compo.json`;
  - a caixa de pontuação (com o texto "Pontuação" do pt_BR) e a de estrelas;
  - setas e botão voltar;
  - os quadros de fase `LS_LEVEL_BG_NORMAL_OPEN_1..7`, o fechado e o cadeado;
  - `LS_STARS_1..3` e os pontinhos de página.
- **Cartões sem arte própria:**
  - Short Fuse usa o `BTN_BOMB_BIRD` e Bird Island o `BTN_BIRDISLAND`, como no original;
  - Mighty Feathers não tem cartão no APK: usa a arte de Poached Eggs com o título escrito na fonte original.
- **Memória de vídeo:**
  - os menus não desenham fase, então, enquanto um menu está aberto, as páginas do `MENU.BIN` ocupam as quatro posições de textura das fases (512×512, 512×512, 512×256, 512×128);
  - ao entrar numa fase, as texturas dela voltam (da memória, se já estavam lidas).
- Novas funções `sprites_upload_pages()` e `sprites_slot()` em `sprites.c` e novo módulo `ps2/src/menu_gfx.c` / `menu_gfx.h`.

## Tela inicial (`main_menu.lua`)
- O jogo agora abre numa tela inicial: céu, nuvens, silhuetas de árvores, chão, o logotipo "ANGRY BIRDS", "CLASSIC" e o botão "JOGAR".
- X abre os episódios; O volta ao jogo.

## Seleção de episódios (`episode_selection.lua`)
- Um cartão por episódio, lado a lado:
  - o selecionado fica maior e os bloqueados escurecidos, com cadeado;
  - em cada cartão: número, título, pontuação total e estrelas conquistadas sobre o máximo.
- Esquerda/direita trocam de episódio (L2/R2 pulam 6). Há setas nos lados e o botão voltar.
- O volta à tela inicial.
- Fundo verde com raios saindo do sol na parte de baixo, desenhado em faixas horizontais.

## Seleção de fases (`level_selection.lua`)
- Grade 5×3 com o quadro de fase de cada episódio (`level_button` do `episodes.lua`, traduzido à mão):
  - número da fase na fonte original;
  - as estrelas `LS_STARS_n` embaixo;
  - quadro cinza com cadeado nas fases bloqueadas.
- A fase selecionada fica maior. Embaixo ficam o nome da fase com o recorde e os pontinhos de página originais.

## Testes
- `menu_test.cpp`: tela inicial, Jogar → episódios, esquerda/direita, O volta à tela inicial.
- Novo `menu_assets_test.py`: cabeçalho, as quatro páginas, os 19 cartões, os títulos e os sprites sem sobreposição.
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.73.0_menus.png`):
  - tela inicial;
  - episódios depois de vencer a 1-1 (31000 pontos, 2/189 estrelas);
  - grade com a 1-1 de 2 estrelas e a 1-2 liberada;
  - a volta ao jogo com as texturas da fase certas.

## Gerador e documentação
- `gerar_iso.py` grava `MENU.BIN` na ISO e na pasta USB/HDD. Agora são oito arquivos.
- O kit para Windows inclui o `extract_menu.py`. O README e o `LEIA-ME` foram atualizados.

## Limpeza
- Removidos os documentos da 0.72.0 e a captura da 0.71.0, que mostrava a fonte antiga.
- Removidas da área temporária as pastas da 0.72.0, as prévias das páginas do menu, as capturas de teste e o log de testes.

## Versão
- Tela de pausa e README: 0.73.0.
