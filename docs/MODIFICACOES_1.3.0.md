# Modificações — Classic 1.3.0 (episódio extra "Siege")

O Rio continua na 0.50.3.

## O que é
O pacote `siege` (`SLevel1` a `SLevel8`, `Siege1`) é um protótipo que o Classic nunca lista nos episódios (`g_siege_proto = false` no `game_conf`). Entram como **episódio extra offline** as 3 fases que o port consegue montar com o que o jogo tem: `SLevel7`, `SLevel8` e `Siege1` (tema `themeSiege`/`FaceLift`, pássaros Black, Basic e Yellow).

## Adicionado
- Episódio "SIEGE" (o 22º do menu): 3 fases, com banco de sprites próprio (banco 82). `SPRITES.BIN` com 83 bancos.
- Cartão no menu: a imagem do cartão genérico, sem título (o episódio não tem arte de título). `MENU.BIN` com 111 sprites; `menu_gfx.h`: as recompensas vêm depois de 22 cartões.
- `docs/MODIFICACOES_1.3.0.md` (este arquivo).

## Editado
- `tools/import_level.py`: as peças `DecorationFaceLift*` de `SLevel7` e `SLevel8` não têm definição em nenhum script do APK (nada é desenhado por elas no original); o importador as descarta nas fases do `siege`.
- `profiles/classic_episode.json` (página 86), `ps2/tests/campaign_test.cpp` (1.006 fases, `unverified==43+243+15+3`), `menu_assets_test.py` (22 cartões), `sprite_test.py` (83 bancos), `themes_assets_test.py` (temas do siege sem entrada em `themes.lua`).
- Versão do Classic 1.3.0.

## Fora
- `SLevel1` a `SLevel6` (6 fases): usam `EmptyPlaceholderBird`, um espaço do `BirdSelectPool` (o jogador monta o bando com 7 pontos de custo). O port não tem essa escolha de pássaros; sem ela essas fases não têm fila de pássaros.

## Limitação conhecida
`SLevel7`/`SLevel8` e `Siege1` usam o cenário simples embutido (os temas `themeSiege`, `theme01FaceLift` e `theme02FaceLift` não têm entrada em `themes.lua`). Nenhuma das três tem roteiro de vitória verificado; as três assentam sem quebrar peças.

## Conferido
- `campaign_test`: 1.006 fases em ordem, contagens, travas; as três do Siege carregam e assentam sem perder porcos nem pontuar.
- A investigação da `FlockFavorites09` (junta tipo 5 com âncora distante) não gerou mudança: a solda ancorada no corpo-alvo consertava essa fase, mas derrubava fases com roteiro verificado (`Level695`, `Level974`, `FlockFavorites25`), então foi desfeita.
