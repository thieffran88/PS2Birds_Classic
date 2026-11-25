# Modificações — Rio 0.52.0 (playground, recompensas, bônus e pássaros especiais) e Classic 1.3.3

Continuação da 0.51.0: os 5 grupos que tinham ficado fora. O Rio passa de 373 para **458 fases** em 17 episódios.

## Adicionado
- **PLAYGROUND** (`tutorial`, 5 fases), **REWARD LEVELS** (`rewardlevels`, 26) e **BONUS LEVELS** (os 13 pacotes `bonus*`, 4 fases cada, 52 no total), como episódios 15 a 17 do menu. O nome do Playground vem dos textos do APK (`EPISODE_SELECTION_TUTORIAL`).
- Os bônus têm um banco de sprites por pacote (13 bancos): juntos não cabiam num banco só. As recompensas, que misturam todos os temas, usam um banco com parte das peças em meia resolução.
- **RocketBird** e **SUPERPOWERUP_BIRD** (9 fases: `paradise1/bp_01` e `bp_02`, as de foguete em `newyear`/`bonusNewyear`, `rewardlevels`), como **aproximação**: o pássaro-foguete joga como o amarelo (aceleração ao toque) e o macaw do power-up de bando como o azul (divide em três). Os números originais (empuxo do foguete, mísseis teleguiados do bando) estão no código nativo e não foram recuperados; veja `docs/rio/ROCKET_E_SUPERPOWERUP.md`. Na tela aparece a arte do pássaro substituto.
- `SPRITES.BIN` com 29 bancos; `MAX_LEVEL_OBJECTS` 896 (uma fase de recompensa tem 862 peças).

## Conferido
- Uma fase de cada grupo no emulador (Playground 1, Reward 8/9/23, Bonus 1/30/33, Timber Tumble 1).
- Cartões do menu: a arte dos cartões de Rocket Rumble, Blossom River, Timber Tumble, Hidden Harbor e Treasure Hunt combina com o cenário das fases, o que confirma a correspondência entre pacotes e episódios deduzida na 0.51.0.

## Limitações
- Os pássaros especiais são aproximações (acima).
- Fases sem roteiro de vitória verificado; camadas de tema ilegíveis seguem como na 0.51.0.
