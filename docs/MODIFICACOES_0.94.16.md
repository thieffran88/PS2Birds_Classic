# Modificações — 0.94.16 (Classic) e 0.49.6 (Rio)

## Investigado
- **`setTargetY` do Egg Defender:** o método existe em `EggDefenderVehicle.lua` (guarda uma altura-alvo e duas forças e `doAutoTargeting` as aplica a cada quadro), mas nenhum dos 2.057 scripts do APK o chama, nem o código nativo o menciona. É código morto, não há o que portar. O `scriptTargetY = -45` da fase 6 (onda 3) também não é lido por nada.
- **Conferência dos veículos contra o original:** os 72 scripts de veículo têm controlador no port e vice-versa; as 222 peças/porcos distintos citados nos scripts aparecem no port (os porcos só em `setAbandoningLogic`, `setGrabLogic` e `surrender`, modelados por papel); todos os números dos scripts aparecem; `never_abandon` bate com os `setAbandoningLogic`. Nenhuma divergência.

## Adicionado
- `ps2/tests/ed_conference.py` (ferramenta de desenvolvedor) e `docs/MODIFICACOES_0.94.16.md`, `docs/rio/MODIFICACOES_RIO_0.49.6.md`.

## Editado
- `docs/EGG_DEFENDER.md`, `docs/LACUNAS_PORT.md` e `ps2/tests/AUXILIARES.md`. O jogo não mudou.

## Excluído
- Notas da versão anterior.

## Não feito
- Comparar o comportamento de cada fase com o jogo original rodando: o APK antigo não instala nos aparelhos novos e aqui não há como executá-lo.
