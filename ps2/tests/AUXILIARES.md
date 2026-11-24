# Auxiliares de desenvolvimento dos testes

Estes arquivos **não fazem parte do ELF nem do `make test`**. São as ferramentas usadas para
achar e recalcular os roteiros de vitória da tabela de `campaign_test.cpp` quando a física do
jogo muda (foi assim em 0.90.0, com o `velocityMultiplier`). Todos rodam no PC, usando o mesmo
`game.cpp` do jogo, e precisam dos dados gerados em `ps2/assets` e `ps2/data`
(`python3 tools/gerar_iso.py APK --dev`).

| Arquivo | Para que serve |
| --- | --- |
| `solver.cpp` | Busca, para cada fase, um tiro (ângulo, força e momento da habilidade) que, repetido para todos os pássaros, vence. Uso: `solver [primeira_fase] [última_fase]`. |
| `solver3.cpp` + `beam.py` | Busca em feixe para as fases em que um tiro repetido não basta: testa sequências de pássaros diferentes. Uso: `beam.py FASE [LARGURA]`; imprime `WIN` seguido de ângulo, força e habilidade de cada pássaro e da pontuação. |
| `solver4.cpp` | Busca em feixe sobre a sequência inteira de pássaros, num único processo (bem mais rápida que o `beam.py`): grade mais grossa, orçamento de tempo e quatro momentos de habilidade. Uso: `solver4 FASE [LARGURA] [SEGUNDOS]`; imprime `WIN` com ângulo, força, habilidade e espera de cada pássaro, ou `none`. `WAITS=0,2,5` testa esperas antes do lançamento (Egg Defender). Resolveu 19 fases na 0.94.15. |
| `rio_aim_check.cpp` | Confere a mira do Rio contra o estilingue original (`gamelogic.lua` do Rio): velocidade de lançamento = 46,25 × esticada para qualquer potência e ângulo, e pontos da prévia sobre o voo real. Precisa dos dados do Rio: `PS2BIRDS_CAMPAIGN=CAMPAIGN.BIN rio_aim_check`. |
| `ed_conference.py` | Confere os 72 controladores do Egg Defender (`egg_vehicles.cpp`) contra os scripts originais decompilados: nomes, nomes de peças, números e `never_abandon`. Uso: `ed_conference.py PASTA_COM_OS_.scripts.lua`. |
| `ed_solver.cpp` | O mesmo passo da busca em feixe para o Egg Defender, onde a espera antes do lançamento importa, porque os veículos chegam em ondas. Uso: `ed_solver fase [espera ângulo força habilidade]...`. |
| `settle_probe.cpp` | Mede quanto as peças de cada fase se movem ao assentar, antes de qualquer tiro. |
| `make_campaign_table.py` | Reconstrói a tabela de fases do `campaign_test.cpp` a partir dos resultados das buscas (`ps2/build/all_*.txt` e `nb*_<índice>.txt`). Fases sem vitória ficam com roteiro vazio e entram no teste como "sem roteiro". |

## Receita usada na 0.90.0

1. Rodar `CAMPAIGN_LIST=1 ./build/campaign_test` para listar as fases (`FAIL level N`) que deixaram de vencer.
2. Rodar o `solver` nessas fases (em paralelo, uma por processo).
3. Para as que sobrarem, rodar `beam.py` (cada uma pode levar dezenas de minutos).
4. Gravar os roteiros achados na tabela do `campaign_test.cpp` e ajustar a contagem esperada de fases sem roteiro (`unverified`).

## Fumaça do Rio (`rio_smoke.cpp`)

Carrega um `CAMPAIGN.BIN` do Rio (gerado por `tools/rio/rio_campaign.py`) pela variável `PS2BIRDS_CAMPAIGN`, deixa a primeira fase assentar, dá um tiro e imprime os contadores. Compilar com `g++ -O2 -std=c++11 -Isrc -c tests/rio_smoke.cpp` e ligar com `build/host/game.o` e os objetos do Box2D em `build/host/vendor` (depois de um `make test`); rodar com `PS2BIRDS_CAMPAIGN=caminho/CAMPAIGN_RIO.BIN`.
