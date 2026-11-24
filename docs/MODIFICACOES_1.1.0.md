# Modificações — Classic 1.1.0 (episódio extra Mighty League, offline) e Rio 0.50.3

O Classic sai da 1.0.0 porque ganha conteúdo: o **episódio "MIGHTY LEAGUE"**, com 243 fases jogáveis offline. O Rio ganha só a correção de memória abaixo (0.50.3).

## O que é
A Mighty League é o modo online de liga do Classic. O APK traz as 273 fases dela (`MightyLeagueLevels`), mas o ranking, os ingressos e as recompensas dependem de servidor. Aqui só as **fases** entram, como um episódio comum, sem ranking nem ingressos.

## Adicionado
- Episódio "MIGHTY LEAGUE" (o 20º do menu): **243 fases** (de 273), na ordem do `episodes.lua` (`Level2000_n` primeiro, depois `LevelPanu_n`, `TemeLevel_n` etc.), com o tema `themeMLExtra` e a trilha de ambiente do tema.
- 16 páginas, cada uma com o próprio banco de sprites (bancos 65 a 80): as fases usam peças de vários episódios, e uma união só passaria do limite de memória de vídeo. `SPRITES.BIN` do Classic: 195 MB (era 147 MB).
- 29 adaptadores de peça em `profiles/classic_level.json` e `sprite_metrics.json`, gerados de `blocks.lua` com `tools/add_definitions.py`: decorações da liga (`DecorationMightyleagueExtra*`, grama e plantas, sem colisão), peças temáticas `MISC_THEMED_*` (madeira, esferas, abóbora, versões "candy"), moedas, enfeites de ouro, presentes de Natal, lata de refrigerante, barril nuclear, bola de plasma, flor 6, pato de borracha, óculos do rei, três balões e uma pedra de fazenda.
- Cartão do episódio no menu: `BTN_MIGHTYLEAGUE_BACKGROUND` com o título `MIGHTY_LEAGUE_LOGO_TEXT_BR` (`tools/extract_menu.py`, `MENU.BIN` com 107 sprites; `menu_gfx.h`: as recompensas passam a vir depois de 20 cartões).
- `docs/MODIFICACOES_1.1.0.md` (este arquivo).

## Memória: o `CAMPAIGN.BIN` passa a ser lido por partes (Classic e Rio)
Com as 988 fases o `CAMPAIGN.BIN` do Classic chegou a 14 MB. O carregador lia o arquivo inteiro para a memória e ainda montava o vetor de peças (outros ~16 MB), então a tela "Arquivos de dados ausentes" mostrava `CAMPAIGN.BIN: ERRO 3` (memória) no emulador. Agora `game_campaign_load` lê só a tabela de fases e as juntas, polígonos, forças e textos (poucos MB); **as peças de cada fase ficam no disco e a fase aberta é lida na hora** (`campaign_objects`, algumas centenas de registros por fase). Vale para o Rio também (Rio 0.50.3, mesmo motor); não muda nenhum comportamento de jogo.

## Editado
- `profiles/classic_episode.json`: 16 páginas novas (69 a 84).
- `ps2/tests/campaign_test.cpp`: tabela com as 243 fases (988 no total), `unverified==43+243` e as fases da liga só carregam e assentam (sem tiro de teste, que levava minutos). `ps2/tests/menu_assets_test.py`: 20 cartões.
- `ps2/src/game.cpp`: carregador lazy descrito acima (`campaign_parse`, `campaign_objects`, `campaign_read`).
- `ps2/tests/sprite_test.py`: 81 bancos, 747 sprites. `tools/extract_menu.py`: o cartão da liga reaproveita a imagem do cartão genérico (apelido `ALIAS:`), porque as páginas do menu estão cheias.
- Versões: Classic 1.1.0 e Rio 0.50.3 (`GAME_VERSION` em `main.c`).

## Que fases ficaram de fora e por quê
- **29 fases instáveis:** quebram peças sozinhas nos primeiros quadros (pontuam sem tiro e a física fica lenta). São fases da comunidade com peças sobrepostas no início. Entre elas `Level2000_1`, `_2`, `_3`, `_4`, `_6`, `_8`, `_12`, `_21`, `_127`, `_136`, `_137`, `_148`, `_151` e outras 16. Em vez de entregar fases estragadas, deixei de fora; a lista está em `docs/MIGHTY_LEAGUE_FORA.txt`.
- **1 fase** (`Level2000_87`) usa o canhão de bolas de neve (`MISC_FB_SNOWBALL_CANNON`), cujo sprite (`MISC_SNOWCANNON`) não existe no APK.
- Sem servidor: ranking, ingressos, recompensas e o ciclo de fases da liga não existem.

## Falta
- Nenhuma das 243 fases tem roteiro de vitória verificado (o mesmo caso das 43 do Classic sem roteiro): não sei se todas são vencíveis na física do port.
- Conferi no emulador o cartão do episódio ("LIGA PODEROSA", 20º, 0/729 estrelas) e a fase "MIGHTY LEAGUE 8"; não passei fase por fase dos 16 bancos.
- O Rio foi conferido com a fase "BEACH VOLLEY 1" depois da mudança do carregador.
