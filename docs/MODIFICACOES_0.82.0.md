# Modificações completas — PS2Birds Classic 0.82.0

## Recompensas voando na tela inicial (`main_menu.lua`, `buildBirdList`)
- No original, a lista de coisas que voam na tela inicial inclui, além dos pássaros, a recompensa de cada episódio:
  - `reward_completion`, quando todas as fases do episódio têm pontuação;
  - `reward_three_stars`, quando todas têm 3 estrelas (substitui a de episódio completo).
- O port agora faz igual, com base nos recordes salvos no cartão de memória.
- Episódios com recompensa no APK: Poached Eggs, Mighty Hoax, Danger Above, Big Setup, Ham 'Em High, Mine and Dine, Birdday, Bad Piggies, Short Fuse, Surf and Turf, Piggy Farm e Dino.
- As recompensas usam as mesmas regras de lançamento dos pássaros (camada, velocidade, gravidade). O limite de objetos no ar subiu de 8 para 12.

## Arte e dados
- O `extract_menu.py` grava no `MENU.BIN` 24 sprites novos: os pares `REWARD_n` / `REWARD_n_STAR` dos 12 episódios, em 45% do tamanho original. O arquivo passou de 74 para 98 sprites.
- No `menu_gfx.h`:
  - novos `MG_REWARD(r)` e `MG_REWARD_STAR(r)`;
  - `MG_MAX` subiu de 96 para 128.
- O teste `menu_assets_test.py` foi atualizado.

## Levantamento
- A decoração dos lados da seleção de fases (`LS_THEME_*_LEFT/RIGHT`) só existe no APK para o tema 15 e para o Egg Defender. Os episódios clássicos não têm essa decoração, então não há o que portar para eles. O `LACUNAS_PORT.md` foi corrigido.
- A borda de grama no alto do terreno não aparece em nenhum dado Lua (`themes.lua`, `blocks.lua`): é desenhada pelo código nativo, que não foi recuperado.

## Conferido no PCSX2
- Captura em `docs/pcsx2_0.82.0_recompensas.png`.
- Foi feita com um build de teste que libera todas as recompensas: elas voam junto com os pássaros.
- O build final só mostra as recompensas conquistadas. Num cartão novo, só os pássaros voam, como no original.

## Limpeza
- Removidos os documentos da 0.81.0 (substituídos pelos da 0.82.0).
- Removidas da área temporária as pastas e os zips da 0.81.0, o `MENU.BIN` de teste e as capturas.

## Versão
- Tela de pausa, "Sobre" e README: 0.82.0.
