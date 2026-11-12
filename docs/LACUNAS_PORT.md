# Lacunas do port (levantamento atualizado na 0.21.0)

Base: os 762 níveis dos episódios do Classic foram decompilados do APK e comparados com o que o port já entende (definições de objetos, pássaros, juntas). Nenhum nível falhou na leitura.

## Cobertura por episódio (níveis totalmente suportados / total)

- Poached Eggs: 63/63 (o port inclui as 3 páginas, 63 fases)
- Mighty Hoax: 42/42 depois da 0.21.0 (o port inclui as páginas 4 e 5, 42 fases; as duas fases restantes eram as que usavam LightBlock8, maçã e bandeira)
- Danger Above: 19/45
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

## O que falta no jogo (código nativo não recuperado)

- Fórmulas de dano, de queda de explosão e de "sono" dos corpos: hoje são aproximações. São a causa provável de a 3-20 continuar sem vitória (cinco pássaros explosivos).
- Pontuação: as estrelas usam os limites originais, mas a pontuação do port é aproximada. A contagem de estrelas é, portanto, indicativa.
- Música de fundo, preenchimento de terreno (texturas de solo das cenas) e efeitos de partículas.

## Prioridade sugerida

1. Pássaros verde, Terence e Stella, mais blocos estáticos frágeis: liberam Danger Above e Ham 'Em High.
2. `velocityMultiplier` por material (dado já recuperado).
3. Juntas prismáticas e cordas.
4. Pan de câmera e música.
