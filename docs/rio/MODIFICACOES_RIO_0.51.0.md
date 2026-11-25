# Modificações — Rio 0.51.0 (8 episódios novos) e Classic 1.3.2

Primeira entrega das fases da lista de assets "Necessário": os pacotes de fases do Rio que o port ainda não tinha. O Rio passa de 180 para **373 fases** em 14 episódios.

## Adicionado
- Episódios, na ordem do menu (7 a 14), com banco de sprites próprio cada um:
  - `MARKET MAYHEM` (`fruitmarket1/2`, 30 fases), `GOLDEN BEACHBALL` (`goldenbeachball`, `goldenbeachball2`, 30), `ROCKET RUMBLE` (`newyear1/2`, 20), `HIGH DIVE` (`blossomdive`, 15), `BLOSSOM RIVER` (`river1/2`, 20), `TIMBER TUMBLE` (`paradise1/2`, 18), `HIDDEN HARBOR` (`manaus1/2`, 20) e `TREASURE HUNT` (`treasurehunt1..4`, 40).
  - Os nomes vêm dos textos do APK (`EPISODE_SELECTION_EPISODE_NAME_7.._13`, `_GBB`). **Qual pacote é qual episódio foi deduzido** pelo ambiente sonoro dos temas e pelos nomes dos cartões (`BTN_EP_NYE`, `BTN_EP_HIDDEN`, `BTN_EP_TIMBER`...); o APK não traz a tabela de forma legível. Confira os nomes se algum não bater.
- Cartões do menu para 6 dos 8 episódios novos (o `HIGH DIVE` não tem cartão próprio). `MENU.BIN` do Rio com a arte `BTN_EP_*` das folhas `LEVELSELECTION`.
- Músicas de ambiente de 6 temas novos (Golden Beachball, Rocket Rumble, Harbor, Blossom River, Bird Paradise, avião), nas faixas do `MUSIC.BIN` que o Rio não usava. `MUSIC.BIN` com 15 faixas com música.
- Meia resolução automática: quando o banco de um episódio não cabe nas quatro texturas, as maiores peças de cenário (nunca pássaros) são guardadas pela metade do tamanho e o jogo as desenha em dobro (bit 8 da página no `SPRITES.BIN`; `sprites.c`).

## Editado
- `tools/rio/rio_import_level.py`: o tipo de forma `block` do catálogo (6 definições: caixas de fruta e toldos) vale `box`.
- `ps2/src/campaign_types.h`: `MAX_LEVEL_OBJECTS` de 640 para 768 (há fase do Rio com 697 peças).
- `tools/rio/build_rio_data.py` (lista de episódios), `extract_menu_rio.py`, `extract_audio_rio_music.py`, `rio_campaign.py`, tabelas `ps2/src/rio/*.h` regeneradas.
- Versões: Rio 0.51.0, Classic 1.3.2 (mesmos `sprites.c` e `campaign_types.h`; o Classic não muda de comportamento).

## Fora (por enquanto)
- `paradise1/bp_01` e `bp_02`: pássaro `SUPERPOWERUP_BIRD`, sem tipo no motor.
- Pacotes `bonus*` (4 fases por tema), `rewardlevels` (26) e `tutorial` (5): não têm lugar no menu de episódios.
- Camadas de tema ilegíveis (WebP sem decodificador no caminho do tema): alguns planos do mar do Hidden Harbor e High Dive, o arco-íris e as cachoeiras do Timber Tumble e Treasure Hunt, e o fundo do Market Mayhem (o chão aparece liso).
- Nenhuma das fases novas tem roteiro de vitória verificado; foram conferidas no emulador uma fase de cada episódio (peças, macacos, cenário, ambiente).
