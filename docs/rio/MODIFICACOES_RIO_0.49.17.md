# Modificações do Rio — 0.49.17

Cenas em quadrinhos e créditos do Rio. O Classic continua na 1.0.0, sem mudança de comportamento (`scenes.c` só troca as tabelas dentro de `#ifdef PS2BIRDS_RIO`).

## Adicionado
- `tools/rio/extract_cutscene_rio.py`: gera o `CUTSCENE.BIN` do Rio com as 33 imagens `CUTSCENE_IMAGE_n` (`CUTSCENES_SHEET_1..4`, conjunto 864×480). O `CutsceneScene.lua` do Rio monta 7 animações (`theme1Start` e `theme1End`..`theme6End`): cada uma é uma lista de estados de 2 s, e cada estado mostra uma imagem nova sem apagar as anteriores. As imagens ficam no centro da tela deslocadas pelo pivô de cada quadro (o lugar dele na página) e todas as imagens de uma animação são reduzidas juntas para caber em 85 % × 95 % da tela (`layout()`). A tabela das animações foi lida do script decompilado, porque o script a monta em código e o leitor de dados não executa código. Saem como cenas de painéis (o modo que o runtime já tinha, opção bit 2): `rio_intro`, `rio_end1`..`rio_end6`, 1,69 MB.
- `docs/rio/MODIFICACOES_RIO_0.49.17.md` (este arquivo).

## Editado
- `ps2/src/scenes.c`: no Rio, "SMUGGLERS DEN" abre com `rio_intro` (entrar na primeira fase do episódio sem pontuação) e fecha com `rio_end1` (depois da primeira pontuação que completa as 30 fases). As outras cenas finais estão no `CUTSCENE.BIN`, mas os episódios delas ainda não estão no port.
- `tools/rio/extract_menu_rio.py`: o texto "sobre" do `MENU.BIN` agora é o `TEXT_ABOUT_RIO` do APK, e a imagem da tela é a `IMG_CREDITS_2`.
- `ps2/src/main.c`: a tela de créditos escreve o nome do jogo ("PS2Birds Rio") em vez de "PS2Birds Classic" fixo; versão do Rio 0.49.17.
- `build_rio_data.py`, `gerar_iso.py` e `make_windows_kit.py`: geram, gravam na ISO e levam no kit o `CUTSCENE.BIN` do Rio.

## Conferido no emulador (capturas na pasta "Capturas")
- Entrando na primeira fase do Smugglers' Den, a abertura toca e os painéis aparecem um por um, a cada 2 s.
- Tela "sobre" do Rio com o texto do APK e a versão.
- Conferi todas as 7 cenas só renderizando o último quadro (disposição dos painéis); no jogo vi a `rio_intro`.

## Falta
- Os créditos longos (`TEXT_CREDITS`, que o `CreditsPage.lua` rola) e as logos de abertura (Rovio, Fox, Blue Sky): a tela "sobre" mostra só o texto de direitos.
- As cenas `rio_end2`..`rio_end6` só tocam quando os episódios 2 a 6 entrarem no port.
- O fundo das cenas é preto; no original é o fundo do menu principal, e a primeira cena escurece o fundo.
- A música é a `story_music` (trilha INTRO do `MUSIC.BIN` do Rio), tocada uma vez, sem conferir o áudio (o emulador daqui não toca som).
