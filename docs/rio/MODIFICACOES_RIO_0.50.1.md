# Modificações do Rio — 0.50.1

Fundos compostos e plataformas do carnaval. O Classic continua 1.0.0, sem mudança.

## Causa do que faltava
Não era problema de formato de imagem: o leitor de WebP (`tools/webp.py`) decodifica os 100 arquivos WebP do APK do Rio (89 VP8X e 11 VP8). O que faltava era:
- **Camadas de fundo compostas.** No `themes.lua`, algumas camadas (praia, aeroporto e outros) não são um quadro só, e sim uma **composição** (`THEME_n_COMPOSPRITES.dat`): ladrilhos da faixa mais os itens colocados nela (barcos, pedras, estrela-do-mar, guarda-sóis). O gerador só sabia ler quadros simples e deixava essas camadas de fora.
- **Plataformas do carnaval** (`THEME_CARNIVAL_PLATFORM_*`): ficam na pasta `themes_android`, que o gerador de sprites não consultava.

## Editado
- `tools/rio/extract_themes_rio.py`: `composites_of` lê as composições, e `composite_layer` desenha a camada em uma imagem (cada filho com o pivô na posição `dx, dy`, composição "fonte sobre destino"). A faixa se repete com a largura dos ladrilhos; os marcadores 1×1 (`THEME_*_EMPTY`) marcam as pontas; os itens que passam da borda dão a volta; os ladrilhos se sobrepõem em 1 pixel (sem emenda por arredondamento). O ladrilho-base é o de maior largura total, com desempate por nome: o primeiro teste escolhia por contagem, num conjunto sem ordem, e às vezes repetia os barcos pela tela inteira.
- `tools/rio/crop_sprites.py`: também procura sprites na pasta `themes_android` (por último, depois dos sprites de fase).
- `ps2/src/rio/sprite_ids.h` e `definition_sprites.h` regenerados (as plataformas do carnaval entram no banco do episódio: 3 páginas, 2,6 MB de 2,88 MB). Versão do Rio 0.50.1.
- `docs/rio/MODIFICACOES_RIO_0.50.0.md`: a lista "Falta" dela está resolvida aqui.

## Conferido no emulador (capturas na pasta "Capturas")
- "BEACH VOLLEY 1": mar, veleiros, iate, pedras, estrela-do-mar, areia e guarda-sóis no primeiro plano.
- "CARNIVAL UPHEAVAL 1": as plataformas verde, azul e rosa com franjas.

## Falta
- Os quadros de ação do Blu (`BIRD_BLU_FLAPPING_n`) continuam sem imagem: são composições de corpo e asas, só para o dash do Blu (usa o sprite de voo).
- Não vi o fundo do aeroporto em jogo (o período da faixa foi conferido nos números).
