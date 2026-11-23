# Modificações — 0.94.13 (Classic) e 0.49.3 (Rio)

## Editado
- **Pontos de dano por colisão** (`ps2/src/game.cpp`): cada batida que causa dano soma `floor(dano aplicado) × 10` à pontuação (`CollisionCallbacks.lua`); numa batida que destrói a peça vale o que restava de resistência. O dano em si continua a fórmula aproximada (por isso os roteiros de vitória não mudam). Os primeiros resultados ficam na mesma ordem de grandeza dos limites de estrela (a 1-3 chega a 40.780 contra 41.000 do ouro).
- **Fórmula exata de colisão, experimental e desligada:** `-DPS2BIRDS_EXACT_COLLISION` troca o dano pela fórmula do `AdventureModeCollisions.lua` com a velocidade em unidades de mundo (×20), divisor 10 e multiplicador 20 no chão. Resultado: na busca simples a mesma amostra de 125 fases vence em 57% (contra 72% com a fórmula atual), então o jogo ficaria mais difícil sem evidência de que o original seja assim. Por isso fica desligada. Ela concorda com a memória de que o Red quebra vidro numa batida (resistência 26 a 40, dano ≈ 90).
- `ps2/tests/solver.cpp` (variável `SOLVER_FAST`), `ps2/tests/tnt_test.cpp`, `docs/LACUNAS_PORT.md`.

## Adicionado
- `docs/MODIFICACOES_0.94.13.md` e `docs/rio/MODIFICACOES_RIO_0.49.3.md`.

## Excluído
- Notas da versão anterior (`MODIFICACOES_0.94.12.md`, `MODIFICACOES_RIO_0.49.2.md`).

## Ainda aproximado
- O dano de colisão e, por isso, a contagem de estrelas. Pendência 33b.
