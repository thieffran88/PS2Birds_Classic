# Modificações completas — PS2Birds Classic 0.74.0

## Fundos originais das fases (`themes.lua`)
- Novo `tools/extract_themes.py` e novo arquivo `THEMES.BIN` (30 MB), com um fundo por tema, 37 ao todo.
- **Fonte dos dados:**
  - o `themes.lua` do APK, lido só como dados: cor do céu (`color`), cor do chão (`groundColor`), camadas de trás (`bgLayers`) e da frente (`fgLayers`) com a escala de cada uma;
  - as folhas `themes_android` e as composições `THEMEnn.compo.json` (camada + árvores, nuvens e outros itens).
- **Montagem no PC:**
  - cada camada é reduzida de 1024×768 para 640×448, apoiada na linha do chão pelo pivô do sprite e repetida na largura;
  - o resultado é uma imagem opaca de 640×648 em 16 bits (448 linhas acima do chão e 200 abaixo).
  - A câmera do port não se move, então os fatores de paralaxe não entram.
- **Temas sem arte no APK** (baixados à parte no original):
  - `themeDino2` (16 fases do Dino) usa o fundo do `themeDino`;
  - `themeMLExtra` (sem fases) usa o do tema 1.
- `CAMPAIGN.BIN` passa à versão 9: cada fase guarda o índice do seu tema. Há uma função nova, `game_level_theme()`. Todas as 745 fases têm fundo.
- **No PS2** (novo `ps2/src/background.c` / `background.h`):
  - ao entrar numa fase, o tema é lido do disco só se mudou (cerca de 830 KB);
  - as 448 linhas em volta do chão da fase são copiadas para uma imagem de tela;
  - a cada quadro, essa imagem vai para o buffer de desenho dentro da fila do gsKit, antes dos desenhos da fase, sem gastar memória de vídeo (as páginas de textura estão cheias).
- **Detalhes técnicos resolvidos no PCSX2:**
  - um pacote de imagem do GIF comporta no máximo 512 KB, então a cópia vai em três faixas (192, 192 e 64 linhas) alinhadas às páginas de 64 linhas do GS;
  - a fila "oneshot" do gsKit passou de 1 MB para 3 MB, porque a cópia é feita para dentro da fila;
  - o gsKit recebe o endereço em bytes e o converte sozinho.
- A tela inicial também usa o fundo do tema 1, como no original.
- Sem `THEMES.BIN`, volta o céu com morros desenhado pelo port.

## Testes
- Novo `ps2/tests/themes_assets_test.py`: cabeçalho, 37 fundos opacos com camadas (nenhum de cor lisa), alinhamento e todos os temas usados pelas fases presentes.
- Os arquivos de teste da física ganharam o campo `theme` da fase.
- **Conferido no PCSX2** (captura em `docs/pcsx2_0.74.0_fundos.png`):
  - tela inicial com o fundo do tema 1;
  - a 1-1 com o fundo atrás do estilingue e da torre;
  - a tela de vitória por cima do fundo.

## Gerador e documentação
- `gerar_iso.py` grava `THEMES.BIN` na ISO e na pasta USB/HDD. Agora são nove arquivos.
- O kit para Windows inclui o `extract_themes.py`.
- O gerador leva mais tempo (cerca de 6 min aqui; 10 min sob Wine) por causa da montagem dos fundos. O README e o `LEIA-ME` agora dizem "de 5 a 10 minutos".

## Limpeza
- Removidos os documentos da 0.73.0 (substituídos pelos da 0.74.0) e a captura da HUD da 0.72.0 (a nova captura mostra a HUD sobre o fundo).
- Removidos da área temporária as pastas da 0.73.0, as prévias dos fundos, os scripts e ISOs de teste e as capturas.

## Versão
- Tela de pausa e README: 0.74.0.
