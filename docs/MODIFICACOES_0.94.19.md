# Modificações — 0.94.19 (Classic) e 0.49.9 (Rio)

## Adicionado
- **Botão de pausa na tela** (`ps2/src/main.c`): quadrado laranja com as duas barras brancas no canto esquerdo, sob o nome da fase, com a dica `START` ao lado (o `INGAME_BUTTON_PAUSE` do `GameHud.lua`). O PS2 não tem toque: é só o indicador, a pausa continua no START.
- **Visor de power-ups** (`ps2/src/main.c`): uma coluna de discos coloridos com o rótulo, sob o botão de pausa. Mostra o power-up da fase (Estilingue Real sempre ligado; Poção, Onda de Choque e Terremoto com `L2`, apagados depois de usados) e os da loja escolhidos antes da fase (Chapéu Teleguiado com `R2`, aceso só em voo e uma vez; Pontos em Dobro; Pássaro Extra na cor do pássaro). Substitui o texto solto que ficava no alto da tela.
- Capturas na pasta `Capturas` do Drive.
- `docs/MODIFICACOES_0.94.19.md` e `docs/rio/MODIFICACOES_RIO_0.49.9.md`.

## Excluído
- Notas da versão anterior.

## Aproximado
- O botão e os discos são desenhados com formas simples e a fonte de pixels, não com os sprites originais (`INGAME_BUTTON_PAUSE`, `INGAME_POWERUP_*`): a página de interface (`UI.BIN`) e a memória de vídeo estão cheias.
- Não é um botão de toque; não há contador de usos (a loja do original tem quantidades; no port o uso é ilimitado).
