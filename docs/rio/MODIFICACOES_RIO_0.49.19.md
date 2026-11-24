# Modificações do Rio — 0.49.19

Dois dos quatro pássaros do Rio que faltavam: **Blu** (`BIRD_RIO_BLUE_ALONE`, 27 fases dos pacotes ainda fora do port) e **Blu & Jewel** (`RioBirds`, 13 fases). O Classic continua 1.0.0: os tipos novos só aparecem em fases do Rio, e `ps2/tests` do Classic passam sem mudança.

## Adicionado
- `ps2/src/game.h` / `game.cpp`: tipos `BIRD_BLU_TYPE`, `BIRD_JEWEL_TYPE` e `BIRD_ROCKET_TYPE` (este ainda sem comportamento) e as constantes de cada um.
- **Blu** (`birds.lua` + `activateBluSpecialty` e o `specialtyActive` do `gamelogic.lua`, lidos do script decompilado): corpo raio 1,4, densidade 1,5, atrito 0,8, restituição 0,3. Ao toque ele fica `specialtyDuration` = 0,9 s em um padrão de estrela: a cada `specialtyInterval` = 0,2 s há uma explosão sem dano (raio 9, força 4000) e a velocidade dele passa a ser massa × `specialtyImpulse` (3) numa direção girada 4π/5 a cada passo (a primeira é a direção em que ele voava); a gravidade fica anulada. No fim ele segue com (última velocidade x ÷ 5, −10).
- **Jewel** (`RioBirds`, especialidade `BLU&JEWEL`): é o boomerang do Classic (mesmo corpo, mesmas fórmulas de força), com as constantes dela: força horizontal 1000, vertical −13050, velocidade horizontal máxima 100, força de voo −180 (absoluta, antes do toque). Sem a rotação do boomerang.
- `ps2/tests/rio_birds_test.cpp` (no `make test`): Blu fica 108 quadros a 120 Hz = 0,9 s à velocidade massa × 3 e só toca uma vez; Jewel passa de 46,25 m/s depois do toque e fica limitada pelo máximo. `game_force_bird_type` (só para testes de host) força o tipo do pássaro do estilingue.
- `docs/rio/MODIFICACOES_RIO_0.49.19.md` (este arquivo).

## Editado
- `tools/rio/rio_campaign.py`: `BIRD_RIO_BLUE_ALONE` → 11 e `RioBirds` → 12.
- `tools/rio/extract_sprites_rio.py` e `pack_sprites_rio.py`: os dois pássaros entram no banco de sprites e na tabela de pássaros por tipo (13 linhas). O sprite de ação de Blu (`BIRD_BLU_FLAPPING_n`) é uma composição de corpo e asas que não está nas folhas, então durante o dash ele desenha o sprite de voo; Jewel usa `BIRD_JEWEL_FLY_WINGSUP`. Banco: 153 sprites, 1.575.168 bytes.
- `ps2/src/main.c`: versão do Rio 0.49.19.

## Falta
- **RocketBird** (6 fases) e **SUPERPOWERUP_BIRD** (3 fases) continuam sem tipo, e as fases deles continuam fora. O foguete é um corpo composto (pássaro + `BOTTLE_ROCKET_BIRD` unido por junta, empuxo 2880 por 1,6 s e explosão raio 8, força 10500, dano 400 até 3 m); o pássaro do power-up chama um bando (`makeSuperpowerBirds`). Os dois pedem corpos extras que o runtime ainda não tem.
- As 40 fases que usam Blu e Jewel estão em pacotes que o port ainda não inclui (aeroporto, carnaval, praia, mercado de frutas, selva 2), então hoje nenhuma fase do jogo usa os dois. Eles entram em jogo quando esses pacotes entrarem (pendência 75).
- Conferi o comportamento no teste de host, não no emulador: o teste em jogo (fase da selva com Blu forçado) mostrou o Blu no estilingue, mas a fase de teste ficou parada e não vi o dash na tela.
- Dano dos dois contra as peças usa o fator padrão; o `RioBirdDamageFactors` do Rio ainda não foi importado.
