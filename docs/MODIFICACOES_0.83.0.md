# Modificações completas — PS2Birds Classic 0.83.0

## Grito dos pássaros na tela inicial (`main_menu.lua`, `yell_sprite`)
- No original, tocar num pássaro que voa na tela inicial o faz gritar: o sprite troca para a versão `_YELL` e toca o grito.
- O PS2 não tem toque, então o **Triângulo** faz isso: o pássaro em voo mais perto do centro da tela grita por 0,7 s.
- O som é o grito de lançamento do próprio pássaro, que já estava no `SOUNDS.BIN`. Por isso não foi preciso gastar mais memória de som.
- A dica no rodapé da tela inicial agora mostra "TRIANGULO: GRITO".

## Arte e dados
- O `extract_menu.py` grava os 7 sprites `_YELL` (vermelho, azul, amarelo, preto, verde, irmão grande e bumerangue) em 80% do tamanho. Em 100% não cabiam na memória de vídeo.
- O `MENU.BIN` passou de 98 para 105 sprites.
- No `menu_gfx.h`, entrou o novo `MG_YELL_BIRD`.
- O teste `menu_assets_test.py` foi atualizado.

## Investigação desta rodada (sem código mantido)
- Testei aplicar a tabela `velocityMultiplier` do `blocks.lua` (quanto da velocidade o pássaro conserva ao quebrar uma peça).
- Resultado: 86 fases deixaram de ser vencidas pelos roteiros verificados, que foram calibrados na física atual. O código foi desfeito.
- A tabela completa e o resultado estão anotados em `LACUNAS_PORT.md`.

## Conferido no PCSX2
- O console registra o grito ao apertar Triângulo na tela inicial.
- A tela inicial mostra a nova dica no rodapé.

## Limpeza
- Removidos os documentos da 0.82.0 (substituídos pelos da 0.83.0).
- Removidas da área temporária as pastas e os zips da 0.82.0, os logs e as capturas.

## Versão
- Tela de pausa, "Sobre" e README: 0.83.0.
