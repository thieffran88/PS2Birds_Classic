# Rio: RocketBird e SUPERPOWERUP_BIRD (pendência 61, restante)

Blu e Jewel foram feitos na 0.49.19. Os outros dois pássaros sem tipo (RocketBird, 6 fases em `bonusNewyear` e `rewardlevels`; SUPERPOWERUP_BIRD, 3 fases em `paradise1` e `rewardlevels`) foram lidos nos scripts, mas **não implementei**: cada um depende de uma parte que o script não define.

## RocketBird (specialty `ROCKET`)
O que está nos dados e nos scripts:
- Corpo: raio 0,8, densidade 7, atrito 0,05, restituição 0,13. Antes do toque, força vertical absoluta de −40 (`flyVerticalForce`).
- É um corpo composto: o pássaro mais um `BOTTLE_ROCKET_BIRD` (polígono, densidade 1, escala 0,7) unidos por uma junta (tipo 2, deslocamento 0,52 / 1,45; quebra com força 400).
- No toque (`gamelogic.lua`): as velocidades angulares do pássaro e do foguete vão a 0, o pássaro fica com 0,4 da velocidade do foguete e `shootRocket` arma o foguete: sem a massa do pássaro (`removeBirdMass`), resistência 200, defesa 100, cronômetro de 1,6 s (`deathTimerFull`), som `fireworks_fly`. Ao fim do tempo ou ao bater, o foguete explode: raio 8, força 10500, raio de dano 3, dano 400.
- O que **falta**: a força do empuxo. `thrust = 2880` vem do campo do bloco, e o script não o aplica: quem aplica é o código nativo (`rocket = "bird_rocket"`). A unidade não pode ser deduzida: se for aceleração, são 2880 m/s²; se for força numa peça de massa ~0,12, são mais de 20.000 m/s². O Egg Defender do Classic usa `thrust` como aceleração (força = massa × thrust), com valores bem menores. Sem a leitura do empuxo nativo (`libAngryBirdsRio.so`) o voo do foguete seria um número chutado.

## SUPERPOWERUP_BIRD (specialty `SUPERPOWERUP`, o Blu macaw do power-up de bando)
- Corpo: raio 0,85, densidade 1,5, resistência 0,01, defesa 0.
- No toque chama `makeSuperpowerBirds`: cria um **bando de pássaros** (definições `SUPERPOWERUP_BIRD_FLOCK_1` a `_10`, em quantidades por espécie), cada um com um comportamento de míssil teleguiado (`homingMissile`: sinal-guia, empuxo aleatório, distância de armar, `approaching`) a partir de pontos fora da tela.
- O que **falta**: as quantidades por espécie e as constantes do míssil (empuxo, distância de armar, desaceleração) ficam em constantes que o descompilador perdeu, e o movimento é aplicado por uma rotina de atualização do bando que só o código nativo cobre por inteiro. Também exigiria desenhar até dezenas de pássaros por vez (hoje o runtime desenha no máximo 3 projéteis).

## Decisão
Não inventei os números. Para fechar a pendência 61 há duas rotas:
1. **Aproximar:** Rocket com empuxo calibrado para um alcance plausível e o bando do macaw com um número fixo de pássaros em linha reta. Funciona, mas não é fiel, e nenhuma fase do port usa esses pássaros hoje (os pacotes `bonusNewyear`, `paradise1` e `rewardlevels` não estão no port).
2. **Ler o `.so` do Rio** (x86, como foi feito com o do Classic) para extrair o empuxo e o míssil. Trabalho longo.

Enquanto isso, as 9 fases continuam fora do `CAMPAIGN.BIN` do Rio, como já eram.
