# Modificações — Classic 1.2.0 (episódio extra "Year of the Goat")

O Rio continua na 0.50.3.

## O que é
O pacote `packTalkweb_1_1` é o episódio da versão chinesa do Classic (parceiro Talkweb; botão `BTN_YEAROFTHEGOAT`, "Ano da Cabra"). Não faz parte da versão internacional que o port seguia, mas as 15 fases (`TWLevel01` a `TWLevel15`) não usam nenhuma peça sem adaptador, então entra como **episódio extra offline**, depois da Mighty League.

## Adicionado
- Episódio "YEAR OF THE GOAT" (o 21º do menu): 15 fases, na ordem do `episodes.lua`, com um banco de sprites próprio (banco 81; união de 1,2 MB). `SPRITES.BIN` com 82 bancos.
- Cartão no menu: a imagem do cartão genérico (a mesma do Mighty Feathers, como a da Mighty League, porque as páginas do menu estão cheias) com o título `BTN_YEAROFTHEGOAT_TEXT`. `MENU.BIN` com 109 sprites; `menu_gfx.h`: as recompensas vêm depois de 21 cartões.
- `docs/MODIFICACOES_1.2.0.md` (este arquivo).

## Editado
- `tools/import_level.py`: a gravidade das fases que não trazem `worldGravity` (várias das Talkweb) passa a ser 20, o valor do motor. Antes o importador recusava a fase.
- `profiles/classic_episode.json` (página 85), `ps2/tests/campaign_test.cpp` (1.003 fases, `unverified==43+243+15`), `menu_assets_test.py` (21 cartões), `themes_assets_test.py` (`themeTalkweb1` sem tema próprio) e `sprite_test.py` (82 bancos).
- Versão do Classic 1.2.0.

## Limitação conhecida
O `themeTalkweb1` não tem entrada no `themes.lua` nem no `themes_new.lua`: as camadas dele (montanhas, plataformas) só existem como imagens (`THEME_TALKWEB_1_*`) e uma composição, sem a tabela que diz onde ficam. Para não inventar o cenário, as 15 fases usam o cenário simples embutido (céu, sol e colinas) e o terreno de fallback. As peças e a física são as do original.

## Conferido
- As 15 fases assentam sem quebrar peças (como as 243 da Mighty League; nenhuma tem roteiro de vitória verificado).
- Emulador: o cartão do episódio (0/45 estrelas) e a fase "YEAR OF THE GOAT 3".
