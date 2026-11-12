# Lacunas do port (levantamento atualizado na 0.23.0)

Base: os 762 níveis dos episódios do Classic foram decompilados do APK e comparados com o que o port já entende (definições de objetos, pássaros, juntas). Nenhum nível falhou na leitura.

## Cobertura por episódio (níveis totalmente suportados / total)

- Poached Eggs: 63/63 (o port inclui as 3 páginas, 63 fases)
- Mighty Hoax: 42/42 depois da 0.21.0 (o port inclui as páginas 4 e 5, 42 fases; as duas fases restantes eram as que usavam LightBlock8, maçã e bandeira)
- Danger Above: o port inclui as páginas 6 e 7 (30 fases) desde a 0.23.0; falta o mundo 8 (15 fases)
- Mighty Feathers: 15/46; Ham 'Em High: 8/48; Golden Eggs: 4/31
- Surf and Turf 2/45, Bad Piggies 3/45, Flock Favorites 3/30
- Zero: Tutorial, Big Setup, Mine and Dine, Short Fuse, Bird Island, Piggy Farm, Dino, Birdday 2–7

## O que falta na decompilação/importação

- 547 definições sem adaptador. As mais usadas:
  - pássaros: RedBigBird (Terence), BoomerangBird (verde), PinkBird (Stella), GlobeBird (orbe);
  - blocos: StaticFragileBlock06, LightBlock8, WhiteBlock11, STATIC_WALL_5;
  - extras: cordas (ExtraRope*, BP_ROPE_THICK), barris, capacetes, trampolim, ferramentas e tesouros;
  - decorações: Farm, Dino, Birthday.
- Juntas: 544 níveis usam juntas. A 0.20.0 suporta distância (1), solda (2) e dobradiça (3). Ainda faltam prismática (4) e "destruir anexo" (5).
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

## O que falta no jogo (código nativo não recuperado)

- Fórmulas de dano, de queda de explosão e de "sono" dos corpos: hoje são aproximações. São a causa provável de a 3-20 continuar sem vitória (cinco pássaros explosivos).
- Pontuação: as estrelas usam os limites originais, mas a pontuação do port é aproximada. A contagem de estrelas é, portanto, indicativa.
- Música de fundo, preenchimento de terreno (texturas de solo das cenas) e efeitos de partículas.

## Prioridade sugerida

1. Terence (RedBigBird) e Stella (PinkBird), mais StaticFragileBlock06 e as demais peças de Danger Above 7–8 e Ham 'Em High. O pássaro verde e os balões já foram feitos na 0.22.0.
2. `velocityMultiplier` por material (dado já recuperado).
3. Juntas prismáticas e cordas.
4. Pan de câmera e música.
