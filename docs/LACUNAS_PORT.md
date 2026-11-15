# Lacunas do port (levantamento atualizado na 0.50.0)

Base: os 762 níveis dos episódios do Classic foram decompilados do APK e comparados com o que o port já entende (definições de objetos, pássaros, juntas). Nenhum nível falhou na leitura.

## Cobertura por episódio (níveis totalmente suportados / total)

- Poached Eggs: 63/63 (o port inclui as 3 páginas, 63 fases)
- Mighty Hoax: 42/42 depois da 0.21.0 (o port inclui as páginas 4 e 5, 42 fases; as duas fases restantes eram as que usavam LightBlock8, maçã e bandeira)
- Danger Above: completo no port desde a 0.24.0 (páginas 6, 7 e 8, 45 fases)
- Mighty Feathers: 15/46; Ham 'Em High: 8/48; Golden Eggs: 4/31
- Surf and Turf 2/45, Bad Piggies 3/45, Flock Favorites 3/30
- Zero: Tutorial, Big Setup, Mine and Dine, Short Fuse, Bird Island, Piggy Farm, Dino, Birdday 2–7 (levantamento inicial; hoje todos, exceto o Tutorial, estão no port)

## O que falta na decompilação/importação

- 547 definições sem adaptador. As mais usadas:
  - pássaros: RedBigBird (Terence), BoomerangBird (verde), PinkBird (Stella), GlobeBird (orbe);
  - blocos: StaticFragileBlock06, LightBlock8, WhiteBlock11, STATIC_WALL_5;
  - extras: cordas (ExtraRope*, BP_ROPE_THICK), barris, capacetes, trampolim, ferramentas e tesouros;
  - decorações: Farm, Dino, Birthday.
- Juntas: o port suporta todos os tipos usados nas fases: distância (1), solda (2), dobradiça com motor e limites (3), prismática (4) e "destruir anexo" (5).
- Câmera: os roteiros de pan (castleCameraData e birdCameraData) não são aplicados.

## Achados da 0.21.0

- `blocks.lua` declara, para cada pássaro, duas tabelas por material: `damageMultiplier` (já usada pelo port: azul ×2,9 no gelo, amarelo ×2,4 na madeira, preto ×4 na pedra) e `velocityMultiplier` (azul ×1,9 no gelo e ×2 em osso; Terence ×1,8). Esta segunda tabela ainda não é aplicada: ela controla quanto da velocidade o pássaro conserva ao atravessar a peça.
- 5-16 e 3-20 são as únicas fases sem roteiro, e as duas só têm pássaros pretos/explosivos. A cena da 5-16 assenta sem perder nenhum porco (deslocamento máximo de 2 unidades), então a importação está correta; o que falta é a fórmula nativa da explosão do pássaro preto (raio, queda com a distância e impulso), que não está nos scripts Lua.
- A página 5 precisou de um terceiro banco de texturas. Para caber, as peças numeradas desse banco usam dois quadros de dano (intacto e muito danificado) em vez de quatro.

## Achados da 0.22.0

- O pássaro verde (BoomerangBird) não depende de código nativo: o algoritmo inteiro está em `gamelogic.lua` (ativação e atualização por quadro) e os parâmetros em `blocks.lua`. Ele foi portado com esses valores (trechos em `ps2/data/boomerang_logic_excerpt.lua` e `ps2/data/page6_definitions_excerpt.lua`).
- O grupo `staticFragile` (material `clouds`, resistência 0,1) descreve peças estáticas que somem no primeiro impacto: balões. O trampolim usa o material `immovableRubber` com restituição própria de 1,1.
- Fases da página 6 com até 169 objetos: o limite do port subiu de 160 para 192.
- O extrator de sprites só verificava o TNT do último objeto de cada fase (erro de indentação), corrigido.

## Achados da 0.23.0

- O mundo 7 só precisava de três peças estáticas: StaticFragileBlock06 (material `immovableFragile`: resistência 2, defesa 1), StaticBlock_06 e StaticBalloon01. O bloco frágil usa a mesma regra de "estoura no primeiro impacto" dos balões; a fórmula exata de dano de peças estáticas é nativa.
- Os blocos de terreno são desenhados no original com texturas de tema (`THEME_01_THEME_GROUND_1` etc.) recortadas pela máscara. O port desenha só a máscara tingida; a máscara do GROUND_BLOCK_06 é quase transparente, então esses blocos frágeis de 1×1 quase não aparecem. Falta portar o preenchimento por textura.
- A campanha passou de 128 fases: o save ganhou o formato v5 (256 posições).
- Fases com até 200 objetos: o limite subiu para 224. As ferramentas de medição de acomodação guardavam só 200 objetos e acusavam movimento falso na 7-2; foram corrigidas.
- Com a medição corrigida, 12 fases antigas mostram alguma peça se deslocando mais de 3 unidades ao carregar (1-14, 2-9, 3-5, 3-13, 3-19, 4-4, 4-18, 5-6, 5-8, 5-9, 5-20, 5-21), sem porcos mortos. Vale investigar se são objetos que o original mantém "dormindo" até o primeiro impacto.

## Achados da 0.24.0

- **Chão curto (corrigido):** o chão do port era uma caixa de x = −100 a 140. O objeto `Ground` dos dados cobre a fase inteira, e várias fases têm peças além de 140 (4-5 até 203; 7-13 inteira em 700–825, inclusive o estilingue). Essas peças caíam pelo chão, e porcos "morriam" sozinhos: a 7-13 era vencida porque a fase inteira despencava. Com o chão de ±1000, 8 fases antigas precisaram de roteiros novos (3-19, 4-1, 4-4, 4-11, 5-8, 6-2, 7-5, 7-15).
- **Peças "dormindo" (hipótese forte):** o motor nativo mantém o estado `sleeping` de cada objeto (`updateObjectProperties` em `gamelogic.lua`). O editor acorda as peças só quando as move (`setSleeping(..., false)` em `editor.lua`), e o modo Demolisher as põe para dormir explicitamente. Isso indica que, no original, as construções começam em repouso e só reagem quando atingidas. No port, 15 fases têm alguma peça que se desloca mais de 3 unidades ao carregar (1-14, 2-9, 3-5, 3-13, 3-19, 4-4, 4-18, 5-6, 5-8, 5-9, 5-20, 5-21, 8-4, 8-9, 8-11). Exemplo na 4-4: um bloco apoiado no encontro de duas tábuas diagonais que, com as medidas exatas do APK, não se tocam; ele só fica de pé se começar dormindo. Implementar isso (`b2BodyDef.awake=false`) muda a física de todas as fases e exige refazer todos os roteiros: é o próximo passo recomendado.
- **Memória:** cada banco de texturas embutido custa 2,8 MB de RAM; o ELF chegou a 19,7 MB com 5 bancos. Para os próximos episódios, os bancos devem ser lidos do disco sob demanda em vez de ficarem dentro do ELF.
- **Bola superelástica:** o `blocks.lua` dá restituição 5,5 à ExtraTrampoline (o Box2D original também usa o maior valor do par), mantido como no dado.

## Achados da 0.25.0

- **Peças "dormindo" — testado.** Com a opção `SCENE_STARTS_ASLEEP` (peças criadas em repouso), nenhuma das 150 fases tem peça se movendo mais de 1 unidade ao carregar (antes, 15 fases passavam de 3 unidades). Mas 109 dos 148 roteiros deixam de vencer e a 1-1 cai de 119 para 68 acertos na grade de teste, porque as construções ficam mais firmes. Há também um contra-indício: no jogo original, porcos às vezes morrem antes do primeiro tiro porque a construção se acomoda, o que só acontece com as peças acordadas. A opção ficou no código, desligada, até haver evidência direta (por exemplo, o código nativo que cria os corpos).
- **Memória — resolvido.** Os bancos de textura 1+ saem do ELF e vão para o `SPRITES.BIN`; cada novo episódio passa a custar só disco, não RAM. Testado no PCSX2 pela ISO (disco) e pelo ELF com o arquivo ao lado (HostFs).
- **Big Setup (mundos 9–11, 45 fases):** nenhuma fase importa ainda. Todas usam o Terence (RedBigBird: raio 1,8, densidade 10, restituição 0,45, sem habilidade especial; `RedBigBirdDamageFactors` dá dano ×0,3 e velocidade ×1,8) e capacetes (ExtraHelmetSmall/Big/Boss) definidos como polígonos livres, com vértices relativos ao tamanho do sprite. O port só tem triângulos fixos: falta um importador de polígonos genéricos. Uma fase também usa corda (ExtraRopeThick01).

## Achados da 0.26.0

- **Big Setup — feito.** Terence (RedBigBird) e capacetes (ExtraHelmetSmall/Big/Boss) portados. Os capacetes são hexágonos com vértices em frações do sprite; o `ObjectCreation.lua` converte cada vértice em `(x·largura − largura/2, y·altura − altura/2)`, e o port usa a mesma regra para qualquer polígono de até 8 vértices.
- **Escala por peça (corrigido):** o `ObjectCreation.lua` multiplica o tamanho pela `scaleX`/`scaleY` da instância. O importador ignorava esses campos; só a 6-14 os usa (um bloco de terreno reduzido a 25% × 88%).
- **Capacetes escorregando:** o capacete grande é mais largo que o porco e fica apoiado no topo redondo; no port ele escorrega devagar ao carregar. Faz parte da mesma questão das peças em repouso.
- **Corda grossa:** segmentos-caixa (BLOCK_ROPE_THICK_1) ligados por juntas de distância; na 10-15 ela segura um TNT pendurado.
- **Pendências do Terence:** a tabela `velocityMultiplier` (×1,8 na madeira, no gelo e na pedra) ainda não é usada; só o multiplicador de dano (×0,3).

## Achados da 0.27.0

- **Ham 'Em High — feito.** Só precisou de peças: cinco chapéus de faroeste (polígonos de 6 vértices, mesma regra dos capacetes), corda curta e a tigela do ovo dourado. Nenhum pássaro novo. As 45 fases das três páginas entraram; a quarta página do mundo no `episodes.lua` (4 fases bônus com blocos de tema 16 e a LevelGE_22, ausente do APK) ficou de fora.
- **13-7 sem roteiro:** 17 porcos para 3 pássaros (Terence e dois pretos). A melhor sequência encontrada chega perto, mas, como na 3-20 e na 5-16, a vitória depende da explosão do pássaro preto, cuja fórmula é nativa.
- **Próximo episódio, Mine and Dine:** a levantar na próxima rodada.

## Achados da 0.28.0

- **Mine and Dine — feito.** 20 peças novas, todas lidas do `blocks.lua` pela nova `tools/add_definitions.py` (tipo, sprite, vértices, material, atrito e restituição). Cinco materiais novos (ametista, âmbar, rubi, estalactite e ponta de estalactite) lidos da tabela de materiais.
- **Folhas RGBA8888:** os chapéus de mineiro estão numa folha (INGAME_TRANSPARENCY_1_1) em RGBA8888, sem a borda de 1 pixel das folhas RGBA4444. O extrator passou a ler os dois formatos.
- **Densidade 0 = peça fixa:** regra do `ObjectCreation.lua`, agora aplicada a qualquer peça (estalactites e outras ficam presas no teto). Peças fixas não recebem dano no port (o dano usa a massa do corpo); a quebra de estalactites fixas depende de código nativo.
- **Junta tipo 5 ("Destroy-attached"):** ver correção na 0.29.0.
- **Limites:** fases com até ~320 peças (limite subiu para 352) e campanha acima de 256 fases (save v6 com 512 posições).
- **Memória de vídeo:** os bancos de Mine and Dine ficaram no limite; neles, o porco machucado usa o quadro normal e as tábuas 10×1 usam dois quadros de dano.
- **8 fases de Mine and Dine sem roteiro ainda** (15-2, 15-7, 15-14, 16-2, 16-5, 16-6, 17-4, 17-14): são as maiores (até 300 peças) e a busca em feixe leva horas; ficou rodando para a próxima versão.

## Achados da 0.29.0

- **GlobeBird — feito, a partir do Lua.** O `puffer_bird.lua` traz o algoritmo inteiro: no toque (ou 1,5 s depois do primeiro contato), troca atrito/restituição/densidade pelos valores "inflados", aumenta o raio para 7,5 e dispara `makeExplosion` (raio 8, força 45000, sem dano); depois de 1,5 s encolhe até sumir em 1,5 s, mudando a velocidade em ±50 a cada 0,05 s. O sorteio (`math.random`) virou um gerador determinístico no port.
- **Balões:** o material `balloon` tem `forceX = −1` e `forceY = −30`, registradas por `setObjectAsForceAdder` (nativo). O port aplica força × massa a cada passo: o balão sobe (30 contra a gravidade de 20) e sustenta as cordas. A escala exata da força nativa não é conhecida.
- **Junta tipo 5 corrigida:** na 0.28.0 o port removia a peça anexada depois de `destroyTimer` desde o início da fase; em Birdday 2 a junta aparece sem tempo, e isso destruía peças ao carregar. A leitura certa do nome e dos dados: quando a ponta 1 é destruída, a ponta 2 é removida `destroyTimer` segundos depois. Duas fases de Mine and Dine mudaram com isso (16-7 ganhou roteiro novo; 15-12 está na busca).
- **Birdday 2 entrou primeiro** porque precisa só do GlobeBird; Birdday 3 e 4 também usam a Stella (PinkBird); Birdday 5–7 têm muitas peças próprias.

## Achados da 0.30.0

- **Stella — feita, a partir do Lua.** O `bubbles.lua` traz `triggerPinkBirdSpecialty` e `updateBubble`; os valores vêm do `default_conf.lua` (antigravidade 1,8, 2 s, 10 u/s) e o atraso de 1,5 s depois do contato do `CollisionCallbacks.lua`.
- **Tamanho declarado:** o `blocks.lua` dá largura/altura ou raio para várias peças; o port usava o sprite. Corrigido; as fases 240–299 foram refeitas.
- **Sem roteiro:** 16-5, 16-6 e Birdday 4-4, além de 3-20, 5-16 e 13-7.

## Achados da 0.31.0

- **`deathSpawn` — feito, a partir do `gamelogic.lua`.** Ao remover uma peça com `deathSpawn`, o jogo cria `amount` peças do tipo sorteado em `types`, na posição dela, com rotação aleatória e os valores da própria definição (o creme: círculo de raio 1,3, densidade 0,1, resistência 10). O port guarda os 10 cremes de cada bolo grande escondidos e os solta quando o bolo é destruído; a rotação aleatória virou uma sequência fixa.
- **Separação jogo/dados:** o ELF não embute mais fases, texturas, sons nem o ícone; tudo vem de `CAMPAIGN.BIN`, `SPRITES.BIN` e `SOUNDS.BIN`, gerados do APK do jogador.
- **Fases sem decompilador:** os arquivos de fase do APK são tabelas puras; um leitor de bytecode Lua 5.1 que só aceita operações de dados dá resultado idêntico ao do unluac nas 330 fases.

## Achados da 0.32.0

- **Junta prismática (tipo 4) — feita.** As fases guardam eixo (`worldAxisX/Y`), limites (`lowerLimit/upperLimit`), motor (`motorSpeed`, `maxTorque`) e `backAndForth`. O `gamelogic.lua` chama a função nativa `checkJointLimits` a cada passo para as juntas com `backAndForth`; o port inverte o motor ao chegar a cada limite.
- **Motor e limites nas dobradiças (tipo 3) — lacuna antiga corrigida.** Três dobradiças de fases já suportadas tinham motor, e o port as tratava como soltas.
- **Peças sem colisão:** `collision = false` no `blocks.lua` (cenário desenhado que nada toca) virou sensor do Box2D.
- **Força própria da peça:** o balão `BP_SINGLE_BALLOON` declara `forceY = -80` na própria definição (não no material) e `targetVelocity = 20`; o port deixa de empurrar quando o balão já sobe a essa velocidade.
- **Birdday 6 (15 fases) entrou.** O Birdday 5 aparece antes no APK, mas precisa de 90 peças novas: 31 de cenário de papelão sem colisão, 7 fixas (régua, lápis, borracha, transferidor, compasso, rinoceronte), 31 extras (chapéus, máscaras, óculos, objetos de escritório), 9 frascos de vidro, um motor de Bad Piggies e a vela do bolo. Todos os pássaros já existem no port.

## Achados da 0.33.0

- **Birdday 5 entrou (30 fases).** As 90 peças saíram da `add_definitions.py`; 35 sprites estão nas folhas de tema (`themes_android`), não nas folhas base.
- **Memória de vídeo é o limite dos episódios com tema próprio:** cada banco tem 2,75 MB de textura; o Birdday 5 precisou de meia resolução nas peças e de bancos de 5 fases. Os próximos episódios com muitos objetos (Dino, Piggy Farm) vão precisar do mesmo.

## Achados da 0.37.0

- **Pássaro de choque (Short Fuse) — feito**, a partir do `BlackBirdAbility.lua` e do `blocks.lua`.
- **Mighty Feathers = modo Egg Defender:** as 30 fases do episódio vêm com scripts de jogabilidade próprios (`EggDefender_*.scripts.lua`); é um modo novo, não só peças.
- **Efeitos especiais do Short Fuse ainda não portados:** frascos (`VialEffects.lua`: azul, laranja e rosa mudam os porcos), foguete (`RocketCanister.lua`), ímã e balão explosivo.
- **Instabilidade em 2-13 e 3-3:** soldas entre vidro leve e pedra pesada; precisa de mais iterações do solver ou outra estratégia de solda, o que mexe em todas as fases.

## Achados da 0.40.0

- **Dino entrou (46 fases).** 107 peças novas (ossos, mandíbulas, máscaras tribais, frutas, palmeiras, plantas, ovos de dinossauro). O cenário do Dino ocupa tanta memória de vídeo que cada página do jogo virou quatro ou cinco (2 a 4 fases por banco, bancos 41 a 53).
- **Gaiolas:** as peças `*_Cage` declaram `spawnNewBirdOnDestruction`; quebrar a gaiola põe o pássaro no fim da fila (`spawnBird` em `gamelogic.lua`). O `blocks.lua` usa duas formas de escrever esse campo, e as duas são lidas.
- **Folha `INGAME_DINOS_DECOS_1`:** os quadros não declaram extrusão, mas a borda existe; a margem vem do cabeçalho do quadro.

- **Levantamento do que ainda não está no port (pastas de fases do APK):**
  - Ham 'Em High: 3 das 48 fases (usam `StaticBlockTheme16_*`, 4 peças).
  - Flock Favorites: 30 fases, 24 peças (veículos de Bad Piggies: rodas, molduras, braço telescópico).
  - Golden Eggs: 31 fases, 15 peças (`BP_BOUNCER`, `BP_SUCKER`, rodas).
  - Tutorial: 15 fases, 5 peças.
  - Mighty Feathers: 46 arquivos, 15 sem cenário (modo Egg Defender), 48 peças.
  - Fora dos episódios: Mighty League (273 fases, 47 peças), siege (9), powerupSchool (5), eagleDemo (3), ugc (15), packTalkweb (15).

## Achados da 0.41.0

- **Peças explosivas:** todo `specialty = "BOMB"` do `blocks.lua` traz raio, força, raio de dano e dano próprios. Antes só o TNT comum explodia; agora explodem também antimatéria, balão explosivo, foguete, fogareiro e os TNT de Bad Piggies, cada um com seus valores. Sete roteiros antigos deixaram de vencer e foram refeitos (o Short Fuse 2-9 ficou sem roteiro).
- **Fases de formato antigo (Ham 'Em High extra):** sem densidade/resistência/defesa por objeto e sem `birdZeroPos`; o jogo completa com a definição, depois o material (`ObjectCreation.lua`), e põe a fila no estilingue (`Slingshot.lua`). `StaticBlockTheme<tema>_NN` sem definição própria é `StaticBlock_NN` (`dynamicassets.lua`).
- **Flock Favorites:** `FlockFavoritesMode.lua` roda as fases de tema 26 no modo Egg Defender (`RMFClassic`) e as de tema 27 no modo do pássaro de choque. As fases 12, 14, 25 e 28 (tema 26) esperam o modo Egg Defender; a 1-9 desmonta ao assentar (cordas soldadas).
- **Golden Eggs:** LevelGE_31 tem gravidade invertida (−20) e LevelGE_UGC1 gravidade zero; o motor do port só aceita gravidade para baixo. Três fases não têm limites de estrela (o original dá o ovo de ouro).

## Achados da 0.42.0

- **Tutorial entrou (15 fases).** Cinco são tutoriais da loja de power-ups (`shopTutorialPowerup`: slingscope, powerpotion, kingsling, birdquake, shockwave) e são jogadas como fases comuns.
- **Power-ups estão inteiros em Lua:** `powerups_Powerup_*.lua` (KingSling, SlingScope, BirdQuake, Shockwave, PowerPotion, SuperSeed, BoomBox, ChronoBird, DoubleScore, ExtraBird, HomingHat, RedsMightyFeathers), com `PowerupManager.lua` e `PowerupDefines.lua`. Portáveis sem código nativo; é o próximo recurso de jogo depois do Egg Defender.

## Achados da 0.43.0

- **Gravidade da fase — feito.** O motor já criava o mundo com a gravidade de cada fase; só o importador recusava valores não positivos. LevelGE_31 (gravidade −20: tudo cai para cima) e LevelGE_UGC1 (gravidade zero) entraram. A resistência das soldas usa o módulo da gravidade.
- Um roteiro achado pela busca (Dino 3-8) não se repete no teste da campanha; a fase continua sem roteiro.

## Achados da 0.44.0

- **Frascos do Short Fuse (`VialEffects.lua`) — laranja e rosa feitos.** Ao quebrar, o frasco age sobre os porcos dentro de `vialEffect.radius` depois de 1,3 s: o laranja (raio 6) os incha como o GlobeBird e eles estouram (`puffTime` 1,5 + `deathTime` 1,5); o rosa (raio 7) os põe numa bolha que flutua e ignora dano (`transformIntoBubble`). 44 fases têm frascos.
- **Frasco azul — feito na 0.45.0.** Ele clona os porcos dentro do raio 7 (`createObject`, escala 0,8). O importador guarda um clone escondido por porco próximo; ao quebrar o frasco, o clone aparece ao lado do porco se ele ainda estiver vivo e no raio, e passa a contar como alvo. O giro dos clones em volta do frasco (`blueVialSpin*`) não foi portado.

## Achados da 0.46.0

- **Power-ups — King Sling e Bird Quake feitos**, nas fases de tutorial da loja. `Powerup_KingSling.lua` soma 0,3 à força do estilingue (1 → 1,3). `Powerup_BirdQuake.lua` dura 2 s e, a cada passo, empurra toda peça móvel com (dt·cos(t·π)·massa·7, −dt·massa·7,5).
- **Power Potion — feita na 0.47.0** (`Powerup_SuperSeed.lua`): o pássaro lançado tem o raio ×1,5 (Terence ×1,4) e a densidade multiplicada por tipo (vermelho 2, azul 3, amarelo 1,5, preto 2, branco 2,5, verde 1,5, Terence 1,01, GlobeBird 2, choque 1,45). O `velocityMultiplierModifier` da poção não é aplicado (o port não tem a passagem por material).
- **Shockwave — feito na 0.48.0:** fora a animação de introdução, o power-up troca o pássaro do estilingue por um pássaro de choque (`replaceBird("BlackBird_Shockwave")`, marcado `bonusShockwaveBird`).
- **Falta:** os demais power-ups da loja (Homing Hat, Boom Box, Chrono Bird, Double Score, Extra Bird, Red's Mighty Feathers). O Sling Scope não muda nada no port: a prévia da trajetória já é longa.
- **Fórmula de dano por colisão em Lua:** `gamefeatures/physics/AdventureModeCollisions.lua` tem o dano de pássaro × bloco (velocidade relativa ÷ 10 × `damageMultiplier` − defesa; ao quebrar, o pássaro segue com a velocidade × min(1, sobra × `velocityMultiplier`)) e de bloco × bloco. Esse arquivo é do modo Aventura, não do Classic; serve de referência para a fórmula nativa, mas não foi aplicado.

## Achados da 0.49.0

- **Mighty Feathers tem dois tipos de fase.** As 15 `EggDefender_classic_N` (classe `RMFClassic` no `episodes.lua`) são fases comuns de estilingue com porcos em veículos: entraram no port. As 15 `EggDefender_N` (com `.scripts.lua`) são o modo Egg Defender de verdade (o jogador defende o ovo; `EggDefender.lua`, 1899 linhas, mais veículo, porco e HUD próprios): continuam fora.
- **Engenhocas dos veículos — faltam.** `RMFClassic_Cannon.lua`, `RMFClassic_Sucker.lua` e `RMFClassic_BubbleGun.lua` (com `EggDefenderGadget_*.lua`) fazem o canhão atirar, o sugador puxar e a arma soltar bolhas. No port essas peças existem, mas ficam paradas; vale para as 15 fases do Mighty Feathers e para as 4 do Flock Favorites de tema 26 (12, 14, 25 e 28), que também entraram.

## Achados da 0.50.0

- **Arma de bolhas (`EggDefenderGadget_BubbleGun.lua`):** a cada 2,5 s cria uma bolha (`BP_BUBBLE_WITH_COLLISION_DAMAGE` nas fases `RMFClassic`) na boca da arma e a lança com força sorteada entre 35 e 50 na direção da peça; para quando a arma se solta do veículo. Exige criar objetos durante a fase (o port só libera peças preparadas pelo importador), por isso ainda não foi portada. Há 10 armas, 15 sugadores e 6 ventiladores nas 19 fases `RMFClassic`.

## O que falta no jogo (código nativo não recuperado)

- Fórmulas de dano, de queda de explosão e de "sono" dos corpos: hoje são aproximações. São a causa provável de a 3-20 continuar sem vitória (cinco pássaros explosivos).
- Pontuação: as estrelas usam os limites originais, mas a pontuação do port é aproximada. A contagem de estrelas é, portanto, indicativa.
- Música de fundo, preenchimento de terreno (texturas de solo das cenas) e efeitos de partículas.

## Prioridade sugerida

1. Engenhocas dos veículos (canhão, sugador, arma de bolhas: `RMFClassic_*.lua`); modo Egg Defender propriamente dito (15 fases `EggDefender_N`); power-ups da loja que não aparecem em fases (Homing Hat, Boom Box, Chrono Bird, Double Score, Extra Bird, Red's Mighty Feathers); giro dos clones do frasco azul; roteiros das fases ainda sem vitória (lista em `docs/VALIDACAO_<versão>.md`).
1b. Velocidade preservada ao atravessar materiais (`velocityMultiplier`), que o Terence usa.
1c. Peças em repouso: só com evidência direta do código nativo.
2. `velocityMultiplier` por material (dado já recuperado).
3. Cordas.
4. Pan de câmera e música.
