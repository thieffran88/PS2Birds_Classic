# Modificações completas — PS2Birds Classic 0.65.0

## Egg Defender: primeiras fases vencidas
- Novo `ps2/tests/ed_solver.cpp`: busca tiro a tiro (espera antes do tiro, ângulo, força e momento da habilidade), porque os veículos chegam em ondas.
- Egg Defender 6, 9, 10 e 12 vencidas com roteiros encontrados pela busca; agora fazem parte do teste da campanha.
- As outras 11 fases terminam em derrota com a grade atual (tiros a cada 5° e esperas de 0 ou 3 s). Na fase 1 (tutorial) o primeiro veículo fica parado perto do ovo; as ondas seguintes só entram depois que o porco dele morre.

## Teste da campanha
- Cada tiro do roteiro tem agora um tempo de espera (`wait`) antes do lançamento; os roteiros antigos usam 0.
- Fases sem roteiro vencedor: de 77 para 73.

## Investigação (decompilação)
- Power-ups da loja decompilados com o unluac: Double Score (pontos dobrados das peças quebradas; bolhas não pontuam), Extra Bird (lógica no `ExtraBirdManager.lua`), Homing Hat, Chrono Bird e Boom Box. Nenhuma fase os traz: no original são escolhidos na loja, então o port precisa de um menu de seleção para usá-los.

## Limpeza
- Removidos cerca de 800 arquivos temporários de buscas antigas da pasta de build (logs `nb*`/`all_*`, scripts `beamq*`/`runq`, `old_scripts`, sondas).

## Versão
- Tela inicial e README: 0.65.0.
