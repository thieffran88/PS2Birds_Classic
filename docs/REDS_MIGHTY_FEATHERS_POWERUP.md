# Red's Mighty Feathers (power-up da loja): investigação da pendência 32d

Conclusão: **é viável**, ao contrário da Boom Box e do Chrono Bird. A definição do pássaro, a regra e quase toda a arte estão no APK, e o port já tem a regra pronta para os modos Egg Defender e Mighty Feathers. Falta ligar a opção da loja. Nenhum código foi alterado.

## O que existe no APK
- **Definição `RedsMightyFeathersBird`** (`scripts/blocks.lua`): igual ao pássaro vermelho (raio 0,85, atrito 0,3, restituição 0,43), com densidade 8 (o vermelho comum é 6), `isMightyFeather` e as funções `triggerRMFBirdSpecialty`, `flyingRMFBirdCollided`, `rmfBirdRemoved`, `rmfBirdShot`, `rmfBirdAdded`.
- **Regra:** `subsystems/redsmightyfeathers/RedsMightyFeathersAbility.lua` (a mesma habilidade do vermelho dos modos Egg Defender e Mighty Feathers): enquanto voa o pássaro escolhe o alvo mais próximo (porco ou peça com `redBirdTarget`, à direita do limite esquerdo da tela); no toque o mundo desacelera por 0,5 s e ele voa reto até o alvo a 75 m/s. **As constantes 0,5 s e 75 m/s são as que o port já usa** (`RED_SPECIAL_DELAY`, `RED_SPECIAL_SPEED`, `red_mode` em `game.cpp`).
- **Projétil `RedBirdFeather`** (raio 1, densidade 2, atrito 0,8, fatores `RedBirdFeatherDamageFactors`: madeira 2, pedra 1, porco 8) e as penas `FEATHER_SHOOT_LEFT/RIGHT`.
- **Arte:** botão `BUTTON_REDSUITS` (e `_DISABLED`) na HUD, explosões `REDSUITS_EXPLOSION_01..09`, estilingue vermelho `SLING_SHOT_RED_BACK/FRONT`, partículas `redsMightyFeathersAppearBuff`.

## O que o power-up faz (`powerups/Powerup_RedsMightyFeathers.lua`)
Ao ser ativado (botão na HUD, durante a fase), troca o **próximo pássaro** da fila, qualquer que seja o tipo, por um `RedsMightyFeathersBird` (`replaceBird`), tira o chapéu dele, põe o estilingue vermelho (`setSlingshot`, `SLINGSHOT_POWERUP_REDSLING`), toca `super_slingshot` e faz fumaça. Vale para **um** pássaro por ativação.

## O que falta ligar no port
1. Uma opção `SHOP_RMF` no painel da loja (como `SHOP_HOMING`) e o botão na HUD (como o visor dos power-ups).
2. `red_mode` ligado também para esse pássaro fora dos modos Egg Defender / Mighty Feathers: hoje `red_mode` só liga quando o nome da fase começa por `EggDefender_`, e a mira do alvo (`red_target`) usa só `kind==4` (porcos).
3. A regra de troca (um pássaro por ativação, qualquer tipo, densidade 8) e o estilingue vermelho.
4. `SLINGSHOT_POWERUP_REDSLING` não é nome de quadro em nenhuma folha; o que existe é `SLING_SHOT_RED_BACK/FRONT`, que é o que usaria.

## Esforço e impacto
Pequeno a médio (a regra existe). Muda o Classic, então sairia como **1.0.1**: o Classic está declarado 1.0.0 e você pediu para não mexer nele por um tempo. Decisões pendentes: aplicar a todo pássaro vermelho com a opção ligada (como o Chapéu teleguiado) ou fiel ao original (um pássaro por ativação, qualquer tipo).
