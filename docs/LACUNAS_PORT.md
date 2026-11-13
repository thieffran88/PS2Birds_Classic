# Lacunas do port (levantamento atualizado na 0.24.0)

Base: os 762 níveis dos episódios do Classic foram decompilados do APK e comparados com o que o port já entende (definições de objetos, pássaros, juntas). Nenhum nível falhou na leitura.

## Cobertura por episódio (níveis totalmente suportados / total)

- Poached Eggs: 63/63 (o port inclui as 3 páginas, 63 fases)
- Mighty Hoax: 42/42 depois da 0.21.0 (o port inclui as páginas 4 e 5, 42 fases; as duas fases restantes eram as que usavam LightBlock8, maçã e bandeira)
- Danger Above: completo no port desde a 0.24.0 (páginas 6, 7 e 8, 45 fases)
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

## Achados da 0.24.0

- **Chão curto (corrigido):** o chão do port era uma caixa de x = −100 a 140. O objeto `Ground` dos dados cobre a fase inteira, e várias fases têm peças além de 140 (4-5 até 203; 7-13 inteira em 700–825, inclusive o estilingue). Essas peças caíam pelo chão, e porcos "morriam" sozinhos: a 7-13 era vencida porque a fase inteira despencava. Com o chão de ±1000, 8 fases antigas precisaram de roteiros novos (3-19, 4-1, 4-4, 4-11, 5-8, 6-2, 7-5, 7-15).
- **Peças "dormindo" (hipótese forte):** o motor nativo mantém o estado `sleeping` de cada objeto (`updateObjectProperties` em `gamelogic.lua`). O editor acorda as peças só quando as move (`setSleeping(..., false)` em `editor.lua`), e o modo Demolisher as põe para dormir explicitamente. Isso indica que, no original, as construções começam em repouso e só reagem quando atingidas. No port, 15 fases têm alguma peça que se desloca mais de 3 unidades ao carregar (1-14, 2-9, 3-5, 3-13, 3-19, 4-4, 4-18, 5-6, 5-8, 5-9, 5-20, 5-21, 8-4, 8-9, 8-11). Exemplo na 4-4: um bloco apoiado no encontro de duas tábuas diagonais que, com as medidas exatas do APK, não se tocam; ele só fica de pé se começar dormindo. Implementar isso (`b2BodyDef.awake=false`) muda a física de todas as fases e exige refazer todos os roteiros: é o próximo passo recomendado.
- **Memória:** cada banco de texturas embutido custa 2,8 MB de RAM; o ELF chegou a 19,7 MB com 5 bancos. Para os próximos episódios, os bancos devem ser lidos do disco sob demanda em vez de ficarem dentro do ELF.
- **Bola superelástica:** o `blocks.lua` dá restituição 5,5 à ExtraTrampoline (o Box2D original também usa o maior valor do par), mantido como no dado.

## O que falta no jogo (código nativo não recuperado)

- Fórmulas de dano, de queda de explosão e de "sono" dos corpos: hoje são aproximações. São a causa provável de a 3-20 continuar sem vitória (cinco pássaros explosivos).
- Pontuação: as estrelas usam os limites originais, mas a pontuação do port é aproximada. A contagem de estrelas é, portanto, indicativa.
- Música de fundo, preenchimento de terreno (texturas de solo das cenas) e efeitos de partículas.

## Prioridade sugerida

1. Peças começando em repouso (sleeping), com novo levantamento de roteiros.
1b. Bancos de textura lidos do disco sob demanda.
1c. Terence (RedBigBird) e Stella (PinkBird) e as peças de Ham 'Em High.
2. `velocityMultiplier` por material (dado já recuperado).
3. Juntas prismáticas e cordas.
4. Pan de câmera e música.
