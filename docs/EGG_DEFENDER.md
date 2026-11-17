# Egg Defender — plano da tradução manual

O modo Egg Defender (15 fases `EggDefender_N` do Mighty Feathers) é feito de ondas de veículos de porcos que tentam roubar o ovo. Cada fase tem um `.scripts.lua` só com dados (ondas, limite de pássaros); cada um dos **72 veículos** (`assets/data/vehicles/EggDefender_N/EggDefender_N_Wk_V1`) tem a montagem (peças e juntas, só dados) e um `.scripts.lua` **com código**: um pequeno controlador que o motor do modo chama.

Como o gerador do port nunca executa código do jogo, cada controlador é **traduzido à mão para C++** e compilado no ELF. As traduções são código do port (não contêm dados do jogo); a montagem dos veículos continua vindo do APK do jogador.

## O que um controlador faz

Callbacks chamados pelo motor (`EggDefenderVehicle.lua`): `init(v)`, `update(v)` (todo quadro), `eggCollected(v, ...)`, `blockRemoved(v, bloco)`, `abandon`/`flee`/`surrender`/`release`, `pigTargeted`, `customAbandoningLogic`. Operações usadas, com a linha em `EggDefenderVehicle.lua`:

- Peças: `getBlock(nome)` com `forceRelative` (força do ventilador ao longo da peça), `forceX`/`forceY`, `targetVelocity`; `togglePower(nome, ligado)` (2056); `removeBlock` (1564); `removeJoint` (1546); `updateJointDefinition` (2394).
- Navegação: `v.x`, `v.y`, `getClosestEggX()` (2411), `getClosestSlingshotX()`, `toggleAutoBalance` (1873), `rotorDescend`.
- Engenhocas: `triggerGadgetFunctionality` (2378), `updateGadgetProperties`, `startAim`.
- Porcos: `getPig(nome)`, `setAbandoningLogic`, `abandonVehicle()` (1972), `surrender`.
- Tempo: `doWithTimer(função, segundos)` (1203).

## Regras do modo (EggDefender.lua)

- **Ondas:** cada fase tem 3 a 5 ondas, cada uma com um veículo (`waves[k][1] = {type, time, x, y}`). `spawnNextVehicle` tira a primeira onda da fila e cria o veículo; a próxima onda entra quando a lista `vehicles` fica vazia (veículo destruído, abandonado ou fugido).
- **Vitória (`checkLevelComplete`):** sem ondas e sem veículos, com o ovo ainda preso ao ninho (`hasJoints(egg)`).
- **Derrota (`checkLevelFailed`):** o ovo foi roubado (`onEggStolen`) ou acabaram os pássaros (`birdLimit`, de 7 a 13).
- **Entrada do veículo (`EggDefenderVehicle.init`):** o ponto (x, y) da onda é o pivô do veículo (`VehiclePivotPoint`: canto inferior esquerdo ou centro esquerdo); com `spawnAtGroundLevel` o veículo é baixado até o chão. `findRightmostBlock` serve para saber quando ele entrou na área de jogo.
- **Peças que faltam no port:** `BP_EGG`, `BP_NEST`, `BP_EGG_SIGN_POLE`, `BP_ROCKET_RED/BLUE`, `BP_CANNON`, `BP_BALLOON_INFLATER(_VIOLET)`, `BP_COG_OFF`, `BP_BELLOWS`, `SpecialBlockStart`. Maior fase: 309 objetos contando os veículos de todas as ondas (cabe no limite de 640).

- **Pássaros:** `getBirdToSpawn` sempre devolve `"RedBird"`; o jogador tem `birdLimit` pássaros vermelhos (7 a 13), que o modo cria um a um (o cenário não traz pássaros).
- **Cenário:** o ninho (`BP_NEST`) e o ovo (`BP_EGG`) ficam à esquerda (x ≈ −85 a −93); os veículos entram pela direita (x da onda de 41 a 75), com coordenadas próprias em torno da origem do arquivo do veículo. A fase 1 ainda tem peças `SpecialBlockStart` e `ExtraStolenEgg` da cena de abertura (`removeUnusedLevelStartCinematicBlocks`).
- **Estilingue:** as fases não têm `birdZeroPos` nem pássaros; a posição do estilingue vem do modo (`EggDefender.onLoadLevel`, linha 750) — próximo ponto a ler.

## Andamento

- **Fase 1 (dados) — feita na 0.54.0.** As 15 fases entram no port (página 68, banco 64). Os veículos das ondas são mesclados ao cenário com o número da onda (bits 26-29 das marcas); o da primeira onda já está na fase, os outros entram quando os porcos da onda atual acabam. Só os porcos dos veículos contam como alvo; a fila é de `birdLimit` pássaros vermelhos. Peças soltas da cena de abertura à direita são removidas, e o veículo desce até a superfície (exceto os 22 com `spawnAtGroundLevel = false`, em `profiles/egg_defender.json`). Os veículos ainda não se movem: os controladores são a fase 3.
- **Fase 2 (motor) — feita na 0.55.0.** Lido o código nativo (`libAngryBirdsClassic.so`, x86) que empurra as peças: com `forceRelative` diferente de zero a força é `forceRelative × (cos ângulo, sen ângulo)`, no eixo x da própria peça; senão vale `forceX`/`forceY`. `targetVelocity` corta cada componente quando a velocidade naquele eixo já chegou ao alvo no mesmo sentido; com `applyForceInterval` (fole) vira um sopro por intervalo; `disableForce` (`togglePower`) desliga. Posição dos veículos corrigida pelo código: a primeira onda fica em (x da onda + x do estilingue) e as outras são teleportadas 2,5 m à direita da tela (`teleport`); a câmera mostra uma tela de 91 m a partir do estilingue, e a cena de abertura sai depois de `rightLimit`. Ovo: o porco pega o ovo a menos de 5 m na horizontal e 10 m na vertical (salto de 0,75 s, depois soldado ao porco); ovo solto rola de volta (`dx × 0,25 / max(1; 0,06 dy)`) e salta para o ninho; se o ovo sai da área de jogo, a fase é perdida; a vitória exige o ovo no ninho. Porcos: tocar o chão estando no veículo, ou ficar parado (10 amostras de 0,5 s com menos de 0,5 m), faz o porco abandonar o veículo e pular (5, −5). Peças que saem da área depois de entrar são removidas. `CAMPAIGN.BIN` v7: nomes originais das peças e juntas dos veículos, empuxo por definição.
- **Fase 3 (controladores) — concluída na 0.61.0.** Interface `ev_*` em `ps2/src/egg_vehicles.h` (getX/getY, getBlock e campos `forceRelative`, `forceX/Y`, `targetVelocity`, `togglePower`, `removeJoint`, `removeBlock`, `updateJointDefinition` de `motorSpeed`, `abandonVehicle`, `doWithTimer`, `toggleAutoBalance`, `getClosestEggX`). Traduzidos os 72: fases 1 a 15 (`ps2/src/egg_vehicles.cpp`). Na 0.56.0 entraram os dispositivos (`triggerGadgetFunctionality`): sugador do Egg Defender (desligado até `startSuction`, 1 s para partir, puxa só o ovo e o pássaro — fator 25 para o pássaro — e pega o ovo a menos de 4 m, o que conta como ovo coletado), foguete (`startRocket`: força sobe até o valor em 3 s, queima mais 2 s e desliga) e inflador de balão (`inflate`: depois de 0,5 s o balão puxa com `inflatedForceX/Y`, −150 por padrão; no port a força vai no próprio inflador, sem criar o balão), além de `updateGadgetProperties`. Na 0.57.0: mira (`startAim`: o motor da junta giratória da peça a vira para o ovo, o pássaro em voo ou o estilingue, com `aimingSpeed`, desacelerando perto do alvo), `setVelocity`, `setGrabLogic()` (o porco deixa o sugador pegar o ovo) e a margem de saída da área por peça (5 vezes o tamanho, `isBlockInsideGameArea`), que antes removia ventiladores de veículos ainda entrando. Na 0.58.0: `runDuration` dos foguetes (`updateGadgetProperties`), `stopAim` e os veículos com `disableTeleporting` (8-5, 13-4, 15-2), que nascem perto do ninho em vez de na borda da tela (`profiles/egg_defender.json`, `no_teleport`). Os canhões (`BP_CANNON`, fase 9) miram o pássaro, mas ainda não atiram (`EggDefenderGadget_Cannon.lua`, 905 linhas: balas de canhão criadas na hora). Na 0.59.0: intervalo do fole mudado pelo script (`applyForceInterval`), `getClosestEggDistance` e infladores desligados até `inflate`, como no `init` do dispositivo. Na 0.60.0: `abandonVehicle(nome do porco)`, só aquele porco. Os `pigTargeted` (fases 2, 6, 13) reagem quando o vermelho marca um porco (`RedBird_Special`, `onRedBirdTarget`); o port ainda não tem a habilidade do vermelho, então essas reações (balões que inflam, bouncer que pula) não acontecem. Na 0.61.0: `createJoint` (junta removida volta, ancorada na primeira peça), `getClosestSlingshotX`, `math.random` (gerador próprio do veículo) e porcos que não pegam o ovo (`setGrabLogic` com função inexistente).
- **Habilidade do vermelho — feita na 0.62.0.** `redbird_special.lua` no modo sem toque (o do acelerômetro, que serve ao controle do PS2): enquanto o vermelho voa, o alvo é o porco mais próximo; trocar de alvo avisa o veículo (`pigTargeted`). Com o botão da habilidade, a física desacelera por 0,5 s e o pássaro sai em linha reta para o alvo a 75 m/s, sem gravidade, até bater; o veículo do alvo recebe `redBirdTriggered`. Vale nas fases Egg Defender e Mighty Feathers clássicas (`g_red_bird_special_available`). As 6 reações dos scripts (2-5, 6-3, 13-1, 13-5, 14-1, 14-4) estão em `egg_vehicles.cpp`.
- **Canhões — feitos na 0.63.0.** `EggDefenderGadget_Cannon.lua`: ao lançar o pássaro, o canhão que vai ter a trajetória a menos de 15 m acende o pavio se sobrar 0,4 s; durante o voo a previsão é refeita a cada 5 quadros. A bala sai pela boca com impulso de −1500 no eixo do canhão (recuo de 30 % no canhão), no máximo 10 tiros; balas que saem da área são removidas. São 2 balas escondidas por canhão (`OBJECT_CANNON_BALL`), reaproveitadas, e a bala não colide com o próprio canhão.
- **Acertos da 0.64.0.** (1) As forças das peças (`forceRelative`, `forceX/Y`) são acelerações: força = valor × massa da peça (o balão da 4-2 a −147 × 21 kg segura o veículo de 3125 N; o rotor da 9-5 a −80 × 68,8 kg segura 5312 N). Com isso os voadores voam e pairam. O balão dos infladores puxa com a massa do balão (21,1 kg). (2) O ovo não bloqueia as peças dos veículos (`collider_types.egg`): antes ele virava uma parede soldada ao ninho e os veículos paravam a 9 m. (3) O ovo não quebra (defesa 1000) e, quando o porco que o leva cai, o ovo volta a colidir com o chão (antes atravessava o terreno). (4) A vitória exige o ovo inteiro no ninho. Sem tiros, 11 das 15 fases são perdidas por roubo do ovo entre 18 e 35 s.
- **Fase 4 (testes e acertos) — em andamento.** Pendências do modo: conferir fase a fase contra o jogo original. Os veículos sem controlador usam só o comportamento padrão (motores, ventiladores e abandono por falta de progresso). Não portado ainda: `setTargetY` (a mira do vermelho, `pigTargeted` e `redBirdTriggered`, está no port desde a 0.62.0 e tem teste próprio desde a 0.87.0); `rotorDescend` é só um campo dos scripts. Os voadores da fase 4 (avião, planador) caem cedo no port: falta calibrar a sustentação contra o jogo original.

## Fases do trabalho

1. **Dados** — importar as 15 fases, as ondas e as 72 montagens de veículo para o `CAMPAIGN.BIN` (peças guardadas escondidas, liberadas na hora da onda); peças novas (`ExtraStolenEgg`, motores, rotores, `SpecialBlockStart`).
2. **Motor de veículos** — em `ps2/src`: veículo como grupo de peças e juntas; ventilador/motor/rotor com potência e força relativa; ovo roubável e ninho; abandono e rendição dos porcos; temporizadores; regras de vitória e derrota do modo (`EggDefender.lua`, `EggDefenderObjectives.lua`).
3. **72 controladores** — um por veículo, em `ps2/src/egg_vehicles.cpp`, traduzidos linha a linha dos scripts descompilados, com referência ao arquivo de origem.
4. **Testes e roteiros** — cada fase carrega, cada onda entra, e um roteiro vence.

## Os 72 controladores

| Veículo | Linhas | Callbacks | Operações |
|---|---|---|---|
| EggDefender_1_W1_V1 | 30 | init | getBlock, getPig |
| EggDefender_1_W2_V1 | 61 | init, update, noabandon | getBlock, getPig, removeBlock, removeJoint, updateJointDefinition |
| EggDefender_1_W3_V1 | 75 | init, update, noabandon | getBlock, getPig, removeJoint, togglePower, updateJointDefinition |
| EggDefender_2_W1_V1 | 43 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock |
| EggDefender_2_W2_V1 | 79 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getPig, removeJoint |
| EggDefender_2_W3_V1 | 58 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getPig |
| EggDefender_2_W4_V1 | 79 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getPig, removeJoint |
| EggDefender_2_W5_V1 | 78 | init, eggCollected, blockRemoved, abandon, pigTargeted | abandonVehicle, doWithTimer, getBlock, getPig, removeJoint |
| EggDefender_3_W1_V1 | 64 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, toggleAutoBalance |
| EggDefender_3_W2_V1 | 33 | eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock |
| EggDefender_3_W3_V1 | 33 | eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock |
| EggDefender_3_W4_V1 | 74 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock |
| EggDefender_3_W5_V1 | 115 | init, eggCollected | doWithTimer, getBlock, removeBlock, removeJoint, toggleAutoBalance, updateJointDefinition |
| EggDefender_4_W1_V1 | 99 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, togglePower |
| EggDefender_4_W2_V1 | 110 | init, update, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, rotorDescend |
| EggDefender_4_W3_V1 | 112 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, removeJoint |
| EggDefender_4_W4_V1 | 117 | init, update, eggCollected | getBlock, getClosestEggX, rotorDescend, togglePower |
| EggDefender_4_W5_V1 | 181 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, rotorDescend, toggleAutoBalance, togglePower, updateJointDefinition |
| EggDefender_5_W1_V1 | 83 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_5_W2_V1 | 56 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_5_W3_V1 | 64 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, togglePower, triggerGadgetFunctionality |
| EggDefender_5_W4_V1 | 91 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getPig, triggerGadgetFunctionality |
| EggDefender_5_W5_V1 | 68 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_6_W1_V1 | 137 | init, update, eggCollected, blockRemoved, abandon, release | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, togglePower, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_6_W2_V1 | 60 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_6_W3_V1 | 179 | init, update, eggCollected, blockRemoved, abandon, flee, pigTargeted | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, removeJoint, setAbandoningLogic, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_6_W4_V1 | 171 | init, update, eggCollected, blockRemoved, abandon, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, startAim, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_6_W5_V1 | 118 | init, update, eggCollected, blockRemoved, abandon, flee | abandonVehicle, doWithTimer, getBlock, getPig, triggerGadgetFunctionality |
| EggDefender_7_W1_V1 | 39 | eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock |
| EggDefender_7_W2_V1 | 77 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, removeJoint |
| EggDefender_7_W3_V1 | 106 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, startAim, triggerGadgetFunctionality |
| EggDefender_7_W4_V1 | 155 | init, update, eggCollected, blockRemoved, flee, noabandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, rotorDescend |
| EggDefender_7_W5_V1 | 132 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_8_W1_V1 | 92 | init, eggCollected | doWithTimer, getBlock, triggerGadgetFunctionality, updateGadgetProperties |
| EggDefender_8_W2_V1 | 102 | init, eggCollected | getBlock, removeJoint, triggerGadgetFunctionality, updateGadgetProperties |
| EggDefender_8_W3_V1 | 53 | init, eggCollected | getBlock, triggerGadgetFunctionality, updateGadgetProperties |
| EggDefender_8_W4_V1 | 108 | init, update, eggCollected, release | doWithTimer, getBlock, getClosestEggX, removeJoint, triggerGadgetFunctionality, updateGadgetProperties, updateJointDefinition |
| EggDefender_8_W5_V1 | 48 | init, eggCollected, disableTeleporting | doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_9_W1_V1 | 57 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, startAim |
| EggDefender_9_W2_V1 | 66 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, startAim |
| EggDefender_9_W3_V1 | 43 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, startAim |
| EggDefender_9_W4_V1 | 46 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, startAim |
| EggDefender_9_W5_V1 | 179 | init, update, eggCollected, blockRemoved, flee, surrender, noabandon | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, setAbandoningLogic, startAim, surrender, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_10_W1_V1 | 61 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, removeJoint, startAim |
| EggDefender_10_W2_V1 | 134 | init, update, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_10_W3_V1 | 43 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, startAim |
| EggDefender_10_W4_V1 | 120 | init, update, eggCollected, blockRemoved, flee, release | abandonVehicle, doWithTimer, getBlock, removeJoint, toggleAutoBalance, togglePower, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_10_W5_V1 | 106 | init, update, eggCollected, blockRemoved, flee, surrender | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, startAim, surrender, triggerGadgetFunctionality |
| EggDefender_11_W1_V1 | 321 | init, update, eggCollected, blockRemoved, abandon, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, removeBlock, removeJoint, rotorDescend, toggleAutoBalance, togglePower, triggerGadgetFunctionality |
| EggDefender_11_W2_V1 | 152 | init, update, eggCollected, release | doWithTimer, getBlock, getClosestEggX, getPig, removeJoint, togglePower, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_11_W3_V1 | 188 | init, update, eggCollected, blockRemoved, flee, surrender | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, removeJoint, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_11_W4_V1 | 168 | init, update, eggCollected | doWithTimer, getBlock, getClosestEggX, removeJoint, rotorDescend, triggerGadgetFunctionality |
| EggDefender_11_W5_V1 | 52 | init, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_12_W1_V1 | 92 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, removeBlock, removeJoint, updateJointDefinition |
| EggDefender_12_W2_V1 | 91 | init, update, eggCollected, blockRemoved | abandonVehicle, getBlock, getClosestEggX, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_12_W3_V1 | 177 | init, update, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeBlock, toggleAutoBalance, togglePower |
| EggDefender_12_W4_V1 | 134 | init, update, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, rotorDescend, toggleAutoBalance |
| EggDefender_12_W5_V1 | 166 | init, update, eggCollected, blockRemoved, flee, customAbandoningLogic | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, rotorDescend, setAbandoningLogic, startAim, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_13_W1_V1 | 149 | init, eggCollected, blockRemoved, pigTargeted | doWithTimer, getBlock, getPig, removeBlock, removeJoint, triggerGadgetFunctionality |
| EggDefender_13_W2_V1 | 115 | init, eggCollected, blockRemoved, abandon, release | abandonVehicle, doWithTimer, getBlock, removeBlock, removeJoint, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_13_W3_V1 | 74 | init, update, eggCollected | doWithTimer, getBlock, removeJoint, triggerGadgetFunctionality |
| EggDefender_13_W4_V1 | 101 | init, update, eggCollected, blockRemoved, flee, disableTeleporting | abandonVehicle, doWithTimer, getBlock, removeBlock, removeJoint, triggerGadgetFunctionality |
| EggDefender_13_W5_V1 | 77 | init, eggCollected, blockRemoved, abandon, pigTargeted | abandonVehicle, doWithTimer, getBlock, getPig, updateJointDefinition |
| EggDefender_14_W1_V1 | 165 | init, update, eggCollected, pigTargeted | doWithTimer, getBlock, getClosestEggX, getPig, removeBlock, removeJoint, triggerGadgetFunctionality |
| EggDefender_14_W2_V1 | 66 | init, update, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, triggerGadgetFunctionality |
| EggDefender_14_W3_V1 | 164 | init, update, eggCollected, blockRemoved, flee | abandonVehicle, doWithTimer, getBlock, getClosestEggX, removeJoint, rotorDescend, startAim, toggleAutoBalance, triggerGadgetFunctionality |
| EggDefender_14_W4_V1 | 113 | init, eggCollected, blockRemoved, abandon | abandonVehicle, doWithTimer, getBlock, removeJoint, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_14_W5_V1 | 226 | init, eggCollected, blockRemoved | doWithTimer, getBlock, removeJoint, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_15_W1_V1 | 236 | init, update, eggCollected, blockRemoved, flee | doWithTimer, getBlock, getClosestEggX, getPig, removeJoint, startAim, toggleAutoBalance, togglePower, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_15_W2_V1 | 311 | init, update, eggCollected, blockRemoved, flee, customAbandoningLogic, disableTeleporting | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, removeBlock, removeJoint, startAim, triggerGadgetFunctionality |
| EggDefender_15_W3_V1 | 276 | init, update, eggCollected, blockRemoved, flee, customAbandoningLogic | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getPig, setAbandoningLogic, startAim, surrender, toggleAutoBalance, togglePower, triggerGadgetFunctionality, updateJointDefinition |
| EggDefender_15_W4_V1 | 641 | init, update, eggCollected, blockRemoved, customAbandoningLogic | abandonVehicle, doWithTimer, getBlock, getClosestEggX, getClosestSlingshotX, getPig, removeBlock, removeJoint, rotorDescend, setAbandoningLogic, togglePower, triggerGadgetFunctionality |
