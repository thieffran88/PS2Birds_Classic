# Modificações do Rio — 0.50.0

Os **6 episódios do primeiro jogo** entram no Rio: 180 fases (eram 45). O Classic continua 1.0.0, sem mudança.

## Adicionado
- Episódios, na ordem do `CutsceneScene.lua` (`theme1End` a `theme6End`): Smugglers Den (`warehouse`, `warehouse2`), Jungle Escape (`jungle1`, `jungle2`), Beach Volley (`beach1`, `beach2`), Carnival Upheaval (`carnival1`, `carnival2`), Airfield Chase (`airport1`, `airport2`) e Smugglers Plane (`plane1`, `plane2`): 30 fases cada. Os títulos contam dentro do episódio ("BEACH VOLLEY 7"), e o menu os agrupa sozinho.
- **Um banco de sprites por episódio** (`tools/rio/pack_sprites_rio.py`, `pack_banks`): o runtime já lia vários bancos (o Classic tem dezenas). Os ids dos sprites são globais (ordem alfabética, 240 no total) e cada banco tem as próprias páginas e quadros; o sprite que o banco não tem fica com quadro vazio. `SPRITES.BIN` do Rio: 6 bancos, 8 MB. Cada fase grava o banco do seu episódio (`texture_bank`).
- `docs/rio/MODIFICACOES_RIO_0.50.0.md` (este arquivo).

## Editado
- `tools/rio/build_rio_data.py`: `EPISODES` (nome e pacotes de cada episódio), `DEFAULT_PACKS` passa a ter os 12 pacotes e a geração é feita por episódio. O `CAMPAIGN.BIN` tem 180 fases; `THEMES.BIN`, 8 temas (7,7 MB).
- `ps2/src/rio/sprite_ids.h` e `definition_sprites.h` regenerados (6 bancos, 240 sprites, 204 peças).
- `ps2/src/scenes.c`: no Rio, a primeira vez que um episódio é concluído toca o final dele: `rio_end1` a `rio_end6`.
- Versão do Rio 0.50.0.

## Conferido no emulador (capturas na pasta "Capturas")
- Menu de episódios com os cartões de Smugglers Den, Jungle Escape e Beach Volley (a arte de cada cartão é a do episódio certo).
- A primeira fase da praia ("BEACH VOLLEY 1") abre com a troca de banco: sprites, pássaros, macacos e o fundo do tema aparecem.

## Falta
- Há camadas de fundo e alguns sprites que o leitor de imagens ainda não lê (WebP de outro formato): praia (`THEME_BEACH_BG_LAYER_*`, barcos), aeroporto (`THEME_AIRPORT_BG_LAYER_3`) e as plataformas do carnaval (`THEME_CARNIVAL_PLATFORM_*`). Nessas fases falta parte do cenário ou das peças.
- Não passei as 135 fases novas uma a uma: peças que se mexem sozinhas ao tocar (lâmpadas, macacos) e a estabilidade das estruturas ainda não foram conferidas fase a fase (pendências R10 e R7).
- O Rio 2 (frutaria, golden beach ball, new year, manaus, blossom, river, treasure hunt, paradise) e os pacotes de bônus ainda não estão: pedem novos bancos e os pássaros Rocket e do power-up.
- O texto do título de um cartão pode passar do cartão ao lado ("BEACH VOLLEY CA...").
