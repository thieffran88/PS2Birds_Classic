# Modificações — 0.94.23 (Classic) e 0.49.13 (Rio)

## Investigado (Rio: mira)
- **Esticar máximo da borracha, lido:** no `gamelogic.lua` do Rio (decompilado do bytecode), `launchBird` usa `razão = min(1, comprimentoDaBorracha / rubberBandMaximumLength())`, com `rubberBandMaximumLength = shootRange + 3,2` e `shootRange = 2,2` (nenhuma fase o muda): o máximo é 5,4 unidades de física. O impulso é `direção × (−defaultForce × physicsScale × massa) × razão`, com `defaultForce = −925` (`updateValues`) e `physicsScale = 1/20` (todas as 459 fases têm `physicsToWorld 20`). A velocidade de saída é, portanto, 46,25 × razão para qualquer massa, a mesma do Classic. Só o `RocketBird` multiplica por 1,42 (1,04 a mais com superseed): o port ainda não tem esse pássaro (6 fases).
- **Conferido no port com os dados reais do Rio** (`ps2/tests/rio_aim_check.cpp`): 540 tiros (potências 20 a 100, ângulos 5, 30 e 60) em fases do Rio: erro de velocidade 0,000 %; os pontos da prévia da mira ficam exatamente sobre o voo real (0,000 unidades em voo livre).
- A razão mínima do port é 0,2 (potência 20); no original a esticada vai de 0 a 1.

## Adicionado
- `ps2/tests/rio_aim_check.cpp`, `docs/MODIFICACOES_0.94.23.md` e `docs/rio/MODIFICACOES_RIO_0.49.13.md`.

## Editado
- `ps2/tests/AUXILIARES.md`. O jogo não mudou.

## Excluído
- Notas da versão anterior.

## Não feito
- A gravidade real do Rio (pendência 56) continua sem fonte: não está em nenhum script do Rio (é nativa, como no Classic); ficou +20.
