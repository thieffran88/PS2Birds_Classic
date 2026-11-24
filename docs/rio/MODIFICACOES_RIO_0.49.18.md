# Modificações do Rio — 0.49.18

Música de cada fase do Rio. O Classic continua na 1.0.0, sem mudança.

## Editado
- `tools/rio/rio_campaign.py`: cada fase grava no `CAMPAIGN.BIN` a trilha do tema (`music`). O `themes.lua` do Rio dá a música de cada tema (`music`); a tabela `CLIP_TRACK` liga cada clipe à faixa do `MUSIC.BIN` do Rio: `ab rio ambience warehouse` → CAVE, `ambience_jungle` → JUNGLE, `ambience_beach` → BEACH, `ambience_carnival` → BIRTHDAY, `ambience_airport` → CITY. Antes todas as fases gravavam -1 e tocavam sem música.
- `ps2/src/main.c`: versão do Rio 0.49.18. O runtime não mudou: ele já tocava `game_level_music` da fase.

## Adicionado
- `docs/rio/MODIFICACOES_RIO_0.49.18.md`.

## Conferido
- No emulador: a primeira fase do Smugglers' Den (tema `theme2`) toca a trilha 7 (CAVE, o ambiente do galpão) e a primeira da selva (`theme3`) toca a 3 (JUNGLE); o log mostra "ambience 7 playing" e "ambience 3 playing". Não ouvi o áudio, porque o emulador daqui não toca som.

## Falta
- Os 45 stages do port só usam os temas `theme2` e `theme3`. Os clipes dos outros temas (`ambience_harbor`, `ambience_rocketrumble`, `ambience_goldenbeachball`, `ambience_blossomriver`, `ambience_birdparadise`, `ambience_airplane`, `plane_loop04`) existem no APK ou não, mas não têm faixa no `MUSIC.BIN`: entram junto com os episódios deles.
- O volume por tema (`musicVolume`, 0,7 na selva e nos outros ambientes, 1,0 no galpão) não é aplicado: o runtime toca todas as trilhas no mesmo volume.
- A trilha do menu é a mesma da 0.49.17 (`angry_birds_rio2_theme`); a música de vitória do Rio não tem arquivo próprio no APK.
