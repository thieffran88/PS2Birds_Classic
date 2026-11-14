# Modificações completas — PS2Birds Classic 0.40.0

- **Episódio Dino (páginas 45 a 57, 46 fases)**
  - Ordem do `episodes.lua` (Dino1_profcar a Dino3_uphilldino, sem a cena de abertura) e limites do `starLimits.lua`.
  - O cenário do Dino ocupa muita memória de vídeo: cada página do jogo virou quatro ou cinco (2 a 4 fases por banco), bancos de textura 41 a 53; numeração 1-1 a 3-16.
  - A campanha passa de 591 para 637 fases; 31 das 46 novas já têm roteiro vencedor. As outras 15 continuam na busca e entram fase por fase nas próximas versões.
  - Ao carregar, nenhuma peça cai sozinha nas 46 fases.
- **107 peças novas pela `tools/add_definitions.py --material-kinds`**
  - Ossos, mandíbulas e crânios de dinossauro, máscaras e roupas tribais, frutas, palmeiras, plantas, ovos de dinossauro, pedras, colares e enfeites.
  - Todas guardadas em meia resolução e desenhadas com escala 2.
- **Gaiolas (novo no jogo)**
  - As peças `*_Cage` declaram `spawnNewBirdOnDestruction`; quebrar a gaiola põe o pássaro preso no fim da fila, como o `spawnBird` do `gamelogic.lua`.
  - Seis tipos: azul pequeno, amarelo, preto, BasicBird2, Terence e Stella. O `blocks.lua` escreve esse campo de duas formas, e as duas são lidas.
  - O pássaro liberado é guardado nos bits altos das marcas do objeto no `CAMPAIGN.BIN` (formato 3, sem mudança de tamanho).
  - Os sprites do pássaro liberado entram no banco da fase mesmo quando ele não está na fila inicial; os sprites das gaiolas só ficam nos bancos que as usam (os 41 bancos anteriores não mudaram).
- **Correção no extrator de sprites**
  - A folha `INGAME_DINOS_DECOS_1` não declara a extrusão dos quadros, mas guarda a borda; a margem agora vem do cabeçalho do quadro.
- **Levantamento do que falta (`docs/LACUNAS_PORT.md`)**
  - Ham 'Em High: 3 fases (4 peças); Flock Favorites: 30 fases (24 peças); Golden Eggs: 31 fases (15 peças); Tutorial: 15 fases (5 peças); Mighty Feathers: modo Egg Defender (48 peças).
  - Fora dos episódios: Mighty League (273), siege, powerupSchool, eagleDemo, ugc, packTalkweb.
- **Testes**
  - `globe_test`: quebrar uma gaiola acrescenta o pássaro à fila; peças espelhadas procuradas em todas as fases.
  - `profile_test`: Dino (ordem, 13 bancos, 107 peças, gaiolas e pássaros liberados no `CAMPAIGN.BIN`).
  - `sprite_test`: 54 bancos, 685 quadros; gaiolas só nos bancos do Dino.
  - `campaign_test`: 637 fases.
  - Suíte com 65 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.40.0; README com 637 fases e as gaiolas; captura do Dino 1-1; documentos e captura da 0.39.0 substituídos.
  - Drive: zips da 0.38.0 e documentos da 0.39.0 removidos.

## Modificações completas — 0.39.0

- **Episódio Piggy Farm (páginas 39 a 44, 45 fases)**
  - Ordem do `episodes.lua` (Farm1_Level106 a Farm3_tractorpulling, sem as cenas de abertura e encerramento) e limites do `starLimits.lua`.
  - Cada página do jogo virou duas (8 + 7 fases), bancos de textura 35 a 40, para caber na memória de vídeo; numeração 1-1 a 3-15.
  - A campanha passa de 546 para 591 fases; 44 das 45 novas têm roteiro vencedor (falta a 1-8).
  - Ao carregar, nenhuma peça cai sozinha nas 45 fases.
- **73 peças novas pela `tools/add_definitions.py --material-kinds`**
  - Moinhos, tratores e pneus, fardos de feno, trigo, capim, repolho, sombras, cacto, fogareiro, roupa de vaca, cesta, escada, flores, ovos e orelhas de coelho de Páscoa, abóbora, pimenta, peças de avião e balão dourado de Bad Piggies, blocos brancos (`WhiteBlock*`, tipo madeira).
  - Todas guardadas em meia resolução e desenhadas com escala 2.
- **Material sem colisor (`decoration_new`)**
  - O `blocks.lua` declara `collider = collider_types.none` nesse material; a ferramenta passou a ler isso e as 11 decorações de fazenda entram como cenário fixo sem colisão. Nenhuma peça anterior usava esse material.
- **Peças espelhadas (novo no jogo)**
  - Escala negativa (`scaleX`/`scaleY` < 0) espelha a peça no editor do jogo. O importador usa o tamanho em módulo e guarda o espelhamento; o jogo espelha triângulos e polígonos na física e o sprite no desenho. São 143 peças espelhadas na campanha.
- **Correções**
  - Pivôs fracionários em algumas folhas de sprites do APK eram rejeitados; agora são arredondados (os sprites anteriores não mudaram).
  - A tabela do `campaign_test` aceita roteiros de até 8 tiros (fases com 6 pássaros).
- **Testes**
  - `profile_test`: Piggy Farm (ordem, seis bancos, 73 peças, material sem colisor, peças espelhadas no `CAMPAIGN.BIN`); teste de tamanhos com o módulo da escala.
  - `globe_test`: peças espelhadas chegam ao desenho com a marca de espelhamento.
  - `sprite_test`: 41 bancos, 579 quadros; peças da fazenda só nos bancos 35-40, em meia resolução.
  - Suíte com 64 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.39.0; README com 591 fases; captura do Piggy Farm 1-1; documentos e captura da 0.38.0 substituídos.
  - Drive: zips da 0.37.0 e documentos da 0.38.0 removidos.

## Modificações completas — 0.38.0

- **Episódio Bird Island (página 38, 21 fases)**
  - Ordem do `episodes.lua` (Level232_Movie a Level265_t_Movie) e limites do `starLimits.lua`; banco de texturas 34.
  - A campanha passa de 525 para 546 fases; 19 das 21 novas têm roteiro vencedor (faltam 1-10 e 1-15).
  - Ao carregar, nenhuma peça cai sozinha nas 21 fases.
- **17 peças novas pela `tools/add_definitions.py --material-kinds`**
  - Estátua de pássaro em cinco partes, sacos de areia, figuras e montes de areia, flor, manchas de tinta, ingresso, pôsteres.
  - **Rei porco barbudo (`BeardedKingPiglette`)**: material `piglette` e `levelGoal` no `blocks.lua`, então entra como porco (tipo 4, conta como alvo), raio 2,825, resistência 100, com o sprite próprio.
- **Correção na `add_definitions.py`**
  - A busca do sprite também casava com `spriteScore`; quando a pontuação vinha antes no `blocks.lua`, a peça recebia o sprite de pontos (o rei barbudo apareceu como "5K_GREEN"). Agora só `sprite = ...` conta. Nenhuma peça já importada mudou.
  - O material `piglette` passa a dar o tipo porco com `--material-kinds`; o jogo desenha porcos importados com o sprite próprio.
- **Testes**
  - `profile_test`: Bird Island (ordem, banco, 17 peças, o rei barbudo como porco com o sprite certo).
  - `sprite_test`: 35 bancos, 508 quadros; peças do episódio só no banco 34.
  - Suíte com 62 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.38.0; README com 546 fases; captura do Bird Island 1-1; documentos e captura da 0.37.0 substituídos.
  - Drive: zips da 0.36.0 e documentos da 0.37.0 removidos.

## Modificações completas — 0.37.0

- **Episódio Short Fuse (páginas 35 a 37, 45 fases)**
  - Ordem do `episodes.lua` (BlackBird_1 a BlackBird_45) e limites do `starLimits.lua`; bancos de texturas 31 a 33.
  - A campanha passa de 480 para 525 fases; 37 das 45 novas têm roteiro vencedor (faltam 1-7, 1-15, 2-2, 2-4, 2-5, 2-7, 2-13 e 3-3).
- **Pássaro de choque (`BlackBird_Shockwave`), novo**
  - Corpo e valores do `blocks.lua`; lógica do `subsystems/blackbird/BlackBirdAbility.lua`.
  - Dispara no toque ou 1,5 s depois do primeiro contato; carrega por 0,4 s e solta uma onda elétrica que cresce até 15 unidades em 1,9 s.
  - Cada peça dentro da onda leva dano uma vez (25 a 35% da resistência, menos a defesa) e um empurrão proporcional à distância (força de 470 caindo a 25); juntas quebráveis presas a ela se soltam.
  - Porcos dentro da onda são eletrocutados e morrem 2 s depois; o turno não termina enquanto a onda ou a eletrocução estão em curso.
  - Sprites próprios (normal e em voo), só nos bancos do Short Fuse; som de lançamento do pássaro preto.
- **70 peças de laboratório pela `tools/add_definitions.py --material-kinds`**
  - Tubos (enferrujados e de metal), funis, frascos e garrafas, cristais, cogumelos, flores, placas de aviso, bico de Bunsen, termômetro, manômetro, ímã, foguete, máscaras, cabelos e óculos.
- **Save versão 7 (1024 fases)**
  - O save da 0.36.0 (versão 6, 512 fases) é lido e regravado no novo formato; a campanha passou de 512 fases.
  - Limite da campanha no jogo e no gerador: 1024 fases.
- **Investigação**
  - Mighty Feathers é inteiro o modo "Egg Defender" (defender ovos), com scripts de jogabilidade próprios (`EggDefender_*.scripts.lua`): fica para uma etapa própria.
  - Os frascos coloridos (`VialEffects.lua`), o foguete (`RocketCanister.lua`), o ímã e o balão explosivo têm efeitos próprios ainda não portados; por enquanto são peças comuns.
  - Short Fuse 2-13 e 3-3: vidro leve soldado a pedra muito mais pesada deixa o Box2D do port instável e algumas peças quebram ao carregar. Mudar o solver afetaria todas as fases; ficou registrado.
- **Testes**
  - `globe_test`: o pássaro de choque carrega, a onda cresce e os porcos são eletrocutados.
  - `save_test`: migração v6→v7 (480 → 481 fases) e limite de 1024 fases.
  - `profile_test`: Short Fuse (ordem, bancos, 70 peças e os valores da onda contra o `blocks.lua`).
  - `sprite_test`: 34 bancos, 491 quadros; `campaign_test` com 525 fases e as duas fases instáveis marcadas.
  - Suíte com 60 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.37.0; README com 525 fases e dez pássaros; captura do Short Fuse 1-1; documentos e captura da 0.36.0 substituídos.
  - Drive: zips da 0.35.0 e documentos da 0.36.0 removidos.

## Modificações completas — 0.36.0

- **Episódio Bad Piggies (páginas 32 a 34, 45 fases)**
  - Ordem do `episodes.lua` (Level743 a Level1247) e limites do `starLimits.lua`; bancos de texturas 28 a 30.
  - A campanha passa de 435 para 480 fases; 41 das 45 novas têm roteiro vencedor (faltam 2-7, 2-15, 3-5 e 3-12).
  - Ao carregar, nenhuma peça cai sozinha nas 45 fases.
- **6 peças de veículo pela `tools/add_definitions.py --material-kinds`**
  - Saco, óculos, galão de óleo, capacete de piloto, pneu de madeira (tipo madeira) e chave inglesa dourada.
  - As juntas do episódio (446 soldas, 191 dobradiças, uma com motor, uma prismática com motor, 10 "destruir anexo") já eram suportadas.
- **Testes**
  - `profile_test`: Bad Piggies (ordem das 45 fases, bancos 28-30, 6 peças contra o `blocks.lua`).
  - `sprite_test`: 31 bancos, 419 quadros.
  - Suíte com 58 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.36.0; README com 480 fases; captura do Bad Piggies 1-1; documentos e captura da 0.35.0 substituídos.
  - Drive: zips da 0.34.0 e documentos da 0.35.0 removidos.

## Modificações completas — 0.35.0

- **Episódio Surf and Turf (páginas 29 a 31, 45 fases)**
  - Ordem do `episodes.lua` (LevelS001 a LevelS045) e limites do `starLimits.lua`; bancos de texturas 25 a 27.
  - A campanha passa de 390 para 435 fases; 42 das 45 novas têm roteiro vencedor (faltam 2-15, 3-5 e 3-14).
  - Ao carregar, nenhuma peça cai sozinha nas 45 fases.
- **14 peças de praia pela `tools/add_definitions.py --material-kinds`**
  - Boia, balde, castelo de areia, óculos, martelo, chapéus, rede, vara de pesca, pá, graveto e as duas partes do guarda-sol.
  - Cada peça fica com o tipo do seu material (madeira, vidro, pedra ou extra).
- **Investigação**
  - O campo `chains` que aparece nas fases deste episódio está vazio em todas as fases do APK: não há correntes a portar.
- **Testes**
  - `profile_test`: Surf and Turf (ordem das 45 fases, bancos 25-27, 14 peças contra o `blocks.lua`).
  - `sprite_test`: 28 bancos, 413 quadros; peças de praia só nos bancos 25-27.
  - Suíte com 56 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.35.0; README com 435 fases; captura do Surf and Turf 1-1; documentos e captura da 0.34.0 substituídos.
  - Drive: zips da 0.33.0, um código-fonte da 0.31.0 que tinha ficado para trás e documentos da 0.34.0 removidos.

## Modificações completas — 0.34.0

- **Episódio Birdday 7 (página 28, 15 fases)**
  - Ordem do `episodes.lua` (Level263_BirdDay7 a Level301_BirdDay7) e limites do `starLimits.lua`; banco de texturas 24.
  - A campanha passa de 375 para 390 fases; 13 das 15 novas têm roteiro vencedor (faltam 7-7 e 7-14).
- **36 peças novas pela `tools/add_definitions.py`**
  - Conjunto de madeira temático (11 blocos `WoodBlock*_BD7`), cinco balões com a força do material `balloon`, cupcake, bandeiras e adesivo sem colisão, arbustos, montes de areia e paredes de cenário, flores e folhas da ilha, barril, motor e chave inglesa de Bad Piggies.
- **Tipo do material nas peças temáticas (`--material-kinds`)**
  - Peças de madeira, vidro e pedra adicionadas a partir do Birdday 7 ficam com o tipo do material (dano por pássaro, som, pontuação), como no original; antes toda peça automática virava "extra".
  - Material `immovableFragile` (flores e folhas) vira cenário frágil, que estoura ao ser tocado.
  - As peças dos episódios anteriores não mudaram (nenhum roteiro afetado).
  - O jogo desenha esses blocos temáticos com o sprite próprio.
- **Testes**
  - `profile_test`: Birdday 7 (ordem, banco, 36 peças contra o `blocks.lua`, tipo pelo material, força dos balões); verificação de Birdday 6 restrita às fases dele.
  - `sprite_test`: 25 bancos, 399 quadros; peças do Birdday 7 só no banco 24.
  - Suíte com 54 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.34.0; README com 390 fases; captura da Birdday 7-6; documentos e captura da 0.33.0 substituídos.
  - Drive: zips da 0.32.0 e documentos da 0.33.0 removidos.

## Modificações completas — 0.33.0

- **Episódio Birdday 5 (30 fases, todas com roteiro vencedor)**
  - Ordem do `episodes.lua` (UGC1Level01 a 30) e limites do `starLimits.lua`; numeradas de 5-1 a 5-30.
  - Dividido em seis páginas de 5 fases (bancos de textura 18 a 23) para caber na memória de vídeo.
  - A campanha passa de 345 para 375 fases; 368 com roteiro vencedor.
- **90 peças novas, todas pela `tools/add_definitions.py`**
  - 39 sem colisão (papelão rasgado, fitas, clipes, carimbos, post-its), 7 fixas com colisão (régua dobrável, lápis, borracha, transferidor, compasso, rinoceronte), chapéus, máscaras, óculos, frascos de vidro, TV, câmera, motor de Bad Piggies e a vela do bolo.
  - Peças de material `immovable` entram como cenário fixo; o jogo passou a desenhar peças fixas com o sprite colorido próprio (antes só sabia desenhar máscaras de chão).
- **Texturas**
  - O extrator lê também as folhas de tema (`themes_android/THEME_UGC_*`), em PNG com borda de 1 pixel.
  - As 90 peças e os objetos grandes do tema UGC ficam em meia resolução e são desenhados com escala 2 (marca `half_resolution` no perfil).
  - Nos bancos 18+ os estados de dano dos blocos e os quadros de voo/colisão dos pássaros usam o quadro normal.
  - O empacotador tenta outras ordens de arrumação quando a primeira não cabe; os bancos 0 a 17 ficam iguais.
  - 24 bancos, 365 sprites.
- **Limites maiores por fase**
  - 640 peças e 512 juntas por fase (antes 352 e 64): cobre todos os episódios do APK, inclusive Dino e Golden Eggs.
- **Correção encontrada pelos testes**
  - A primeira versão da regra de quadros de dano trocava blocos de tamanhos diferentes (um bloco intacto de tamanho 2 viraria o de tamanho 1); corrigido antes da versão.
- **Ferramentas**
  - Títulos de páginas que continuam a numeração (`first`), para o Birdday 5 numerar 5-1 a 5-30.
  - `settle_probe` lista as peças que mais se mexem antes do primeiro tiro (`SETTLE_LIST=1`).
- **Testes**
  - `profile_test`: Birdday 5 (ordem, seis bancos, numeração, 90 peças contra o `blocks.lua`, 39 sem colisão).
  - `sprite_test`: 24 bancos, 365 quadros; peças do Birdday 5 só nos bancos 18-23 e em meia resolução; regras de quadros repetidos conferidas.
  - Suíte com 52 verificações aprovadas.
- **Versão, documentos e limpeza**
  - Tela e log em 0.33.0; README com 375 fases; captura da Birdday 5-3; documentos e captura da 0.32.0 substituídos.
  - Drive: zips da 0.31.0 e documentos da 0.32.0 removidos.

## Modificações completas — 0.32.0

- **Episódio Birdday 6 (página 21, 15 fases)**
  - Ordem do `episodes.lua` e limites de estrelas do `starLimits.lua`; banco de texturas 17.
  - A campanha passa de 330 para 345 fases; 14 das 15 novas têm roteiro vencedor (falta a Birdday 6-1).
  - Quatro peças novas pela `tools/add_definitions.py`: bolo do Birdday 5 (polígono), balão simples de Bad Piggies, roda de madeira fraca e o bloco de cenário sem colisão.
- **Junta prismática (tipo 4), nova**
  - Eixo (`worldAxisX/Y`), limites, motor (`motorSpeed`, `maxTorque` como força máxima) e `backAndForth`.
  - O `gamelogic.lua` chama a função nativa `checkJointLimits` para essas juntas; o port inverte o motor a cada limite.
  - Usada pelos balões que carregam porcos no Birdday 6 (9 juntas).
- **Motor e limites nas dobradiças (tipo 3) — correção**
  - Três dobradiças de fases já suportadas têm motor no APK e o port as deixava soltas; agora giram como no original.
  - Nenhum roteiro antigo deixou de vencer.
- **Peças sem colisão**
  - `collision = false` no `blocks.lua` vira sensor do Box2D: a peça é desenhada e nada esbarra nela.
  - Peças de material `immovable` adicionadas automaticamente entram como cenário fixo (não contam como blocos).
- **Força da própria peça e velocidade-alvo**
  - A `add_definitions.py` lê `forceX`/`forceY` da definição (antes só do material) e `targetVelocity`.
  - O balão simples (força −80) deixa de ser empurrado quando já sobe a 20 unidades/s.
- **Formato do `CAMPAIGN.BIN` (versão 3)**
  - Juntas com eixo, limites, motor e marcas; peças com marca de "sem colisão"; forças com velocidade-alvo.
- **Ferramentas**
  - `gerar_iso.py --dev` também atualiza `sprite_ids.h` e `definition_sprites.h` quando entram peças ou episódios.
  - Cache de tamanhos de sprites refeito direto das folhas do APK (2078 quadros).
- **Testes**
  - `joint_test`: bloco em junta prismática com motor, limites e vai-e-vem; dobradiça girando por motor; peça que atravessa o cenário sem colisão.
  - `profile_test`: Birdday 6 (ordem, banco, peças contra o `blocks.lua`, 9 juntas prismáticas e as dobradiças com motor no `CAMPAIGN.BIN`).
  - `sprite_test`: 18 bancos, 276 quadros; peças novas só no banco 17. Suíte com 50 verificações aprovadas.
- **Investigação**
  - Levantamento de todos os episódios do APK: Birdday 5 precisa de 90 peças (a maioria cenário de papelão sem colisão e objetos de escritório), Birdday 7, Mighty Feathers, Short Fuse, Bird Island, Piggy Farm, Dino e outros têm temas próprios. Registrado em `LACUNAS_PORT.md`.
- **Versão, documentos e limpeza**
  - Tela e log em 0.32.0; README com 345 fases; captura da Birdday 6-2; documentos e captura da 0.31.0 substituídos.
  - Removidos `solver2.cpp` e `settle_detail.cpp` (ferramentas de teste sem uso); `profiles/README.md` reescrito (citava um arquivo que já tinha saído).
  - Drive: zips da 0.30.0 removidos (você já tinha enviado os da 0.31.0).

## Modificações completas — 0.31.0

- **Gerador de ISO a partir do APK (novo)**
  - `tools/gerar_iso.py`: recebe o APK original e grava a ISO (e, se pedido, uma pasta para USB/HDD) em menos de 1 minuto; confere o SHA-256 do APK suportado (`profiles/supported_apk.json`).
  - Kit para Windows (`tools/make_windows_kit.py`): `GERAR_ISO.bat`, `LEIA-ME.txt`, `PS2BIRDS.ELF`, as ferramentas e o Python 3.12 oficial (NuGet) com pycdlib e ffmpeg; nada é instalado no Windows.
  - Testado com Wine: os arquivos gerados no "Windows" são idênticos aos do Linux, e a ISO abre no PCSX2.
- **O jogo não embute mais nada do original**
  - O ELF (4,6 MB, antes 10,1 MB) lê na inicialização `CAMPAIGN.BIN` (fases), `SPRITES.BIN` (tabelas de quadros, todas as texturas e o ícone do cartão) e `SOUNDS.BIN` (efeitos), no disco ou ao lado do ELF.
  - Sem os arquivos, o jogo mostra a tela "ARQUIVOS DE DADOS AUSENTES" em vez de travar.
  - Leitura do disco em blocos de 256 KiB (open/read) no lugar do `fread`: um banco de texturas passou de cerca de 17 s para 3 s no PCSX2, na inicialização e em cada troca de episódio.
  - Cabeçalhos novos só com identificadores do port: `sprite_ids.h`, `audio_ids.h`, `save_assets.h`; `campaign_data.h` (5,3 MB) e `level_data.h` saíram.
- **Leitores próprios em Python puro (sem programas externos)**
  - `lua51_data.py`: lê as fases direto do bytecode Lua 5.1 do APK, só com operações de dados (recusa qualquer chamada ou laço). Nas 330 fases o resultado é idêntico ao do decompilador unluac, que deixa de ser necessário.
  - `aes_cbc.py` (no lugar do OpenSSL), `mini7z.py` (7-Zip/py7zr), `png_decode.py` (Pillow) e `spu_adpcm.py` (adpenc do PS2SDK).
  - Texturas e `CAMPAIGN.BIN` saem byte a byte iguais aos do processo antigo.
- **`deathSpawn` (lacuna da 0.30.0, resolvida)**
  - O bolo grande de Birdday 3/4 solta 10 cremes ao ser destruído, como no `gamelogic.lua`: na posição do bolo, com os valores do próprio `blocks.lua` (raio 1,3, densidade 0,1, resistência 10).
  - Formato do `CAMPAIGN.BIN` com o campo `spawn_of`; 18 bolos em 11 fases, 180 cremes.
  - O sprite do creme passou a estar também no banco 15 (Birdday 3), onde faltava.
  - Roteiro da Birdday 4-15 refeito, porque a física dela mudou com os cremes; as outras 10 fases com bolo continuam vencendo com o roteiro antigo.
  - Campanha: 324 de 330 fases com roteiro vencedor (sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6 e Birdday 4-4).
- **Repositório sem dados do jogo**
  - Saíram do Git: 802 arquivos de `ps2/data` (Lua decompilado, fases em JSON) e `ps2/assets` (texturas, sons, manifestos). `.gitignore` impede que voltem.
  - Dados numéricos do port foram para `profiles/` (tamanhos de sprites, limites de estrelas, parâmetros dos pássaros); saiu a cópia do README de outro projeto.
  - Modo de desenvolvimento: `gerar_iso.py APK --dev` recria `ps2/assets` e `ps2/data` para compilar e testar.
- **Testes**
  - Novo `tools_test.py`: AES (vetor oficial FIPS-197), leitor Lua (recusa código executável), 7z, PNG e ADPCM.
  - `globe_test`: cremes escondidos no início e soltos quando o bolo cai; `profile_test`: valores do `deathSpawn` contra o `blocks.lua` e leitura do `CAMPAIGN.BIN`.
  - `sprite_test`: formato novo do `SPRITES.BIN`; `audio_test`: `SOUNDS.BIN` sintético e arquivo ausente.
  - Testes de procedência (contra scripts decompilados) rodam só onde o desenvolvedor tem esses arquivos. Suíte com 48 verificações aprovadas.
- **Limpeza**
  - `docs`: removidos 59 documentos por versão (0.1 a 0.29), 17 listas de hashes, logs de boot, diagnóstico inicial e capturas antigas; o histórico completo segue em `HISTORICO_MODIFICACOES.md`.
  - `tools`: removidos `split_sprite_banks.py` e `py7z.py`, substituídos.
  - README reescrito para o novo fluxo; versão 0.31.0 na tela e no log; captura da Birdday 3-2 no lugar da 0.30.0; documentos da 0.30.0 substituídos.

## Modificações completas — 0.30.0

- **Stella (PinkBird), novo pássaro**
  - Corpo do `blocks.lua`: raio 0,85, densidade 6, atrito 0,3, restituição 0,43; dano ×0,5 em blocos (`PinkBirdDamageFactors`).
  - Algoritmo do `bubbles.lua`, `default_conf.lua`, `mode_MuseumMode.lua` e `CollisionCallbacks.lua` (trechos em `ps2/data/bubbles_excerpt.lua`):
    - no toque, ou 1,5 s depois do primeiro contato, a Stella para no ar e flutua por 0,5 s;
    - em seguida solta bolhas num raio de 7: cada peça dinâmica dentro dele flutua por 2 s (antigravidade ×1,8, subida limitada a 10 u/s, empurrão lateral ×1,3 da distância);
    - juntas quebráveis presas a peças com bolha se soltam;
    - a Stella some quando a própria bolha acaba.
  - Sprites originais (normal e "solta"), só nos bancos que usam o pássaro.
  - Teste `globe_test` ampliado: flutuação, subida máxima e saída em 2,5 s.
- **Episódios Birdday 3 e 4 (páginas 19 e 20, 30 fases)**
  - Ordem do `episodes.lua`, limites de estrelas do `starLimits.lua`; bancos de texturas 15 e 16.
  - A campanha passa de 300 para 330 fases.
  - Peças novas pela `tools/add_definitions.py`: bolo grande (`Spawning_cake`), creme (`CakeCream`) e chapéus de festa 5, 6 e azul.
- **Correção: tamanho declarado das peças automáticas**
  - Peças com largura/altura (caixa) ou raio (círculo) declarados no `blocks.lua` agora usam esses valores; antes, usavam o tamanho do sprite ×0,92.
  - Afeta peças de Mine and Dine e Birdday 2 (ex.: estalactite 3 × 13,9; balão 2,4). Os roteiros das fases 240–299 foram descartados e refeitos.
- **Correção: `add_definitions.py` pegava a definição errada**
  - Quando um nome aparece mais de uma vez no `blocks.lua` (ex.: tabela de deslocamentos de chapéus), agora vale a última atribuição que tem forma (`polygon`, `box`, `circle`).
- **Busca de roteiros**
  - 90 fases (240–329) refeitas com a busca em feixe (grade de 10°/10%) e uma segunda passada em grade de 5° nas que sobraram.
  - Campanha: 324 de 330 fases com roteiro vencedor. Sem roteiro: 3-20, 5-16, 13-7, 16-5, 16-6 e Birdday 4-4.
- **Testes**
  - Procedência de Birdday 3/4: ordem, bancos, corpo e bolhas da Stella contra os scripts; peças de festa.
  - Teste de tamanhos aceita as medidas declaradas no `blocks.lua`.
  - Sprites: 17 bancos, 273 quadros. Tabela da campanha com 330 fases. Suíte com 47 verificações aprovadas.
- **Lacuna conhecida**
  - `deathSpawn` do bolo grande (soltar 10 cremes ao ser destruído) não foi portado; o bolo some sem soltar nada.
  - Peças com tamanho declarado podem ter o desenho um pouco diferente do corpo físico.
- **Versão e documentos**
  - Tela e log em 0.30.0; README, lacunas, validação e histórico atualizados; captura da Birdday 3-1; documentos e captura da 0.29.0 removidos.


## Modificações completas — 0.29.0

- **GlobeBird (pássaro que infla), novo**
  - Corpo do `blocks.lua`: raio 0,85, densidade 5, atrito 0,3, restituição 0,25; dano ×3 em madeira e gelo e ×1,5 em pedra (`GlobeBirdDamageFactors`), zero quando inflado (`GlobeBirdBigDamageFactors`).
  - Algoritmo do `puffer_bird.lua` (cópia em `ps2/data/puffer_bird_excerpt.lua`): infla no toque ou 1,5 s depois do primeiro contato; raio 7,5, atrito 0,5, restituição 0, densidade 0,1; explosão de empurrão (raio 8, força 45000); fica 1,5 s inflado, encolhe em 1,5 s com mudanças de velocidade de ±50 a cada 0,05 s e some.
  - Sprites originais (normal e inflado, este em meia resolução), só nos bancos que usam o pássaro.
  - Novo teste `globe_test`: tempos de inflar e encolher, uso depois do contato e inflar automático.
- **Episódio Birdday 2 (página 18, 15 fases)**
  - Ordem do `episodes.lua`, limites de estrelas do `starLimits.lua`; banco de texturas 14.
  - A campanha passa de 285 para 300 fases; as 15 novas com roteiro vencedor.
  - 13 peças de festa adicionadas pela `tools/add_definitions.py`: balões de aniversário (4), bolos (3), vela, chapéus de festa (4) e bolo especial; materiais `balloon` e `cake`.
- **Força de material (balões)**
  - `add_definitions.py` registra `forceX`/`forceY` do material; o gerador cria a tabela `campaign_forces`; o motor aplica força × massa a cada passo (aproximação do `setObjectAsForceAdder` nativo). Os balões sustentam as cordas.
- **Correção: junta tipo 5 ("Destroy-attached")**
  - Agora a peça anexada é removida quando a outra ponta é destruída, depois de `destroyTimer`; antes, era removida pelo relógio da fase, o que destruía peças ao carregar a 2-15.
  - Roteiros refeitos para as fases afetadas: 16-7 resolvida; 15-12 em busca.
  - Remoção de porco pela junta soma 5000 pontos e gera o evento de porco.
- **Investigação e busca**
  - A busca em feixe com grade de 10° e 10% resolveu as 5 fases difíceis de Birdday 2; a 16-2 de Mine and Dine foi resolvida pela busca da rodada anterior; as fases grandes restantes continuam na fila.
- **Testes**
  - Novo teste de procedência de Birdday 2: GlobeBird (corpo, inflado, tempos, explosão) contra `blocks.lua` e `puffer_bird.lua`; peças de festa e força dos balões.
  - Teste de sprites: 15 bancos (266 quadros), sprites do GlobeBird só no banco 14 e quadro inflado em meia resolução.
  - Tabela da campanha com 300 fases: 289 com roteiro. Suíte com 46 verificações.
- **Versão e documentos**
  - Tela e log em 0.29.0. README, lacunas, validação e histórico atualizados; captura da Birdday 2-1; captura da 0.28.0 removida.
- **Drive**
  - Arquivos 0.27.0 removidos (substituídos pelos 0.28.0 que você enviou); documentos 0.28.0 substituídos pelos 0.29.0.

## Modificações completas — 0.28.0

- **Episódio Mine and Dine (15-1 a 17-15)**
  - 45 fases na ordem dos mundos 15, 16 e 17 do `episodes.lua` (trechos em `ps2/data/episode_mine_and_dine_page1..3.decompiled.lua`), com limites de estrelas do `starLimits.lua`.
  - A campanha passa de 240 para 285 fases; 37 das 45 novas com roteiro vencedor.
  - Bancos de texturas 11, 12 e 13 no `SPRITES.BIN`; o ELF tem 9,6 MB.
- **20 peças novas (trecho em `ps2/data/mine_and_dine_definitions_excerpt.lua`)**
  - Chapéus de mineiro (3) e do rei mineiro, pás (2), picaretas (2, triângulos), ametista, âmbar, rubi, cordas finas (2), estalactites (3) e pontas de estalactite (4).
  - Materiais novos lidos da tabela de materiais: amethyst, amber, ruby, stalaktite e stalaktite_tip.
- **Ferramenta nova `tools/add_definitions.py`**
  - Lê do `blocks.lua` o tipo, o sprite, os vértices, o material, o atrito e a restituição de cada peça, e do APK o tamanho do sprite.
  - Marca as peças como `auto`: o extrator inclui seus sprites e o gerador da campanha cria `ps2/src/definition_sprites.h`, que o jogo usa para desenhá-las sem novas linhas no `main.c`.
- **Extrator de sprites:** lê folhas RGBA8888 (sem a borda de 1 pixel), além das RGBA4444.
- **Motor**
  - Peças com densidade 0 são fixas, como no `ObjectCreation.lua`.
  - Junta tipo 5 ("Destroy-attached"): segura como solda e remove a peça anexada depois do `destroyTimer` (aproximação; a lógica original é nativa).
  - Limite de peças por fase de 224 para 352.
- **Save v6:** 512 posições (2.080 bytes); saves v5 são lidos e migrados (um save de 240 fases com a 14-15 vencida libera a 15-1).
- **Texturas:** nos bancos 11–13, porco machucado com o quadro normal e tábuas 10×1 com dois quadros de dano, para caber na VRAM. Bancos anteriores sem mudança.
- **Testes**
  - Novo teste de procedência de Mine and Dine: as 20 peças conferidas contra o `blocks.lua` e seus materiais (usando o próprio leitor da ferramenta), juntas tipo 5 com os tempos, regra de densidade 0 e tabela de sprites gerada.
  - Teste de sprites: 14 bancos (254 quadros) e os chapéus RGBA8888 com alfa completo.
  - Teste de save: migração v5 → v6 e limite de 512 posições.
  - Tabela da campanha com 285 fases: 274 com roteiro. Suíte com 43 verificações.
- **Versão e documentos**
  - Tela e log em 0.28.0. README (save v6), lacunas, validação e histórico atualizados; captura da 15-2 no PCSX2; captura da 0.27.0 removida.
- **Drive**
  - Arquivos 0.26.0 removidos (substituídos pelos 0.27.0 que você enviou); documentos 0.27.0 substituídos pelos 0.28.0.

## Modificações completas — 0.27.0

- **Episódio Ham 'Em High completo (12-1 a 14-15)**
  - 45 fases na ordem dos mundos 12, 13 e 14 do `episodes.lua` (trechos em `ps2/data/episode_ham_em_high_page1..3.decompiled.lua`), com limites de estrelas do `starLimits.lua`.
  - A campanha passa de 195 para 240 fases; 44 das 45 novas com roteiro vencedor.
  - Um banco de texturas por página (bancos 8, 9 e 10) no `SPRITES.BIN`; o ELF cresceu 0,3 MB (9,1 MB).
- **Peças novas (definições conferidas contra o `blocks.lua`, trecho em `ps2/data/ham_em_high_definitions_excerpt.lua`)**
  - Chapéus de faroeste como polígonos de 6 vértices: ExtraCowboyHelmetSmall_1, ExtraCowboyHelmet_1, ExtraSheriffHat_1, ExtraSheriffHat_2 e ExtraBanditoHat_1, com atrito 1 e restituição próprios.
  - BP_ROPE_THICK: segmento curto de corda.
  - ExtraSuperBowl: a tigela do ovo dourado, peça fixa.
- **Investigação**
  - A 13-7 (17 porcos, 3 pássaros) ficou sem roteiro; a vitória depende da explosão do pássaro preto, cuja fórmula é nativa.
  - Registrado que a quarta página do mundo (fases bônus) depende de blocos de tema 16 e de uma fase ausente do APK.
- **Testes**
  - Novo teste de procedência de Ham 'Em High: ordem, bancos 8–10, vértices, sprites, atrito e restituição dos chapéus, corda e tigela.
  - Testes de sprites cobrem 11 bancos (234 quadros).
  - Tabela da campanha com 240 fases: 237 com roteiro (3-20, 5-16 e 13-7 sem). Suíte com 41 verificações.
- **Versão e documentos**
  - Tela e log em 0.27.0. README, lacunas, validação e histórico atualizados; captura da 12-2 no PCSX2; captura da 0.26.0 removida.
- **Drive**
  - Arquivos 0.25.0 removidos (substituídos pelos 0.26.0 que você enviou); documentos 0.26.0 substituídos pelos 0.27.0.

## Modificações completas — 0.26.0

- **Episódio Big Setup completo (9-1 a 11-15)**
  - 45 fases na ordem dos mundos 9, 10 e 11 do `episodes.lua` (trechos em `ps2/data/episode_big_setup_page1..3.decompiled.lua`), com limites de estrelas do `starLimits.lua`.
  - A campanha passa de 150 para 195 fases, as 45 novas com roteiro vencedor.
  - Um banco de texturas por página (bancos 5, 6 e 7), lidos do `SPRITES.BIN`; o ELF só cresceu 0,3 MB.
- **Terence (RedBigBird), novo pássaro**
  - Corpo do `blocks.lua`: raio 1,8, densidade 10, atrito 0,3, restituição 0,45; sem habilidade especial.
  - Dano ×0,3 em madeira, gelo e pedra (`RedBigBirdDamageFactors`).
  - Sprite original (BIRD_BIG_BROTHER), só nos bancos que o usam.
- **Polígonos livres**
  - Novo formato `polygon` no perfil, com os vértices do `blocks.lua`; o gerador da campanha emite uma tabela de polígonos, e o motor monta o corpo com a regra do `ObjectCreation.lua` (até 8 vértices).
  - Capacetes pequeno, grande e de chefe (hexágonos), com atrito 1 e restituição próprios; sprites originais da folha INGAME_BLOCKS_2_1.
- **Corda grossa (ExtraRopeThick01):** segmentos-caixa ligados por juntas de distância.
- **Escala por peça (correção):** `scaleX`/`scaleY` da instância agora multiplicam o tamanho, como no original; muda um bloco da 6-14, que ganhou roteiro novo.
- **Testes**
  - Novo teste de procedência de Big Setup: ordem, bancos 5–7, Terence, capacetes (vértices, sprite, atrito, restituição) e regra de vértices do `ObjectCreation.lua`, corda e escala por peça.
  - O teste de tamanhos considera a escala por peça.
  - Testes de sprites cobrem 8 bancos (227 quadros) e Terence/capacetes só nos bancos 5–7.
  - Tabela da campanha com 195 fases: 193 com roteiro (3-20 e 5-16 seguem sem). Suíte com 39 verificações.
- **Versão e documentos**
  - Tela e log em 0.26.0. README, lacunas, validação e histórico atualizados; captura da 9-11 no PCSX2; capturas da 0.25.0 removidas.
- **Drive**
  - Arquivos 0.25.0 substituídos pelos 0.26.0.

## Modificações completas — 0.25.0

- **Texturas lidas do disco (`SPRITES.BIN`)**
  - O banco de texturas 0 (Poached Eggs) continua dentro do ELF; os bancos 1 a 4 foram para o arquivo `SPRITES.BIN` (11,1 MB).
  - Quando o episódio muda, o banco é lido para um único buffer (2,75 MB) e enviado à VRAM como antes.
  - O ELF caiu de 19,7 MB para 8,6 MB. Cada episódio novo passa a ocupar disco, não RAM.
  - Caminho do arquivo: `cdrom0:\SPRITES.BIN;1` quando o jogo roda do disco; a pasta do ELF nos outros casos (`mass:`, `host:` etc.).
  - Sem o arquivo, o jogo abre normalmente, mostra "SPRITES.BIN AUSENTE: COPIE-O PARA A PASTA DO ELF" e usa as texturas do banco 0.
  - Novo `tools/split_sprite_banks.py` divide o atlas no build; `make iso` grava o `SPRITES.BIN` na ISO (`tools/make_iso.py --extra`).
  - O jogo registra no console o caminho com que foi iniciado, para diagnóstico.
- **Investigação: peças "dormindo"**
  - Testado criar as peças em repouso: elimina todo movimento ao carregar, mas muda a física de todas as fases (109 roteiros deixam de vencer) e contraria o comportamento conhecido do original, em que construções às vezes se acomodam antes do tiro.
  - Mantido como opção de compilação `SCENE_STARTS_ASLEEP`, desligada.
- **Investigação: Big Setup**
  - O próximo episódio (45 fases) precisa do Terence e de capacetes poligonais; definições lidas e registradas em `docs/LACUNAS_PORT.md`.
- **Testes**
  - Novo teste do `SPRITES.BIN`: banco 0 no ELF, bancos 1–4 no arquivo, idênticos byte a byte ao atlas.
  - Suíte completa com 37 verificações.
- **Versão e documentos**
  - Tela e log em 0.25.0. README explica o `SPRITES.BIN`; lacunas, validação e histórico atualizados.
  - Capturas novas: fase 8-15 com o banco 4 lido do disco e o aviso sem `SPRITES.BIN`; capturas da 0.24.0 removidas.
- **Distribuição**
  - O ZIP do ELF agora leva o `SPRITES.BIN` junto. A ISO já o contém.
  - No Drive, os arquivos 0.24.0 foram substituídos pelos 0.25.0.

## Modificações completas — 0.24.0

- **Danger Above 8-1 a 8-15 (episódio completo)**
  - 15 fases na ordem do mundo 8 do `episodes.lua` (trecho em `ps2/data/episode_danger_above_page3.decompiled.lua`), com limites de estrelas do `starLimits.lua`.
  - A campanha passa de 135 para 150 fases, todas as 15 novas com roteiro vencedor.
  - Quinto banco de texturas (banco 4) para a página 8; os bancos 0 a 3 não mudaram.
- **Peças novas (definições conferidas contra o `blocks.lua`, trecho em `ps2/data/page8_definitions_excerpt.lua`)**
  - ExtraTrampoline: bola de borracha (BLOCK_SUPER_BALL), material `rubber`, restituição 5,5 própria.
  - BlockCarpet: tapete (BLOCK_CARPET), material `extras`.
  - ExtraStrongBall: bola de aço (BLOCK_STEEL_BALL), material `rock`, defesa 2000.
- **Correção de física: chão da fase inteira**
  - O chão do port ia de x = −100 a 140; peças além disso caíam no vazio (até x = 825 na 7-13, onde a fase inteira despencava e os porcos "morriam" sozinhos).
  - Agora o chão cobre ±1000 unidades (`FLOOR_HALF_WIDTH`), abrangendo todas as fases (máximo x = 825).
  - Na 8-10, uma peça em x = 163 deixa de ser destruída ao carregar.
  - Oito fases que só venciam por causa da queda receberam roteiros novos: 3-19, 4-1, 4-4, 4-11, 5-8, 6-2, 7-5 e 7-15.
- **Investigação**
  - Evidência nos scripts de que as peças começam "dormindo" no original (estado `sleeping` mantido pelo motor nativo, acordado pelo editor e posto para dormir pelo Demolisher). Explica as 15 fases com peças que se movem ao carregar; registrado como próximo passo em `docs/LACUNAS_PORT.md`.
  - Registrada a necessidade de ler os bancos de textura do disco: o ELF está com 19,7 MB com 5 bancos embutidos.
- **Testes**
  - Novo teste de procedência da página 8: ordem, banco 4, as três peças e o chão cobrindo todas as fases.
  - Testes de sprites cobrem 5 bancos (222 quadros) e as peças novas só no banco 4.
  - Tabela da campanha regenerada para 150 fases: 148 com roteiro (3-20 e 5-16 seguem sem).
- **Versão e documentos**
  - Tela e log em 0.24.0. README, lacunas, validação e histórico atualizados; capturas das fases 7-13 e 8-15 no PCSX2.
- **Limpeza**
  - Capturas da 0.23.0 removidas do repositório.
  - No Drive, o código-fonte da 0.22.0 (que tinha ficado por causa do limite de envio) e os arquivos 0.23.0 foram substituídos pelos 0.24.0.

## Modificações completas — 0.23.0

- **Danger Above 7-1 a 7-15**
  - 15 fases na ordem do mundo 7 do `episodes.lua` (trecho em `ps2/data/episode_danger_above_page2.decompiled.lua`), com limites de estrelas do `starLimits.lua`.
  - A campanha passa de 120 para 135 fases, todas as 15 novas com roteiro vencedor.
  - Usam o banco de texturas 3, compartilhado com a página 6.
- **Peças novas (definições conferidas contra o `blocks.lua`, trecho em `ps2/data/page7_definitions_excerpt.lua`)**
  - StaticFragileBlock06: bloco de terreno frágil (grupo `staticFragile`, material `immovableFragile`, 0 pontos). Novo tipo de objeto 9.
  - StaticBlock_06: bloco de terreno fixo com a máscara GROUND_BLOCK_06.
  - StaticBalloon01: balão-porco que estoura (BLOCK_PIG_BALLOON_1).
  - Material `immovableFragile` (atrito 0,8, restituição 0) no perfil.
- **Save v5**
  - Capacidade de 128 para 256 fases (1.056 bytes).
  - Saves v4 são lidos e migrados para v5. Um save de 120 fases com a 6-15 vencida libera a 7-1.
  - O teste de save cobre a migração v4 → v5, a expansão 84 → 85 e o limite de 256 posições.
- **Motor e limites**
  - Limite de objetos por fase de 192 para 224 (a 7-2 tem 200).
  - Balões e blocos de terreno frágeis compartilham a regra de estouro (`IS_FRAGILE_KIND`).
- **Texturas**
  - O banco 3 passa a usar dois quadros de dano nas peças numeradas (intacto e muito danificado), como o banco 2, para caber as páginas 6 e 7 sem criar um quinto banco. Cada banco novo custaria 2,8 MB de RAM.
  - GROUND_BLOCK_06 e BLOCK_PIG_BALLOON_1 adicionados.
- **Ferramentas de investigação**
  - `settle_probe` e `settle_detail` guardavam só 200 objetos e mostravam movimento falso na 7-2; agora usam o limite do jogo.
  - Com isso, 12 fases antigas aparecem com alguma peça se deslocando mais de 3 unidades ao carregar; registrado em `docs/LACUNAS_PORT.md`.
- **Testes**
  - Novo teste de procedência da página 7 (ordem, banco, três peças, material e limite de objetos).
  - Testes de sprites atualizados (219 quadros em 4 bancos; máscara GROUND_BLOCK_06 quase transparente como no APK).
  - Tabela da campanha regenerada para 135 fases: 133 com roteiro (3-20 e 5-16 seguem sem).
- **Versão e documentos**
  - Tela e log em 0.23.0. README (inclui o save v5), lacunas, validação e histórico atualizados; capturas das fases 7-2 e 7-3 no PCSX2.
- **Limpeza**
  - Capturas da 0.22.0 removidas do repositório.
  - No Drive, os arquivos 0.22.0 foram substituídos pelos 0.23.0.

## Modificações completas — 0.22.0

- **Pássaro verde (BoomerangBird), novo**
  - Corpo do `blocks.lua`: raio 1,5, densidade 3, atrito 0,3, restituição 0,23. Multiplicadores de dano do `BoomerangBirdDamageFactors`: madeira 3, gelo 3, pedra 1,5.
  - Algoritmo do `gamelogic.lua`, portado linha a linha:
    - antes do toque, força para cima de `flyVerticalAcceleration` (−2,8) × massa;
    - no toque, força horizontal `xVel × physicsScale × massa × (−50)` e força vertical `(2 − min(|yVel/xVel|, 2)) × physicsScale × massa × (−350)`;
    - até o primeiro contato, a força horizontal age enquanto |xVel| ≤ 50 e a vertical enquanto yVel > 5;
    - rotação do corpo como no original (até 6,28 rad/s antes do toque e 20 rad/s depois).
  - Sprites originais (normal, especial e colisão) só nos bancos de texturas das fases que usam o pássaro.
  - Novo teste `green_test`: voo mais leve antes do toque, inversão da direção limitada a 50 u/s, ativação única e bloqueio após contato.
- **Danger Above 6-1 a 6-15**
  - 15 fases na ordem do mundo 6 do `episodes.lua`, com limites de estrelas do `starLimits.lua`. A campanha passa de 105 para 120 fases.
  - As 15 têm roteiro vencedor encontrado pela busca automática, e todas assentam sem perder porcos (deslocamento máximo de 2 unidades).
- **Peças novas (definições conferidas contra o `blocks.lua`)**
  - StaticBalloon02 a 05: balões do grupo `staticFragile` (material `clouds`). Ficam fixos e estouram no primeiro impacto depois do lançamento, valendo 100 pontos (`destroyedScoreInc`). Novo tipo de objeto 8.
  - ExtraTrampoline2: trampolim fixo, material `immovableRubber`, restituição 1,1.
  - ExtraBoomerangBird: enfeite do pássaro verde. Corpo pesado (densidade 8), praticamente indestrutível (defesa 1.000.000), com atrito 0,3 e restituição 0,25 próprios.
  - O perfil passou a aceitar restituição própria por peça (`restitution_overrides`).
- **Motor e limites**
  - Limite de objetos por fase de 160 para 192 (a 6-15 tem 169).
  - Nova função `game_level_gravity`.
- **Texturas**
  - Quarto banco (banco 3) para Danger Above, com os quatro estados de dano das peças. A memória de vídeo não muda; os bancos 0, 1 e 2 seguem idênticos.
  - Corrigido o extrator de sprites: a regra do TNT só olhava o último objeto de cada fase.
- **Testes**
  - Novo teste de procedência da página 6: ordem, banco 3, pássaro verde (corpo e especialidade), balões, trampolim e enfeite conferidos contra `blocks.lua` e `gamelogic.lua`.
  - Teste de sprites cobre os 4 bancos (217 quadros).
  - O teste de materiais aceita a restituição própria por peça.
  - Tabela da campanha regenerada para 120 fases: 118 com roteiro (3-20 e 5-16 seguem sem).
- **Versão e documentos**
  - Tela e log em 0.22.0. README, lacunas, validação e histórico atualizados; capturas das fases 6-5 e 6-13 no PCSX2.
- **Limpeza**
  - Capturas da 0.21.0 removidas do repositório.
  - No Drive, os arquivos 0.21.0 foram substituídos pelos 0.22.0.

## Modificações completas — 0.21.0

- **Nova página: Mighty Hoax 5-1 a 5-21**
  - 21 fases recuperadas do APK (`LevelP2_78` a `LevelP2_90`), na ordem do mundo 5 do `episodes.lua` (trecho em `ps2/data/episode_mighty_hoax_page2.decompiled.lua`).
  - A campanha passa de 84 para 105 fases.
  - Limites de estrelas (prata/ouro) das 21 fases lidos do `starLimits.lua`. O leitor agora entende os dois formatos de atribuição que o unluac gera, e confere os 84 valores antigos.
- **Peças novas (definições do `blocks.lua`, trecho em `ps2/data/page5_definitions_excerpt.lua`)**
  - LightBlock8: círculo de gelo de 42×43 px, com 4 estados de dano.
  - ExtraBlockFlag01: bandeira, caixa do material "extras".
  - ExtraApple: maçã, círculo do material "extras" (mesmo tipo do morango).
  - Medidas dos sprites lidas das folhas originais do APK e registradas em `sprite_metrics.json`.
- **Texturas**
  - Terceiro banco (banco 2) só para a página 5, trocado ao mudar de episódio como os outros.
  - VRAM sem alteração (2,75 MiB, com 160 KiB livres).
  - No banco 2, as peças numeradas usam dois quadros de dano: intacto (estados 1 e 2) e muito danificado (estados 3 e 4). Os bancos 0 e 1 não mudaram.
- **Campanha e investigação**
  - 20 das 21 fases novas têm roteiro vencedor encontrado pela busca automática.
  - 5-16 (só pássaros pretos) segue sem vitória, como a 3-20. A cena assenta sem perder porcos; a limitação é a fórmula nativa da explosão.
  - Identificada a tabela `velocityMultiplier` por material, ainda não usada pelo port (documentada em `docs/LACUNAS_PORT.md`).
- **Saves**
  - Saves de 84 fases continuam válidos; se a 4-21 foi vencida, a 5-1 é liberada.
- **Testes**
  - Novo teste de procedência da página 5: ordem do `episodes.lua`, banco 2 e definições das três peças conferidas contra o `blocks.lua`.
  - Testes de sprites cobrem os 3 bancos (209 quadros), os quadros reaproveitados do banco 2 e as peças novas, que existem só nele.
  - Tabela da campanha regenerada para 105 fases.
- **Versão e documentos**
  - Tela e log em 0.21.0; README atualizado (inclui a descrição correta do save v4); validação, histórico e capturas das fases 5-1 e 5-16 no PCSX2.
- **Limpeza**
  - Capturas antigas (`pcsx2_0.19.0_fase_1-1.png`, `pcsx2_0.20.0_menu.png`) removidas do repositório.
  - No Drive, os arquivos 0.20.0 foram substituídos pelos 0.21.0.

## Modificações completas — 0.20.0

- **Juntas (novo)**
  - Importador lê juntas dos níveis: tipos 1 (distância), 2 (solda) e 3 (dobradiça); converte âncoras relativas (coordType 2) para coordenadas do mundo.
  - Motor cria `b2DistanceJoint`, `b2WeldJoint` e `b2RevoluteJoint`; índice −1 prende ao chão.
  - Soldas quebráveis rompem quando a força de reação passa do limite (aproximação do port); juntas somem com os corpos destruídos.
  - Até 64 juntas por fase; novo teste `joint-test`.
- **Estrelas (novo)**
  - Limites prata/ouro de `starLimits.lua` extraídos para `ps2/data/star_limits.json` (84 fases) e gravados na tabela da campanha.
  - Função `game_level_stars`: vitória = 1, prata = 2, ouro = 3.
  - Estrelas mostradas na faixa de vitória e ao lado do recorde no menu de fases; novo glifo `*` na fonte.
- **Campanha**
  - Quatro fases mudaram com as juntas (2-14, 3-6, 4-8, 4-9) e ganharam novos roteiros vencedores; continuam 83 de 84 fases vencíveis (3-20 segue sem roteiro).
  - Teste da campanha lista as fases que não vencem quando `CAMPAIGN_LIST` está definido.
- **Investigação**
  - `docs/LACUNAS_PORT.md`: cobertura dos 762 níveis do Classic, definições e pássaros faltantes, juntas, câmera, dano nativo, pontuação e música.
- **Versão e documentos**
  - Tela e log em 0.20.0; README, validação, histórico e captura do menu (`docs/pcsx2_0.20.0_menu.png`).
- **Drive**
  - Arquivos 0.19.0 (ELF, ISO, código-fonte e modificações) substituídos pelos 0.20.0.

## Modificações completas — 0.19.0

- Baixado o emulador da pasta Ferramentas pelo link fornecido; SHA-256 igual ao registrado em DEPENDENCIAS.md. PCSX2 v2.9.148 configurado em modo portátil com a BIOS scph39001, rodando sob Xvfb.
- ELF 0.18.0 conferido no PCSX2: inicia a 60 FPS, mostra as 84 fases e carrega as cenas sem erro de VRAM.
- Criados ELFs de teste (não distribuídos) que abrem direto numa fase, para conferir cenas específicas no emulador.
- Confirmado no emulador que o estilingue da 3-2 fica no alto da plataforma, como no original.
- O emulador mostrou a 4-1 desmoronando ao carregar. Uma sonda de acomodação no computador mediu o deslocamento de cada peça antes de qualquer tiro e confirmou desmoronamentos de 10 a 19 unidades em seis fases de Mighty Hoax.
- Corrigido o tamanho físico das peças conforme `ObjectCreation.lua`: caixas, círculos e polígonos dinâmicos usam 92% do sprite (`sizeFactorX/Y`, padrão 0,92); objetos estáticos (densidade 0) usam o sprite inteiro. O port usava 100%, e as peças nasciam sobrepostas.
- Corrigidos atrito e restituição conforme a tabela `materials` de blocks.lua: madeira 4,0/0,2, vidro 0,7/0,2, pedra 4,0/0,1, estático 0,8/0, porco 0,7/0,05 (0,3 nos porcos que declaram atrito próprio) e extras 0,7/0,4. O port usava 0,6/0,03 em tudo.
- O chão passou a usar o material staticGround (atrito 0,8, restituição 0); antes ficava com o padrão do Box2D (0,2).
- Atrito e restituição passaram a fazer parte dos dados de cada objeto da campanha, gerados pelo importador.
- Resultado da acomodação: fases com deslocamento acima de 3 unidades caíram de 13 para 8; a 4-1 aparece de pé no emulador, como no original.
- Conferido nos scripts que o original não coloca corpos para dormir ao carregar a fase; se o motor nativo o faz, não há como saber pelos scripts Lua.
- Refeita a busca de roteiros nas 84 fases: 76 vitórias com tiro único e 7 com busca em feixe.
- Resultado: 83 de 84 fases com vitória verificada, contra 82 na 0.18.0. A 3-9 e a 3-19, sem vitória até a 0.18.0, agora vencem.
- Continua sem roteiro a 3-20 (Level89): cinco pássaros explosivos (três pretos e dois brancos) contra nove porcos, dois deles com resistência 30. A vitória depende do modelo de explosão do port; raio, força e dano vêm dos dados, mas a queda com a distância e a conversão em impulso são aproximações.
- O auxiliar de busca em feixe passou a avaliar as sequências em paralelo e a não reprocessar sequências que já esgotaram os pássaros.
- Adicionadas as sondas `ps2/tests/settle_probe.cpp` e `settle_detail.cpp`, que medem a acomodação inicial de cada fase; não entram no ELF.
- Teste novo: tamanho físico com fator 0,92 nos objetos dinâmicos e tamanho cheio nos estáticos, e atrito e restituição de cada objeto iguais à tabela de materiais.
- Cenários de teste isolados atualizados com os materiais originais.
- Todas as pontuações de referência do teste de campanha mudaram, porque a física mudou.
- Atualizados README, histórico, validação e versão exibida 0.19.0; gerados ELF, ISO e ZIP de fontes, e o ELF final conferido no PCSX2.
- Substituídos no Drive os artefatos e relatórios da 0.18.0; mantidos APK, decompilação original e estudo de compatibilidade.

Faltam a 3-20, a segunda página de Mighty Hoax, os demais episódios, o pássaro grande, música e teste em PS2 físico.


---

# Modificações completas — PS2Birds Classic 0.18.0

- Removida a regra das três derrotas criada na 0.17.0, a pedido do usuário. O avanço volta a ser o do jogo original: só a vitória libera a fase seguinte.
- Investigadas as fases sem vitória (3-2, 3-19, 3-20, 3-21 e 4-4) comparando o importador com os scripts originais recuperados. Foram encontrados três erros presentes desde as primeiras versões do port.
- Corrigida a altura do chão. O objeto Ground de blocks.lua é uma caixa sem tamanho próprio, com centro em y=5; em todas as 84 fases, os objetos apoiados no chão têm a base em y≈0. A superfície agora fica em Ground.y − 5. Antes, o chão estava 5 unidades abaixo e todo o cenário caía e se desarranjava ao iniciar.
- Corrigida a posição do estilingue. `SlingshotSystem.setPositionByStartObject` (Slingshot.lua) coloca o estilingue sobre o primeiro pássaro da fila: x − 0,1 e y − (199 − 30)/20 + raio + 0,1, com a altura e o pivô do sprite SLING_SHOT_01_BACK. O port usava `birdZeroPos`, que só posiciona a fila de espera. Na 3-2, por exemplo, o estilingue fica no alto de uma plataforma (30,0; −30,6), e o port lançava do chão (9,7; 0,1).
- Corrigida a velocidade de lançamento. `gamelogic.lua` define `defaultForce = -925` em `updateValues` e `physicsScale = 1/20`; `launchBird` aplica impulso de −defaultForce × physicsScale × massa × esticamento, ou seja, 46,25 unidades/s com o estilingue esticado ao máximo, igual para todos os pássaros e independente da câmera. O port dividia a força pela escala da tela, de 23 a 152 unidades/s conforme a fase.
- A força passou a ser a porcentagem de esticamento (20 a 100%) e o ângulo foi ampliado de 10°–75° para −30°–85°, como o arrasto livre do estilingue original. A força e o ângulo também são limitados no momento do lançamento.
- A prévia da trajetória usa a nova velocidade física convertida para a tela.
- O enquadramento das fases foi recalculado com o chão e o estilingue corrigidos (exceto a 1-1, que mantém o enquadramento fixo).
- Conferido que as máscaras de plataforma estática são retangulares (93% a 100% de cobertura), confirmando a colisão em caixa.
- Conferido que o dano por impacto (redução de resistência) é calculado no código nativo do jogo, ausente dos scripts Lua; o cálculo de dano do port continua aproximado.
- Refeita a busca de roteiros nas 84 fases com a física corrigida: tiro único em grade de −10° a 80° e força de 30% a 100%, depois busca em feixe por pássaro (largura 3 e 5) para as que não venceram.
- Resultado: 82 das 84 fases com vitória verificada, contra 79 antes. As antes impossíveis 3-2, 3-20, 3-21 e 4-4 agora vencem.
- Continuam sem roteiro 3-9 (Level45, sete porcos e três pássaros vermelhos, vitória depende de derrubar uma torre) e 3-19 (Level22, porcos com capacete e um avô cercado por paredes). As duas dependem de dano por queda e esmagamento, a parte não recuperada do motor.
- Todas as pontuações de referência do teste de campanha mudaram, porque a física mudou para todas as fases.
- Adicionado `ps2/tests/make_campaign_table.py`, que monta a tabela do teste de campanha a partir dos resultados dos buscadores.
- Testes novos: velocidade de lançamento igual em fases com zoom diferente; chão em Ground.y − 5 e estilingue sobre o primeiro pássaro em todas as fases; ponto do estilingue e altura do chão conferidos pelo importador.
- Testes isolados (preto, branco, TNT, triângulo, amarelo) convertidos para a nova escala de força.
- Atualizados README, histórico, validação e versão exibida 0.18.0; gerados ELF, ISO e ZIP de fontes.
- Substituídos no Drive os artefatos e relatórios da 0.17.0; mantidos APK, decompilação original e estudo de compatibilidade.

Fases 3-9 e 3-19 continuam bloqueando o avanço sequencial até serem vencidas pelo jogador. Faltam a segunda página de Mighty Hoax, os demais episódios, o pássaro grande, música e testes no PCSX2 e no PS2 físico.


---

# Modificações completas — PS2Birds Classic 0.17.0

- Conferida a pasta do Drive antes de começar: nenhuma alteração desde a 0.16.0.
- Iniciado o episódio Mighty Hoax: fases 4-1 a 4-21 (primeira página), campanha ampliada de 63 para 84 fases.
- Ordem oficial recuperada de episodes.lua: LevelP2_103, 91, 65, 96, 69, 88, 64, 80, 108, 85, 82, 66, 104, 210, 83, 79, 77, 114, 81, 68 e 95; trecho incluído em `ps2/data/episode_mighty_hoax_page1.decompiled.lua`.
- Recuperados e decompilados os 21 scripts da pasta `levels/2_MightyHoax` com o unluac de referência, sem executar Lua.
- O gerador de campanha passou a aceitar pasta de origem e banco de textura por página do episódio.
- Adicionados doze blocos novos (madeira, vidro e pedra): quadrado 4×4 (9), prancha 10×1 (10), triângulo retângulo (11) e triângulo isósceles (12), com resistências recuperadas.
- Implementada colisão poligonal para os triângulos com os vértices recuperados de blocks.lua: (0,0) (1,1) (0,1) e (0,5;0) (1,1) (0,1), normalizados sobre o tamanho do sprite e centrados na origem do corpo.
- O campo de forma dos objetos passou de "círculo sim/não" para um código: caixa, círculo, triângulo retângulo e triângulo isósceles.
- Adicionados extras com sprites originais: presunto, ovo dourado e bandeira.
- Criados bancos de textura: os sprites ficam em dois bancos na memória principal (Poached Eggs e Mighty Hoax), e o banco da fase ativa é reenviado à VRAM quando o episódio muda.
- As quatro posições de página na VRAM são reservadas uma vez pelo maior tamanho entre os bancos; o uso continua em 2,75 MiB, com 160 KiB livres.
- O banco 0 é idêntico ao atlas da 0.16.0. O banco 1 contém os sprites comuns (pássaros, estilingue, ovo e máscaras de terreno), os porcos e blocos usados nas fases de Mighty Hoax e os 51 sprites novos.
- No banco 1, o piscar dos porcos e os estados de dano 2 e 3 das peças 4×4 e triangulares reutilizam quadros vizinhos (23 apelidos), para caber na VRAM. Poached Eggs não muda.
- O extrator calcula automaticamente quais sprites cada banco precisa a partir das fases geradas e rejeita sprites inexistentes.
- O ELF passou a carregar 5,5 MiB de texturas na memória principal (antes 2,75 MiB); a VRAM usada não mudou.
- Renderizador atualizado para os blocos 9 a 12, os três extras e a troca de banco no início de cada quadro.
- O gerador do ícone de salvamento lê o novo manifesto por bancos; o ícone ficou idêntico byte a byte.
- Save versão 4: 544 bytes e até 128 fases, com mesma assinatura e CRC32. Lê v1, v2, v3 e v4 e grava v4.
- Saves v3 da 0.16.0 são aceitos: vencer a 3-21 libera a 4-1, e a primeira gravação v4 vai para o outro slot, preservando a cópia v3.
- Capacidade de campanha elevada de 64 para 128 fases.
- Nova regra do port, que não existe no jogo original: três derrotas seguidas numa fase liberam a próxima, e uma vitória zera a contagem. A tela de derrota avisa quando isso acontece.
- Motivo da regra: cinco fases não têm vitória encontrada com a física aproximada do port (3-2, 3-19, 3-20, 3-21 e 4-4). Sem ela, a 3-2 bloquearia o acesso a todo o Mighty Hoax.
- Encontrados roteiros vencedores para 20 das 21 fases de Mighty Hoax: 17 com tiro único e 3 com sequências de três ou quatro tiros (4-3, 4-10 e 4-14).
- Adicionado teste isolado de triângulos: os dois tipos ficam em pé ao lado de uma caixa, apoiados no chão, e são atingidos por tiros baixos.
- Teste de campanha ampliado para 84 fases; as 63 pontuações anteriores ficaram idênticas. Inclui teste da regra das três derrotas.
- Testes de sprites reescritos para bancos: 203 sprites, 302 posicionamentos, sem sobreposição, hashes por quadro, apelidos válidos e tamanho das posições de página.
- Teste de save ampliado: layout v3 codificado de forma independente, corrupção byte a byte, migração v3→v4 e limite de 128 fases.
- Teste de procedência ampliado: ordem das 21 fases de Mighty Hoax e formas dos blocos conferidas com a fonte recuperada.
- Adicionado `ps2/data/mighty_hoax_definitions_excerpt.lua` e parâmetros dos novos blocos em `classic_actor_parameters.json`.
- Atualizados README, histórico, validação e versão exibida 0.17.0; gerados ELF, ISO e ZIP de fontes com conferência de integridade.
- Substituídos no Drive os artefatos e relatórios da 0.16.0; mantidos APK, decompilação original e estudo de compatibilidade.

O port continua parcial. Faltam a segunda página de Mighty Hoax e os demais episódios, o pássaro grande, música, refinamento de física e cenários e testes no PCSX2 e no PS2 físico.


---

# Modificações completas — PS2Birds Classic 0.16.0

- Conferida a pasta do Drive antes de começar: nenhuma alteração desde a 0.15.0.
- Expandida a campanha de 42 para 63 fases, completando o episódio Poached Eggs com a terceira página (3-1 a 3-21).
- Ordem oficial recuperada de episodes.lua: Level43, Level77, Level28, Level29, Level87, Level18, Level91, Level49, Level45, Level75, Level51, Level30, Level79, Level40, Level59, Level58, Level95, Level82, Level22, Level89 e Level81; trecho incluído em `ps2/data/episode_page3.decompiled.lua`.
- Recuperados e decompilados os 21 scripts com o unluac de referência, sem executar Lua; incluídas as fontes, JSON de fase, enquadramentos e descritores C++.
- Adicionado o rei porco (KingPiglette): raio 2,825, densidade 1, resistência 100 e defesa 0 recuperados; aparece na fase final 3-21.
- Aceita defesa 0 no importador, como na definição original do rei; o motor já aplica defesa mínima 1 no cálculo de dano.
- Adicionados com sprites originais: escada (ExtraBlockStairs), pneu médio (ExtraTire02), dado (ExtraBlockDice), ovo roubado (ExtraStolenEgg, defesa 20000), rosquinha (ExtraDonut01) e a plataforma Estrade03.
- Limite de objetos por fase elevado de 128 para 160 no importador e no núcleo; a 3-21 tem 135 objetos.
- Atlas ampliado de 144 para 152 sprites.
- Quadros do rei guardados em meia resolução, com filtro 2×2 ponderado pelo alfa, e desenhados em escala 2; ele aparece em uma única fase.
- Acrescentada uma quarta página de textura de 512×128: layout 512×512, 512×512, 512×256 e 512×128, total de 2,75 MiB.
- Orçamento de VRAM elevado de 2,5 para 2,75 MiB, mantendo 160 KiB livres dos 2,9 MiB disponíveis após os dois frame buffers de 16 bits.
- Renderizador atualizado para o rei, os novos objetos e a Estrade03.
- Corrigido defeito introduzido na 0.15.0: ao inserir o porco de bigode, o porco padrão da tabela de sprites passou a ser ele em vez do porco pequeno. O padrão agora é escolhido pelo nome.
- Adicionado `ps2/data/page3_definitions_excerpt.lua` com as definições recuperadas e parâmetros documentados em `classic_actor_parameters.json`.
- Adicionado `ps2/tests/beam.py`, busca em feixe por pássaro sobre o `solver3`, com janela de refinamento; não entra no ELF.
- Encontrados roteiros vencedores para 17 das 21 fases novas: 12 com tiro único e 5 com sequências de três ou quatro tiros.
- Não foi encontrado roteiro vencedor para 3-2 (Level77), 3-19 (Level22), 3-20 (Level89) e 3-21 (Level81), mesmo com feixe de largura 5. Elas carregam, contam objetos corretamente e não vencem sozinhas; o teste libera a fase seguinte diretamente para seguir o restante da campanha.
- Teste de campanha ampliado para 63 fases; as 42 pontuações anteriores ficaram idênticas.
- Teste de sprites ampliado: 152 quadros, layout de quatro páginas, redução do rei com valores conhecidos e margem de VRAM.
- Teste de procedência ampliado: ordem das 21 fases da terceira página e raio do rei conferidos com a fonte.
- Save v3 mantido. Saves da 0.15.0 (42 fases) são aceitos: vencer a 2-21 libera a 3-1; teste de expansão 42→63.
- Atualizados README, histórico, validação e versão exibida 0.16.0; gerados ELF, ISO e ZIP de fontes com conferência de integridade.
- Substituídos no Drive os artefatos e relatórios da 0.15.0; mantidos APK, decompilação original e estudo de compatibilidade.

O port continua parcial. Faltam os demais episódios (Mighty Hoax em diante), o pássaro grande, música, refinamento de física e cenários, validação no PCSX2 e no PS2 físico. As quatro fases sem roteiro vencedor podem ser impossíveis com a física aproximada do port e bloqueiam o avanço sequencial do jogador.


---

# Modificações completas — PS2Birds Classic 0.15.0

- Analisada a versão 0.14.0 fornecida no Drive: fontes extraídas em pasta nova, suíte completa de testes aprovada e ELF recompilado antes de qualquer alteração; repositório Git sincronizado com esse pacote.
- Expandida a campanha de 35 para 42 fases, completando a segunda página de Poached Eggs com 2-15 a 2-21 (Level66, Level85, Level27, Level32, Level72, Level90 e Level96), na ordem oficial de episodes.lua.
- Recuperados e decompilados os sete scripts de fase com o unluac de referência, sem executar Lua; incluídas as sete fontes, JSON de fase, enquadramentos e descritores C++.
- Contagens importadas (objetos/porcos/pássaros): 2-15 93/5/5, 2-16 83/3/3, 2-17 58/3/3, 2-18 66/5/4, 2-19 55/2/3, 2-20 70/4/3 e 2-21 86/5/6.
- Adicionada a caixa de TNT (ExtraBlockTNT) como novo tipo de objeto (kind 7), presente em 2-15 e 2-20.
- Implementada a explosão da TNT ao ser destruída, com os valores recuperados da definição: raio de impulso 10, força 30000, raio de dano 5 e dano 150.
- Reaproveitado o cálculo radial compartilhado do preto e do ovo, com os parâmetros próprios da TNT; estáticos continuam imunes e corpos atingidos entram na cadeia de dano.
- Implementada reação em cadeia: uma caixa destruída por outra explosão detona 0,1 s depois (aproximação do port), gerando eventos de explosão separados em vez de simultâneos.
- Caixa que apenas cai para fora do mundo é removida sem explodir; caixas em repouso nunca explodem sozinhas.
- Explosões de TNT usam o som e o efeito visual de explosão existentes, com o raio da TNT; a pontuação segue o modelo simplificado de objeto (500).
- Adicionado o porco de bigode (GrandpaPiglette): raio 2,2 e resistência 30 recuperados, sprites originais normal, piscada e dano; aparece em 2-21.
- Adicionada a madeira redonda WoodBlock8 com colisão circular conferida na definição recuperada e quatro estados de dano.
- Adicionados ExtraPillar, ExtraDiamond (circular), ExtraHolyGrail e ExtraBanana com sprites originais e propriedades importadas das fases.
- Ampliado o adaptador de fases com as sete definições novas e o kind 7; definições desconhecidas continuam rejeitadas.
- Atlas ampliado de 132 para 144 sprites sem aumentar o uso de VRAM: continuam três páginas e 2,5 MiB.
- Máscaras de terreno GROUND_BLOCK_01 a 05 passaram a ser guardadas em meia resolução, com alfa filtrado por caixa 2×2, e desenhadas em escala 2 (SPRITE_MASK_SCALE); são formas chapadas tingidas em tempo de execução.
- Eliminada a página de 1024 de largura, que existia só para a máscara de 642 pixels; as três páginas agora têm 512 de largura (512×512, 512×512 e 512×256).
- Empacotador de atlas reescrito para um layout fixo de páginas com teto de altura por página; sprite que não cabe gera erro claro em vez de criar página fora do orçamento.
- Documentado o limite real de VRAM (3.047.424 bytes livres após os dois frame buffers) e mantido o orçamento de 2.621.440 bytes já validado; a quarta página opcional do layout só entra se o orçamento for elevado.
- Renderizador atualizado para TNT, pilar, diamante, cálice, banana, madeira redonda e porco de bigode, e para a escala das máscaras de terreno.
- Regenerados atlas, manifesto, identificadores, métricas e ícone de salvamento.
- Adicionado `ps2/data/tnt_definitions_excerpt.lua` com os trechos originais das sete definições, e parâmetros documentados em `classic_actor_parameters.json`, distinguindo valores recuperados de aproximações.
- Criado teste isolado da TNT com cenário próprio: estável em repouso, explosão por impacto direto, cadeia com atraso de 12 passos, dano e eliminação de porco, limite de raio, imunidade estática, eventos únicos, pontuação, reinício, queda sem explosão e 20 trocas de mundo.
- Teste de campanha ampliado para 42 fases com roteiros determinísticos; as 35 pontuações anteriores ficaram idênticas, confirmando que as fases antigas não mudaram.
- Novas pontuações dos roteiros: 2-15 38000, 2-16 35000, 2-17 34000, 2-18 36500, 2-19 27500, 2-20 44000 e 2-21 50500.
- Adicionado `ps2/tests/solver3.cpp`, buscador em feixe com janela de refinamento, usado para a 2-19 (dois pretos lançados em arco, 68°/275 e 66°/255); não entra no ELF.
- Teste de sprites ampliado: 144 quadros, páginas de 512, redução de máscara com valores conhecidos, máscaras brancas com alfa GS e presença dos sprites novos.
- Teste de procedência ampliado: as 21 fases da segunda página conferidas contra o trecho recuperado de episodes.lua e campos de explosão da TNT conferidos contra a fonte.
- Mantido o save v3 de 288 bytes. Saves da 0.14.0 (35 fases) são aceitos: vencer a 2-14 libera a 2-15, apenas ter chegado a ela não libera; adicionado teste de expansão 35→42 com gravação e recarga.
- Atualizadas identificação 0.15.0, README, parâmetros e histórico consolidado; gerados ELF, ISO e ZIP de fontes com conferência de integridade.
- Substituídos no Drive os artefatos e relatórios da 0.14.0; mantidos APK, decompilação original e estudo de compatibilidade. Nenhum outro arquivo avulso irrelevante foi encontrado na pasta.

O port continua parcial: faltam a terceira página de Poached Eggs (Level43 a Level81), os demais episódios, o pássaro grande, música, refinamentos de física/cenários e validação em PS2 físico. Nesta rodada não houve teste no PCSX2.


---

# Modificações completas — PS2Birds Classic 0.14.0

- Expandida a campanha de 34 para 35 fases, adicionando Poached Eggs 2-14 (Level26), na ordem oficial de episodes.lua.
- Incluída a fonte Lua recuperada da fase, com 100 objetos de cenário, quatro porcos e quatro pássaros brancos. A tabela world completa tem 105 entradas contando pássaros e chão.
- Atualizados perfil, JSON, enquadramento e tabela C++ da campanha; mantidas as posições das fases anteriores.
- Adicionado BasicBird2 como quinto pássaro de fila suportado: o branco, apesar do nome BIRD_GREEN de seus sprites no APK.
- Implementados raio 1,3, densidade 4,5, atrito 0,3 e restituição 0,23 do branco, consultados nas definições recuperadas.
- Implementada habilidade com X, uma vez por lançamento e antes de contato: criação de um ovo abaixo do pássaro e recuo do pássaro para cima.
- O ovo não consome outro pássaro da fila. Pássaro e ovo são acompanhados como dois projéteis simultâneos.
- Aplicadas aproximações explícitas de queda/recuo: ovo sem velocidade horizontal inicial, queda de 35 unidades/s, recuo vertical de -25 unidades/s e velocidade horizontal do pássaro multiplicada por 1,2.
- Adicionado EggGranade como projétil circular, com densidade 3 e restituição 0,23 recuperadas; raio 1,125 inferido da largura de 45 pixels do sprite e escala de 20 pixels/unidade, pois a definição não traz raio explícito.
- Separadas as colisões entre o pássaro branco e seu próprio ovo por filtro de grupo.
- Implementada explosão do ovo no primeiro impacto com corpo ou chão. O callback apenas registra o impacto; aplicação de dano e remoção do corpo ocorrem depois do passo do Box2D.
- Usados os parâmetros da definição EggGranade: raio de impulso 10, força-base 10000, raio de dano 8 e dano-base 400. Não foram confundidos com os campos de explosão diferentes da definição BasicBird2.
- Compartilhado o cálculo radial entre ovo e preto, preservando os parâmetros próprios de cada um. Mantidas queda linear pela distância ao centro, força convertida em impulso por divisão por 120 e ausência de oclusão, aproximações do port.
- Corpos estáticos permanecem imunes ao dano/impulso de explosão. Corpos dinâmicos atingidos passam a participar da cadeia de dano.
- O ovo é removido após explodir; o branco continua em voo. Removidos ovos pendentes ao reiniciar, trocar mundo, lançar o próximo pássaro ou expirar o turno.
- Atualizadas APIs de contagem/consulta de projéteis para identificar o ovo e informar uso da habilidade; efeito visual de explosão passa a consultar o raio específico da explosão atual.
- Adicionados sprites originais do branco parado, voando e após contato/uso da habilidade, além do ovo.
- Adicionados ExtraMelon, ExtraTire01 e ExtraTire03, com sprites originais, propriedades importadas e colisões circulares simplificadas. Pneus usam discos sólidos e o modelo de materiais existente, sem reprodução da forma interna/elasticidade original.
- Ampliado o extrator de sprites para ler também folhas PNG, usadas pelo sprite DROPPABLE_EGG; os sprites anteriores continuam extraídos dos streams RGBA4444.
- Adicionados recorte validado de PNG e conversão de alfa de 0–255 para 0–128 do GS, com rejeição de orientação/extrusão não suportadas.
- Atlas ampliado de 125 para 132 sprites, mantendo três páginas: 512×512, 512×512 e 1024×128. Uso de texturas ampliado em 256 KiB, de 2,25 para 2,5 MiB, dentro do limite configurado.
- Regenerados atlas, manifesto, identificadores e ícone de salvamento; atualizadas métricas dos objetos novos.
- Adicionada dependência Pillow, limitada à extração/testes de PNG, com arquivo de requisitos e instruções. Compilar ELF/ISO com assets já convertidos não exige Pillow; executar a suíte de conversão exige.
- Adicionados sons originais do lançamento do branco e da habilidade: bird 05 flying e special group, arquivo associado ao alias special_egg no APK.
- Banco ampliado de 18 para 20 efeitos, totalizando 385712 bytes de ADPCM SPU2 mono 22050 Hz, abaixo do orçamento de 1 MiB.
- Efeitos organizados em dez canais em vez de nove; explosões usam canal próprio, separado de soltar o ovo, permitindo os dois sons simultâneos. Mantidos controle R3 e funcionamento sem áudio.
- Atualizadas renderização do branco/ovo, escolha do som de lançamento e instrução contextual X: SOLTAR OVO.
- Adicionados trechos originais das definições BasicBird2/EggGranade e parâmetros documentados, distinguindo valores recuperados e aproximações.
- Criado teste isolado do branco: ovo único, fila preservada, queda/recuo, explosão no chão e em corpo dinâmico, dano/queda por distância, imunidade estática, evento único, sobrevivência do branco, próximo pássaro, recusa após contato, descarte no timeout e 30 trocas de mundo.
- Teste do branco passou também com AddressSanitizer e UndefinedBehaviorSanitizer; detecção de vazamentos desativada pela limitação do LeakSanitizer sob ptrace no ambiente.
- Expandido teste de campanha para 35 fases; fase 2-14 concluída com três ovos no roteiro 30°/350/0,4 s, 30°/350/0,7 s e 30°/420/0,9 s, somando 37500 pontos nesse teste.
- Ampliada a janela dos roteiros auxiliares de simulação para acompanhar turnos com o ovo e o pássaro em voo. Mantida a regressão das 34 fases anteriores, incluindo o preto após compartilhar a função radial.
- Atualizados testes de atlas para 132 sprites, PNG/alfa/limites, áudio para 20 efeitos/canais separados e procedência da ordem da segunda página até 2-14.
- Mantido formato de save v3 de 288 bytes e capacidade para 64 fases. Acrescentados testes de expansão de campanha de 34 para 35, desbloqueio condicionado à vitória anterior e gravação/recarga do novo recorde final.
- Atualizadas identificação 0.14.0, instruções, histórico e validação. Gerados ELF, ISO e ZIP de fontes, com verificação de integridade e identidade do ELF dentro da ISO.
- Verificadas compilação de ELF/ISO e toda a suíte de testes a partir do ZIP extraído em pasta nova, sem depender do diretório original.
- Substituídos os três artefatos anteriores e os relatórios substituídos após conferência da publicação. Mantidos APK, decompilação original e estudo de compatibilidade, relevantes ao projeto; nenhum outro arquivo avulso irrelevante foi identificado na pasta.

O port continua parcial: faltam as fases 2-15 a 2-21, outras páginas/episódios, demais habilidades/objetos, música, refinamentos de física/cenários e validação em PS2 físico. O branco está funcional nesta versão, com aproximações documentadas de trajetória e explosão.


---

# Modificações completas — PS2Birds Classic 0.13.0

- Expandida a campanha de 32 para 34 fases, adicionando Poached Eggs 2-12 e 2-13 após as fases já disponíveis.
- Integrada a fase 2-12 (Level35), com 45 objetos de cenário, dois porcos e seis pássaros: azul/azul/amarelo/amarelo/amarelo/preto.
- Integrada a fase 2-13 (Level20), com 55 objetos de cenário, quatro porcos e quatro pássaros: amarelo/preto/preto/preto.
- Incluídas as duas fontes Lua recuperadas do APK, sem execução dos scripts; mantidas as propriedades materiais e a ordem de fila importadas.
- Atualizado o perfil da campanha e gerados os JSON, enquadramentos e tabelas C++ das 34 fases, mantendo a posição das 32 anteriores.
- Ampliada a capacidade de campanha e de recordes em memória de 32 para 64 posições.
- Ampliado o limite do gerador de campanha para até 64 fases únicas, preparando a continuação do Classic sem novo limite imediato de 32 fases.
- Criado o formato de save v3: 288 bytes, com até 64 recordes, mesma assinatura/identificador, sequência de gravação e CRC32.
- Preservada a leitura dos formatos v1 (96 bytes/16 posições) e v2 (160 bytes/32 posições), com validação específica de tamanho e capacidade de cada versão.
- Mantida a migração de ordem das fases antigas v1; saves v2 preservam as posições oficiais já usadas nas versões 0.9.0 a 0.12.0.
- Novas gravações usam v3. Recordes e desbloqueios anteriores são mesclados; vencer a última fase de uma campanha antiga libera somente a primeira fase adicionada.
- Mantida a gravação alternada: na primeira migração v2 → v3, o novo registro é escrito e relido no outro slot, preservando a cópia v2 anterior.
- Mantida a escolha do registro válido mais recente entre tamanhos/formatações diferentes; se a nova cópia estiver truncada, a cópia v2 válida pode ser recuperada.
- Mantida a proteção contra sobrescrever cabeçalhos reconhecíveis de formato/campanha mais novos, mesmo quando o outro slot é legível.
- Ampliados buffers de leitura, gravação e verificação para 320 bytes, calculados a partir do formato atual e alinhados a 64 bytes, acomodando leitura de 289 bytes sem ultrapassar a memória reservada.
- Acrescentada verificação em compilação de que as capacidades de recordes do jogo e do codec permanecem iguais.
- Atualizados os auxiliares de busca de trajetórias e testes de pássaros para usar a quantidade real de fases, sem desbloqueio fixo em 32.
- Expandido o teste sequencial da campanha para 34 fases, com contagens, bloqueios, vitórias, desbloqueios, recordes e reinício.
- Registradas trajetórias determinísticas das fases novas: 2-12 com 21000 pontos e 2-13 com 28000 pontos nos roteiros de teste; não são metas de estrelas do original.
- Acrescentadas as filas de 2-12/2-13 à regressão de habilidades mistas, verificando troca de tipo, pavio limpo, uso único, divisão azul e explosão preta.
- Atualizada a conferência de procedência para os treze primeiros nomes da segunda página oficial de episodes.lua.
- Acrescentado codificador independente de save v2 aos testes, verificando sua leitura sem depender do novo codificador de produção.
- Acrescentados testes de migração de save v2 com 32 fases para 34, preservação dos recordes inicial/final, desbloqueio da fase 33, gravação e recarga da fase 34 e preservação da cópia anterior.
- Acrescentados testes de corrupção de cada byte de um registro v2, truncamento do novo slot v3, retorno à cópia v2 e recusa de desbloquear fase nova sem vitória anterior.
- Testado o último índice do formato v3 (64º recorde) e rejeitada contagem 65; mantidos testes de CRC, limites, slots, cartão cheio, saves inválidos e futuros.
- Executados testes de save com AddressSanitizer e UndefinedBehaviorSanitizer; detecção de vazamentos desativada porque o ambiente sob ptrace não oferece suporte ao LeakSanitizer.
- Verificada migração no PCSX2 usando cópia separada do cartão de teste: registro v2 de 160 bytes convertido em v3 de 288, CRC válido, recordes/desbloqueios preservados e cópia v2 mantida no outro slot. A ISO foi reiniciada e carregou o registro v3.
- Conferidas visualmente as duas fases novas com ELF local de teste e desbloqueio temporário, excluído dos artefatos publicados.
- Mantidos os 125 sprites, três páginas de atlas (2359296 bytes), 18 efeitos sonoros (360816 bytes), quatro pássaros e temporização por relógio do PS2SDK.
- Atualizadas identificação 0.13.0, instruções, histórico e relatório de validação; gerados ELF, ISO e ZIP das fontes.
- Verificados integridade do ZIP, identidade do ELF dentro da ISO e build/testes a partir das fontes extraídas em pasta separada.
- Substituídos os três artefatos anteriores e os dois relatórios substituídos após conferência da publicação. Mantidos APK, decompilação e estudo de compatibilidade, que continuam relevantes ao projeto; nenhum outro arquivo avulso irrelevante foi identificado na pasta.

Esta versão contém 34 fases jogáveis, não 64: 64 é a capacidade preparada no jogo/save. O port continua parcial. Faltam as fases 2-14 a 2-21, outras páginas/episódios, demais pássaros e habilidades, música, maior fidelidade de cenário/física e testes em PS2 físico.


---

# Modificações completas — PS2Birds Classic 0.12.0

- Expandida a campanha de 27 para 32 fases, mantendo as 21 fases da primeira página e adicionando Poached Eggs 2-7 a 2-11.
- Integrada a fase 2-7 (Level31): 44 objetos de cenário, três porcos e quatro pássaros, na fila amarelo/amarelo/preto/azul.
- Integrada a fase 2-8 (Level21): 43 objetos, cinco porcos e três pássaros, na fila amarelo/vermelho/preto.
- Integrada a fase 2-9 (Level41): 41 objetos, um porco e quatro pássaros, na fila preto/azul/amarelo/preto.
- Integrada a fase 2-10 (Level76): 29 objetos, três porcos e três pássaros, na fila amarelo/vermelho/vermelho.
- Integrada a fase 2-11 (Level38): 94 objetos, três porcos e quatro pássaros pretos.
- Recuperadas do APK e incluídas nas fontes as cinco tabelas Lua decompiladas, sem execução de scripts do jogo.
- Atualizado o perfil da campanha com o próximo trecho da ordem oficial de episodes.lua; mantidas as posições das fases já existentes.
- Gerados JSON, enquadramentos e tabelas C++ das 32 fases a partir dos dados recuperados. A seleção existente passa a exibir os novos títulos e a página final de dois itens.
- Adicionado ExtraStrawberry (morango), com sprite original, corpo circular simplificado, propriedades materiais importadas e categoria de pontuação dos objetos extras (500 pontos).
- Adicionado LightBlock7 circular, com geometria simplificada e propriedades importadas do material de vidro.
- Corrigida WoodBlock7 de caixa para círculo, conforme blocks.lua; isso corrige a colisão da peça na fase 1-3 (Level53). Preservados os recordes anteriores.
- Incluídos trechos recuperados das definições de WoodBlock7, LightBlock7 e ExtraStrawberry e teste que confere seus tipos circulares contra o perfil e os cenários gerados.
- Adicionados os quatro sprites originais dos estados de dano de LightBlock7 e o sprite original do morango.
- Atlas ampliado de 120 para 125 sprites; mantidas três páginas, 2359296 bytes (2,25 MiB) e o limite de 2,5 MiB. Não houve aumento da memória gráfica das texturas.
- Regenerados o atlas, manifesto, identificadores de sprites e ícone de salvamento; atualizadas métricas dos dois objetos novos.
- Mantidos os quatro pássaros e suas habilidades, os 18 efeitos sonoros e o funcionamento sem áudio quando a inicialização não estiver disponível.
- Mantido o formato de save v2, de 160 bytes; a campanha agora ocupa suas 32 posições disponíveis, sem alterar o layout dos arquivos anteriores.
- Acrescentado teste de migração de save com 27 fases: recorde da 2-6 preservado, fase 2-7 desbloqueada somente quando a anterior foi vencida.
- Acrescentado teste de gravação e recarga da 32ª fase, verificando desbloqueio e recorde no último índice do formato.
- Ajustado teste de preservação de cabeçalho que declara campanha futura com 33 fases; mantidos os testes de formatos futuros, CRC, cópias alternadas e migração antiga.
- Expandido o teste de campanha para verificar contagens, bloqueios, vitórias, desbloqueio sequencial e recordes em todas as 32 fases.
- Encontrados e registrados roteiros determinísticos de conclusão das cinco fases novas, com pontuações 21000/39000/41500/19500/42000.
- Acrescentada regressão de filas mistas em três fases novas: tipo do pássaro, limpeza do pavio, habilidade única, explosão do preto, três projéteis do azul e recusa de habilidade no vermelho.
- Atualizados os testes de atlas para 125 sprites e de procedência para os onze primeiros nomes da segunda página oficial.
- Corrigido o avanço de tempo da partida: usado o relógio do PS2SDK em vez de assumir 59,94 quadros desenhados por segundo. Cenas a 30 FPS deixam de avançar a simulação à metade da velocidade somente por essa suposição.
- Mantida integração física em passos fixos de 120 Hz e limitado o tempo recuperado por quadro a 100 ms para conter atrasos extremos. O relógio é atualizado também no menu e na pausa para evitar recuperar todo o tempo pausado.
- Ajustada a velocidade de mira por segundo, preservando aproximadamente a sensibilidade anterior a 60 FPS e evitando que diminua pela metade a 30 FPS.
- Acrescentada comparação de posições do projétil e dos 94 objetos da fase 2-11 após o mesmo tempo simulado com atualizações a 30/60/120 Hz.
- Atualizadas a identificação de versão, documentação de uso, documentação do gerador e relatório de validação.
- Gerados ELF, ISO e ZIP de fontes 0.12.0; conferidos integridade do ZIP, ELF incorporado na ISO e compilação independente do pacote de fontes.
- Substituídos os três artefatos e os relatórios da versão anterior após conferência da publicação. Preservados APK, decompilação e estudo de compatibilidade, relevantes ao desenvolvimento; não havia outro arquivo avulso sem relevância na pasta.

O port continua parcial. Faltam as fases 2-12 a 2-21, outras páginas e episódios, demais pássaros/habilidades, música, cenários/colisões mais fiéis e validação em PS2 físico. O morango e os novos vidros usam as mesmas aproximações de física do port. A expansão além de 32 fases exigirá ampliar o formato de save com migração.


---

# Modificações completas — PS2Birds Classic 0.11.0

- Expandida a campanha de 21 para 27 fases: primeira página completa e fases 2-1 a 2-6 de Poached Eggs.
- Adicionadas Level52, Level34, Level42, Level24, Level88 e Level36, na ordem de episodes.lua do APK.
- Incluídos os seis arquivos Lua recuperados e um trecho independente da segunda página para conferir a procedência e a ordem.
- Perfil da campanha agora declara páginas adicionais com numeração e títulos próprios, mantendo as posições anteriores dos saves.
- Gerador valida nomes únicos e capacidade máxima de 32 fases; gera os dados das duas páginas sem apagar os JSON da segunda página.
- Substituído o leitor parcial de objetos por um leitor seguro de atribuições, tabelas, aliases de registradores e chaves calculadas; scripts Lua continuam sem execução.
- Corrigida a omissão de objetos armazenados em registradores diferentes de L1_1.
- Reimportadas todas as 27 fases. Corrigidos cenários antigos: Level7 agora 45 objetos; Level8 45; Level9 50; Level17 47; Level16 49; Level44 80.
- Recuperadas também filas de pássaros omitidas: Level9 tem quatro pássaros e Level44 oito. As contagens referem-se aos dados do APK, não a uma expansão inventada dos cenários.
- Adicionado o pássaro preto/Bomb, com fila, lançamento e representação durante voo e pavio.
- Consultados no APK raio 1, densidade 6, restituição 0,03 e fator de dano direto contra pedra 4 do preto.
- Implementada detonação manual com X, antes ou depois do contato, uma vez por pássaro.
- Implementado pavio automático de aproximadamente um segundo após o primeiro contato.
- Agendada a explosão para fora da etapa interna de contatos do Box2D, evitando destruir corpos durante callbacks.
- Implementados impulso radial até 15 unidades e dano até cinco unidades, com parâmetros-base 40000 e 300 recuperados do APK.
- Aplicadas aproximações explícitas do port: força convertida em impulso dividindo por 120, queda linear pela distância ao centro, sem oclusão e pavio de um segundo; não representam uma reprodução integral do motor original.
- Corpos estáticos ficam imunes à explosão; corpos dinâmicos atingidos passam a participar da cadeia de dano.
- Removido o projétil após explodir, preservando a sequência de pássaros e a avaliação de vitória/derrota.
- Evitado encerrar o turno por tempo enquanto o pavio está ativo; reiniciar ou trocar fase limpa o estado da explosão.
- Adicionado evento próprio de explosão e API de consulta do efeito e do pavio.
- Adicionados anel e fumaça temporários da explosão, com recorte na área de jogo.
- Adicionados sprites originais do preto, suas três etapas de pavio, madeira pequena, bola de praia e pato de borracha.
- Bola e pato usam corpos circulares simplificados e a categoria de objetos extras do port.
- Atlas ampliado de 109 para 120 sprites, mantendo três páginas e 2359296 bytes de texturas (2,25 MiB), dentro do orçamento anterior.
- Regenerado o ícone do save junto aos dados gráficos.
- Adicionados sons originais de lançamento do preto e explosão.
- Corrigido o som da divisão do azul: o alias special_group aponta para o arquivo special egg, conforme audio.lua; o arquivo special group pertence a outra habilidade.
- Banco ampliado de 16 para 18 efeitos, totalizando 360816 bytes, dentro do limite de 1 MiB de SPU2.
- Mantidos nove canais por categoria, controle R3, carregamento único e funcionamento sem áudio quando indisponível.
- Menu agora permite esquerda/direita para saltar páginas de três itens, preservando a linha e circulando nas extremidades.
- Ajustados rodapé, instruções, títulos 2-N e mensagem de detonação do preto.
- Mantido o formato v2 de 160 bytes e capacidade de 32 fases; nenhum formato novo é necessário para 27 fases.
- Acrescentados testes de expansão de saves com 21 fases, incluindo desbloqueio da 22ª quando a anterior já estava concluída.
- Atualizados testes de campanha com contagens completas e trajetórias que concluem todas as 27 fases sequencialmente.
- Acrescentado teste isolado de explosão: dano, queda por distância, impulso, imunidade estática, evento único, pavio, detonação após contato, próximo pássaro e 20 trocas de mundo.
- Acrescentados testes do importador para aliases, chaves calculadas, sete tabelas completas e rejeição de chamadas/código executável.
- Atualizados testes do atlas e das 18 amostras ADPCM, incluindo hashes, cabeçalhos, sinal decodificado e orçamento de memória.
- Atualizados parâmetros de procedência, README e relatório de validação; mantidos documentos antigos como histórico no código-fonte.
- Validada a compilação e toda a suíte novamente a partir do ZIP extraído em diretório novo.
- Gerados ELF, ISO e ZIP de fontes 0.11.0, com comparação do ELF incorporado à ISO e conferência de integridade do ZIP.
- Publicação substitui os três artefatos anteriores e os dois relatórios consolidados; preserva APK, decompilação original e estudo de compatibilidade por serem relevantes ao projeto.

O port continua parcial: faltam outras fases e episódios, demais pássaros/habilidades, música, cenários mais fiéis e validação em PS2 físico. Nesta versão, os recordes antigos são preservados mesmo quando o cenário correspondente foi corrigido.


---

# Histórico consolidado — versões anteriores à 0.10.0

Os relatórios soltos do Drive foram reunidos aqui. Os textos descrevem o estado de cada versão na época, não o estado atual.

# Modificações 0.1.1

- Adicionado suporte ao analógico esquerdo para ângulo e força.
- Implementada zona morta de 24 unidades para reduzir deriva do analógico.
- Adicionada solicitação de modo DualShock, sem esperar em laços bloqueantes.
- Implementada nova solicitação de modo analógico após desconexão.
- Mantida compatibilidade com controle digital, sem ler eixos ausentes.
- Definida prioridade do direcional quando usado junto com o analógico.
- Adicionado modo de precisão ao segurar Quadrado: 25% da velocidade de ajuste.
- Ajustado o passo de simulação para 59,94 Hz do modo NTSC utilizado.
- Atualizadas instruções na tela para analógico e precisão.
- Atualizada identificação do executável para 0.1.1.
- Gerados novo ELF, ISO e pacote completo de código-fonte.

As fases originais continuam pendentes; esta entrega aprimora a base técnica.
Compilação e testes de simulação verificados. O novo suporte analógico ainda
precisa de teste com controle físico. Não houve teste em console nesta etapa.


---

# Modificações completas — 0.2.0 em relação à 0.1.1

- Substituído o cenário de alvo único pelos dados recuperados de Poached Eggs / Level1.lua.
- Importados 19 objetos de cenário: 1 porco, 14 blocos e 4 plataformas estáticas.
- Importada a fila de três RedBirds, ordenada por startNumber.
- Preservadas posições, rotações, densidade, resistência e defesa dos objetos.
- Recuperados gravidade 20 e ponto do estilingue a partir dos dados da fase.
- Consultados os atlas do APK para dimensionar madeira, vidro e plataformas.
- Incluídas métricas dos sprites e identificação das folhas de origem em JSON.
- Usadas caixas de tamanho sprite/20 como aproximação das colisões; raio do porco 1,075 recuperado de blocks.lua.
- Adicionado importador sem execução de Lua, com rejeição de definições desconhecidas.
- Incluídos Lua de origem, hash, JSON da fase e cabeçalho C gerado para reprodução da importação.
- Migrado o núcleo da simulação de C para C++11, preservando interface C para vídeo/controle.
- Integrado Box2D 2.4.1, com código-fonte e licença MIT no pacote.
- Criados corpos dinâmicos para blocos e porco e corpos estáticos para plataformas/chão.
- Adicionados gravidade, rotação, atrito, restituição, repouso e colisões entre corpos.
- Ativada detecção contínua de colisão no pássaro lançado para reduzir atravessamento de blocos.
- Substituída a remoção simples por sobreposição por dano baseado no impulso de colisão, massa e defesa.
- Implementada remoção de corpos destruídos fora dos callbacks do solver.
- Adicionado assentamento da estrutura durante a mira; dano inicial suprimido até o lançamento e fim do período de proteção.
- Introduzidos passo fixo de 120 Hz e acumulador de tempo para a física.
- Atualizada a previsão de trajetória para o integrador discreto usado pelo Box2D em voo livre.
- Adicionados limites de voo, detecção de repouso e espera antes de encerrar a tentativa.
- Mantidos corpos da estrutura entre tentativas; reinício recria a fase completa.
- Adicionada liberação explícita do mundo físico no reinício e encerramento.
- Atualizada pontuação: 500 por bloco destruído, 5.000 por porco e 10.000 por pássaro não usado na vitória.
- Adicionada transformação de coordenadas da fase para a tela PS2.
- Implementado desenho de blocos rotacionados por faixas horizontais, com recorte na área de jogo.
- Diferenciados visualmente madeira, vidro e plataformas e adicionada variação de cor por dano.
- Ajustados o tamanho desenhado do pássaro e do porco à escala da cena.
- Reposicionado o estilingue conforme os dados importados.
- Adicionados contadores de porcos/blocos, título Poached Eggs 1-1 e mensagem de fase concluída.
- Corrigida pausa indevida durante a negociação temporária do modo DualShock; pausa por desconexão/erro real mantida.
- Mantidos analógico, zona morta, direcional, ajuste fino, pausa e reinício da versão anterior.
- Atualizado o Makefile para compilar/linkar C++ e Box2D, com dependências dos dados gerados.
- Separada a compilação dos testes próprios, com avisos tratados como erro, da biblioteca de terceiros.
- Adicionados testes de importação, assentamento, trajetória, destruição, vitória, derrota, reinício e liberação do mundo.
- Verificadas 567 combinações de tiro: 175 venceram e 215 destruíram pelo menos um bloco na simulação nativa.
- Verificados boot do ELF e da ISO no PCSX2 e vitória com 28.000 pontos por controle emulado.
- Atualizadas identificação para 0.2.0, instruções, dependências, documentação e capturas.
- Gerados novos ELF, ISO, código-fonte e hashes SHA-256; substituição dos três artefatos 0.1.1 após verificação dos uploads.

Ainda faltam sprites originais, áudio, outras fases, dano/formas de colisão fiéis,
menus, salvamento e testes em PS2 físico. Esta versão executa dados convertidos
da fase com física nova; não executa o motor ou a lógica Lua completa do Android.


---

# Modificações completas — 0.3.0 em relação à 0.2.0

- Adicionado extrator de sprites originais do APK, sem executar código ou scripts do jogo.
- Decodificados os metadados gráficos AES/LZMA e os streams de pixels 7z.
- Interpretados cabeçalhos de 40 bytes e pixels RGBA4444 little-endian.
- Validados formato, dimensões, orientação, comprimento e limites dos registros antes da conversão.
- Removida a borda de extrusão de um pixel, preservando recorte e pivô de cada sprite.
- Convertidas cores para RGBA32 e transparência para a faixa 0–128 do GS.
- Recuperados 23 sprites originais: RedBird; porco normal, piscando e ferido; frente e fundo do estilingue; plataforma; quatro estados para cada um dos três tamanhos de madeira e do bloco de vidro.
- Substituídos os desenhos procedurais desses objetos pelos recursos originais. O fundo e a interface continuam procedurais.
- Implementado empacotamento determinístico por skyline com margens transparentes entre sprites.
- Criado atlas único 512×256, com uso de 512 KiB de VRAM.
- Incluídos atlas binário, cabeçalho de regiões e manifesto com origem, pivôs, dimensões e hashes no código-fonte.
- Incorporado o atlas ao ELF com alinhamento de 128 bytes, dispensando arquivos gráficos externos.
- Adicionado módulo de renderização de sprites via gsKit e envio único da textura à VRAM na inicialização.
- Implementada verificação de falha de alocação da textura na VRAM.
- Ativada composição por transparência no GS e filtragem nearest para evitar mistura entre regiões.
- Implementados quads texturizados por listas de vértices, com rotação acompanhando os corpos do Box2D.
- Preservados pivôs e escala visual original de 20 pixels por unidade física.
- Selecionados os tamanhos e materiais de sprites correspondentes a cada bloco da fase.
- Ligados os quatro estados visuais dos blocos à resistência restante, com faixas de 75%, 50% e 25%.
- Adicionados piscar periódico e sprite ferido do porco; a animação de piscar para ao pausar.
- Implementada ordem de desenho da frente/fundo do estilingue em relação ao pássaro.
- Mantidos dados da fase, física, pontuação e controles da versão 0.2.0.
- Atualizado o Makefile para compilar o renderizador e incorporar o atlas, com dependências dos arquivos gerados.
- Adicionados testes de ordem de canais/endian, transparência GS, recorte, entrada truncada, limites, não sobreposição e hashes de cada sprite.
- Confirmados novamente os testes de física: 175 tiros vencedores e 215 tiros com destruição no conjunto de 567 combinações.
- Verificados boot do ELF/ISO e funcionamento da cena texturizada no PCSX2.
- Atualizados identificação para 0.3.0, instruções de reprodução da extração, documentação de validação e pacote de fonte.
- Gerados novos ELF, ISO e código-fonte; substituição dos três artefatos 0.2.0 após verificação dos uploads.

Ainda pendentes: fundo original, áudio, outras fases, fidelidade do dano e das formas
de colisão, execução integral de Lua, menus, salvamento e teste no PS2 físico.


---

# Modificações completas — 0.4.0 em relação à 0.3.0

- Verificados README e birdout.py fornecidos para identificar suporte declarado a Classic, Rio, Seasons, Space, Star Wars e Star Wars II.
- Criado relatório de compatibilidade distinguindo recuperação de scripts, reaproveitamento e execução de outro jogo.
- Preservada a referência original das declarações no pacote de código-fonte.
- Criado profiles/games.json com títulos, chaves, origem da declaração, estado dos testes e notas de mecânicas.
- Centralizada a decodificação AES/LZMA/7z em tools/game_profiles.py.
- Adicionado --game ao inventário/recuperação de Lua e à extração de sprites.
- Mantido Classic como perfil padrão nas ferramentas existentes.
- Adicionado check_game.py para registrar hash, layout, bibliotecas nativas, amostras recuperadas, cabeçalhos Lua e falhas, sem executar scripts nem anunciar compatibilidade de runtime.
- Adicionado recover_lua_file.py para recuperar um arquivo individual de versão compatível, inclusive de port PC.
- Impedida a substituição do arquivo de entrada pelo comando de recuperação individual.
- Corrigida a detecção de JSON não cifrado, exigindo JSON válido para não confundir bytes cifrados que começam com uma chave de abertura.
- Separados nomes e tipos de objetos/pássaros em profiles/classic_level.json.
- Adicionados --profile e --source-name ao importador de fase.
- Passado a gerar level_config.h separadamente dos dados dos objetos.
- Removidas as constantes fixas de gravidade, posição do estilingue, altura do chão, escala gráfica e enquadramento do núcleo/renderizador.
- Passado a usar dados da fase e do perfil para gravidade uniforme, chão, estilingue, escala, view e quantidade de pássaros.
- Tornada a quantidade de pássaros configurável na importação, sem exigência fixa de três.
- Adicionadas validações de valores finitos, propriedades de material, dimensões, contagem de objetos/pássaros, chão, gravidade e enquadramento.
- Mantida rejeição explícita de definições/layouts e comportamentos não suportados, antes de gerar arquivos.
- Melhorada a mensagem de recurso ausente quando o adaptador gráfico Classic não serve para o jogo/versão selecionado.
- Atualizadas dependências do Makefile para reconstruir núcleo, renderizador e testes ao alterar level_config.h.
- Adicionados testes sintéticos dos seis perfis, comparação das chaves com a referência, rejeição de chave/perfil incorretos e preservação da entrada na recuperação individual.
- Adicionados testes de configuração variável de gravidade, estilingue, enquadramento, duas aves e alias de objeto por adaptador.
- Confirmada rejeição de objeto desconhecido sem sobrescrever cabeçalho válido.
- Revalidada a recuperação dos 2.058 scripts do APK Classic, sem falhas, pelo módulo compartilhado.
- Revalidados os 23 sprites e os testes físicos do Classic; nenhum novo jogo foi anunciado como executável.
- Atualizadas identificação para 0.4.0, instruções e documentação dos limites atuais.
- Gerados novos ELF, ISO e código-fonte; substituição dos três artefatos 0.3.0 após verificação dos uploads.

Os testes com chaves de outros jogos usam entradas sintéticas. A única validação
com pacote real e execução no PCSX2 continua sendo o Classic fornecido.


---

# Modificações completas — 0.5.0 em relação à 0.4.0

- Mantido o foco exclusivo no Classic; nenhum novo jogo foi portado nesta etapa.
- Decompilados os dados Level2.lua e Level3.lua do APK Classic fornecido.
- Integradas Poached Eggs 1-2 e 1-3, além da fase 1-1 já existente.
- Adicionado gerador reproduzível de campanha com JSON de origem e cabeçalho de descritores.
- Preservados posições, rotações, propriedades físicas e filas originais de pássaros das três cenas.
- Passado a carregar a fase selecionada em runtime, sem recompilar o executável para cada fase.
- Ampliado o núcleo para múltiplos porcos e diferentes quantidades de pássaros/objetos por fase.
- Adicionado material pedra, com densidade, resistência e defesa dos dados recuperados.
- Adicionados tamanhos extras de madeira e vidro e plataformas StaticBlock_04/05.
- Ampliado o atlas de 23 para 53 sprites, incluindo os quatro estados de dano das novas peças.
- Adicionado suporte à extrusão declarada nos metadados, além da borda do stream.
- Recuperadas máscaras originais das plataformas estáticas; RGB normalizado para aplicação de cor provisória no renderizador, preservando alfa.
- Adicionado desenho texturizado com tonalidade para essas máscaras; o preenchimento original do terreno ainda não foi integrado.
- Ampliado o atlas para 512×512, ocupando 1 MiB de VRAM.
- Substituída a seleção por largura do sprite por correspondência com a definição real de cada bloco.
- Adicionado enquadramento calculado para acomodar as fases 1-2 e 1-3; fase 1-1 preserva o enquadramento anterior.
- Passado a usar gravidade, chão, origem, escala e estilingue da fase carregada.
- Ajustados os limites de voo para o enquadramento de cada cena.
- Adicionado menu inicial de seleção das três fases, com indicação de bloqueio.
- Implementado desbloqueio sequencial após a vitória.
- Adicionado X na tela de vitória para avançar; na última fase retorna à seleção.
- Adicionado Select para abrir/fechar o menu de fases durante a partida.
- Implementados recordes por fase e preservação do progresso durante reinício/troca de fase.
- Explicitado no menu que recordes/desbloqueios são apenas da sessão, sem gravação em Memory Card.
- Pausada a simulação e a animação de piscar enquanto o menu de seleção está aberto.
- Preservados pausa por Start, reinício por Triângulo e ajuste fino por Quadrado.
- Ampliada a área de instruções para mostrar também o ajuste fino.
- Adicionados títulos dinâmicos e mantidos contadores de porcos/blocos para a fase ativa.
- Mantida liberação do mundo Box2D ao trocar de fase, com validação de índices e fases bloqueadas.
- Impedida repetição do bônus de vitória em atualizações subsequentes.
- Atualizado o Makefile para descritores de campanha e testes adicionais.
- Adicionados testes de bloqueio, contagens das três cenas, vitórias, desbloqueios, recordes, reinício, bônus único e 50 trocas de mundo.
- Concluídas as três fases nos testes nativos: 28.000 / 23.000 / 18.000 pontos nos roteiros de teste.
- Mantidos os testes anteriores de física, sprites e ferramentas.
- Atualizadas versão para 0.5.0, documentação, instruções e pacotes ELF/ISO/fonte.
- Substituição dos três artefatos 0.4.0 após validação e conferência dos uploads.

Esta entrega contém somente três fases. O Classic completo ainda precisa de mais
fases/episódios, habilidades dos outros pássaros, fundo/terreno originais completos,
áudio, salvamento, menus completos e testes no hardware físico. Física e dano
continuam aproximações do port.


---

# Modificações completas — 0.6.0 sobre 0.5.0

- Salvamento de fases desbloqueadas e recordes por fase no Memory Card do slot 1.
- Leitura automática do progresso ao iniciar o jogo.
- Gravação automática após vencer uma fase.
- Comando L1 para gravar ou tentar novamente após conectar o cartão.
- Operações de leitura/gravação assíncronas, atendidas também nos menus e na pausa.
- Estado do Memory Card visível no cabeçalho e na seleção de fases.
- Aviso para não remover o cartão durante a gravação.
- Continuação da partida quando o cartão está ausente ou não formatado.
- Tratamento de cartão cheio, erro de acesso e troca detectada antes da escrita.
- Pasta própria PS2BIRDS; o jogo não formata cartões nem apaga saves de outros jogos.
- Duas cópias alternadas do progresso; a gravação seguinte usa a cópia oposta à válida mais recente.
- Recuperação pela cópia anterior quando a outra está corrompida ou incompleta.
- Validação CRC32, assinatura, versão, identificador Classic, tamanho e limites dos dados.
- Contador de sequência com tratamento de retorno ao zero.
- Releitura após gravar para conferir o conteúdo salvo.
- Preservação dos arquivos quando as duas cópias estão inválidas, com aviso ao jogador.
- Proteção contra sobrescrever registros com uma quantidade de fases maior que a suportada nesta versão.
- Combinação dos melhores recordes e desbloqueios da sessão com os lidos do cartão.
- Pedido de gravação pendente mantido quando uma operação já está em andamento.
- Ícone próprio e identificação PS2Birds Classic para o navegador de saves do PS2.
- Gerador reproduzível do modelo/ícone a partir do atlas existente, sem dependência de Pillow.
- Inclusão de libmc, módulo de persistência e recursos do ícone na compilação do ELF.
- Testes automatizados do formato e do gerenciador assíncrono com cartão simulado.
- Testes de corrupção em cada um dos 96 bytes, truncamento, CRC conhecido e sequência circular.
- Testes de cartão ausente, não formatado, cheio, trocado, gravação parcial, recuperação e preservação de saves inválidos.
- Documentação de uso, formato e validação atualizada; versão exibida alterada para 0.6.0.

O conteúdo jogável continua nas três fases iniciais de Poached Eggs. O Classic completo, áudio, demais episódios, habilidades e validação em console físico continuam pendentes. Outros jogos permanecem reservados para depois do Classic.


---

# Modificações completas — 0.7.0 sobre 0.6.0

- Recuperados e decompilados Level4.lua e Level5.lua do APK Classic fornecido, usando as ferramentas da decompilação original.
- Adicionadas Poached Eggs 1-4 e 1-5; a campanha passa de três para cinco fases jogáveis.
- Fase 1-4: 36 objetos de cenário, três porcos e quatro pássaros vermelhos.
- Fase 1-5: 26 objetos de cenário, quatro porcos e quatro pássaros vermelhos.
- Preservados os dados recuperados de posição, rotação, densidade, resistência, defesa e fila de pássaros; colisões continuam aproximadas.
- Adicionados LightBlock1 e LightBlock3, incluindo quatro estados de dano para cada tamanho.
- Adicionado StoneBlock3, incluindo os quatro estados de dano.
- Adicionadas pedras circulares StoneBlock7 e StoneBlock8, com quatro estados de dano e colisões circulares no Box2D.
- Adicionada a plataforma StaticBlock_01 com máscara original e cor provisória.
- Adicionado ExtraTreasureChest como objeto destrutível, com sprite original; usa a pontuação simplificada de objeto do protótipo (500), sem implementar eventos/recompensas especiais do original.
- Ampliado o adaptador de importação para declarar formas de colisão de caixa ou círculo, com rejeição de formas desconhecidas.
- Atualizados métricas, JSON de origem e descritores compilados para as cinco cenas.
- Calculado enquadramento para acomodar as novas fases sem câmera móvel.
- Ampliada a extração de 53 para 75 sprites originais.
- Substituído o atlas único por duas páginas (512×512 e 512×128), totalizando 1,25 MiB de texturas RGBA32 na VRAM.
- Incorporadas as duas páginas no ELF, sem arquivos externos para carregar durante o jogo.
- Atualizado o renderizador para escolher a página de cada sprite; mantidos pivôs, transparência e rotação.
- Mantido limite de 1,5 MiB para as páginas no gerador; uma expansão que ultrapasse esse orçamento é rejeitada.
- Adaptado o gerador do ícone de Memory Card ao formato com páginas.
- Seleção de fases dividida em páginas de até três linhas, com intervalo e total visíveis.
- Paginação acompanha cima/baixo, inclusive a passagem da última para a primeira fase.
- Mantidos bloqueios sequenciais, recordes e avanço por X até a nova última fase 1-5.
- Preservada leitura dos saves de três fases da 0.6.0, sem alterar o formato de 96 bytes.
- Quando o save antigo registra vitória na antiga última fase, liberada a primeira fase nova; apenas ter chegado à 1-3 não libera a 1-4.
- Gravações novas passam a declarar cinco fases, preservando recordes antigos e inicializando os novos em zero.
- Adicionada verificação de compilação contra campanhas maiores que a capacidade dos recordes/saves.
- Ampliados testes de campanha para cinco vitórias, contagens de objetos, pedras/baú, bloqueios, recordes, reinício e 50 transições.
- Ampliados testes de gráficos para as duas páginas, hashes, transparência, limites e ausência de sobreposição.
- Adicionados testes de migração do save com a antiga última fase vencida ou ainda não vencida.
- Atualizados README, documentos de validação, versão na tela, ELF, ISO e pacote de código-fonte para 0.7.0.
- Consolidado o histórico das versões 0.1.1 a 0.6.0 em HISTORICO_MODIFICACOES.md; os documentos individuais também continuam dentro do código-fonte.
- Limpeza da pasta do Drive após conferir os substitutos: exclusão dos três artefatos 0.6.0, dos seis relatórios antigos já consolidados, da prévia 0.1.0, do checksum 0.1.0 e dos dois arquivos de diagnóstico inicial superados.
- Incluídos no código-fonte os inventários e resultados do diagnóstico inicial, antes distribuídos separadamente.
- Preservados o APK original, o ZIP da decompilação e o estudo de compatibilidade útil aos futuros ports; a pasta Ferramentas não é alterada.

Ainda não é o Classic completo: faltam as demais fases/episódios, outros pássaros e habilidades, áudio, fidelidade de física/terreno/pontuação e testes em PS2 físico. Nenhum outro jogo foi portado nesta etapa.


---

# Modificações completas — 0.8.0 sobre 0.7.0

- Recuperados e decompilados Level6.lua, Level7.lua e Level8.lua do APK Classic fornecido, sem executar scripts do jogo.
- Integradas Poached Eggs 1-6, 1-7 e 1-8, ampliando a campanha de cinco para oito fases.
- 1-6: 39 objetos, cinco porcos (pequenos/médios) e quatro pássaros vermelhos.
- 1-7: 41 objetos, dois porcos (pequeno/médio) e quatro pássaros vermelhos.
- 1-8: 43 objetos, dois porcos (pequeno/médio) e cinco pássaros azuis.
- Preservados dados recuperados de posição, rotação, materiais e ordem da fila de pássaros; mantidas aproximações do motor.
- Adicionados porcos médios com raio de colisão 1,85 e sprites originais de estado normal, piscada e dano.
- Adicionados LightBlock2 com quatro estados de dano e a plataforma StaticBlock_02 com máscara original e preenchimento provisório.
- Introduzida fila tipada de pássaros nos descritores de campanha; a fila e o estilingue mostram a espécie correta.
- Implementado SmallBlueBird com raio 0,6 e densidade 4,5 consultados nos dados recuperados.
- Adicionada divisão do azul pelo segundo toque em X durante o voo, antes de sua primeira colisão.
- Mantido projétil central e criados dois laterais com abertura aproximada de ±10°, preservando a velocidade escalar no instante da divisão.
- Divisão limitada a uma ativação por lançamento, sem consumir pássaros adicionais da fila.
- Impedida divisão no vermelho, na mira, após colisão ou após uso da habilidade.
- Desabilitada colisão entre os três membros do grupo azul, evitando choque entre eles ao nascer.
- Tratados os três projéteis na detecção de fim do turno, com o limite de voo já existente.
- Desenhados os três corpos com posição e rotação próprias e sprite azul de colisão após contato.
- Mantidos visíveis os corpos do lançamento anterior durante a preparação do seguinte, até sua substituição.
- Aplicado fator 2,9 de dano direto do azul contra vidro, consultado nos dados recuperados; a fórmula geral continua aproximada e o multiplicador original de velocidade não foi reproduzido.
- Adicionado aviso contextual de X para dividir o azul durante o voo.
- Passados ângulo e força a mostrar uma casa decimal, tornando ajustes finos visíveis.
- Corrigido dano contínuo por pressão de estruturas em repouso: contatos precisam de aproximação antes do solver e ligação com o pássaro/objetos atingidos.
- Mantidas reações em cadeia entre objetos atingidos; um lançamento que não alcança a estrutura não ativa dano nela.
- Ampliado o importador para resolver atribuições literais por registradores temporários do decompilador, necessárias na 1-7, sem executar Lua.
- Adicionadas validações de tipo de pássaro e posições duplicadas na fila.
- Atualizados métricas, oito fontes Lua/JSON e descritores compilados.
- Ampliado o atlas de 75 para 85 sprites em duas páginas de 512×512 e 512×256, totalizando 1,5 MiB de texturas RGBA32.
- Mantidos os recursos incorporados no ELF e o limite de memória do gerador.
- Estendidos paginação, desbloqueios, recordes e avanço por X até a 1-8.
- Mantida compatibilidade com saves de três e cinco fases; vitória na antiga última fase libera a primeira nova, e novas gravações declaram oito fases.
- Adicionados testes de migração da 0.7.0 e preservação de saves com quantidade de fases maior que a suportada.
- Ampliados testes nativos para oito vitórias, contagens, bloqueios, recordes, reinício e 50 transições.
- Adicionados testes de lançamentos que erram as estruturas para impedir vitórias por desgaste espontâneo nas três cenas novas.
- Adicionados testes da divisão: três trajetórias, conservação de velocidade, ativação única, consumo da fila, índices inválidos, reinício e 30 trocas de mundo com os três corpos.
- Ampliados testes do importador para os registradores da 1-7 e rejeição de filas duplicadas.
- Atualizados testes de integridade, canais, transparência e limites dos 85 sprites.
- Atualizados versão, instruções, documentação, ELF, ISO e pacote de fontes para 0.8.0.
- Incorporadas as modificações 0.7.0 ao histórico consolidado, substituindo os relatórios anteriores no Drive após verificar os novos.
- Substituídos os três artefatos 0.7.0; preservados APK original, decompilação e estudo de compatibilidade, ainda relevantes. Nenhum material da pasta Ferramentas é excluído.

Continua um protótipo parcial do Classic. Faltam as demais fases/episódios, outros pássaros/habilidades, áudio, maior fidelidade de colisões/terreno/pontuação e validação em console físico.


---

# Modificações completas — 0.9.0 sobre 0.8.0

- Recuperada de `assets/data/scripts/episodes.lua` a ordem oficial da primeira página de Poached Eggs; o script foi decompilado, lido e nunca executado.
- Corrigida a numeração das fases: as versões 0.2.0 a 0.8.0 assumiam que 1-N era LevelN, mas o APK declara Level1, Level57, Level53, Level3, Level6, Level2, Level4, Level5, Level7, Level8, Level9, Level13, Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44.
- Criado `profiles/classic_episode.json` com a lista ordenada, pasta de origem e nota de procedência; `tools/build_campaign.py` passou a gerar a campanha a partir dele e grava o nome do script em cada descritor.
- Recuperados e decompilados 13 novos scripts de fase (Level57, Level53, Level9, Level13, Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44) com o unluac do ZIP de referência; a saída para Level8.lua foi comparada com a guardada e é idêntica.
- Campanha ampliada de 8 para 21 fases jogáveis, cobrindo toda a primeira página do episódio.
- Preservados posições, rotações, materiais e filas originais de pássaros das 21 cenas; as colisões continuam caixas/círculos aproximados.
- Adicionado o pássaro amarelo (YellowBird): raio 0,8, densidade 6, atrito 0,3 e restituição 0,23 consultados em blocks.lua.
- Implementada a aceleração do amarelo pelo segundo toque em X durante o voo, antes da primeira colisão: velocidade multiplicada por 1,8 (aproximação do port; a constante nativa não foi recuperada), direção e posição preservadas, uma ativação por lançamento.
- Aplicado fator 2,4 de dano direto do amarelo contra madeira, consultado em YellowBirdDamageFactors; mantido o fator 2,9 do azul contra vidro.
- Adicionados porcos grande (raio 2,325, resistência 20) e com capacete (raio 1,95, resistência 15, defesa 8) com sprites originais normal, piscada e dano.
- Adicionados StoneBlock1, StoneBlock2 e WoodBlock7 com quatro estados de dano cada.
- Adicionado ExtraBlockSmiley como objeto destrutível circular, com os dois estados originais (sorriso e dano).
- Adicionadas a plataforma StaticBlock_03 (máscara original de 642×33, cor provisória) e a Estrade01 com sprite original.
- Ampliado o adaptador de importação com o tipo 2 de pássaro e as nove definições novas; definições desconhecidas continuam rejeitadas.
- Ampliada a extração de 85 para 109 sprites originais, agora também das folhas INGAME_PIGS_NEW_SHEET_0_1 e dos quadros amarelos.
- Empacotador de atlas passou a aceitar páginas de 1024 de largura apenas quando um sprite excede 510 pixels; resultado: três páginas (512×512, 512×512 e 1024×64), 2,25 MiB de RGBA32 incorporados no ELF.
- Orçamento de texturas elevado de 1,5 MiB para 2,5 MiB; o frame buffer duplo passou de 32 para 16 bits com dithering do GS, liberando a VRAM necessária.
- Renderizador escolhe a página e os quadros corretos para amarelo, porcos grandes/capacete, sorridente, Estrade01 e StaticBlock_03; largura de página deixou de ser constante global.
- Aviso contextual de X passou a distinguir "dividir azul" de "acelerar amarelo" pelo pássaro em voo, não pelo próximo da fila.
- Corrigido o lançamento em fases cujo estilingue fica sobre uma rampa aproximada por caixa (StaticBlock_04 girado em Level15 e Level44): as plataformas estáticas que o pássaro já sobrepõe ao nascer são registradas e seus contatos ficam desativados até a separação, evitando colisão falsa, perda de velocidade e habilidade indisponível; antes disso a 1-21 não tinha vitória possível no buscador.
- Capacidade de campanha elevada de 16 para 32 fases nos descritores, no núcleo e nos recordes.
- Formato de save versão 2: 160 bytes, até 32 fases, mesma assinatura, identificador e CRC32; o jogo lê as versões 1 e 2 e grava sempre a versão 2.
- Migração de saves 0.6.0–0.8.0: recordes das antigas fases 1-1…1-8 movidos para as posições 1, 6, 4, 7, 8, 5, 9 e 10 da nova ordem; cada fase antiga vencida continua liberando a seguinte na nova ordem.
- Saves que declaram mais fases do que o executável suporta continuam preservados com aviso.
- Buffers de leitura do Memory Card ampliados para acomodar os dois tamanhos de arquivo; a verificação pós-gravação exige exatamente 160 bytes.
- Gerador de ISO ganhou alternativa em Python puro (pycdlib) quando genisoimage/mkisofs não existem; SYSTEM.CNF e nome do ELF inalterados.
- Adicionado `tools/py7z.py`, substituto do 7-Zip baseado em py7zr para os streams de sprites, deixando a reprodução da extração sem binário externo.
- Teste de campanha reescrito para as 21 fases em ordem de episódio: scripts de tiro encontrados por um buscador nativo, contagens de objetos/porcos/blocos/pássaros, bloqueios, recordes, bônus único, reinício, tiros que erram o cenário e 50 transições.
- Adicionado teste do amarelo: fila, aceleração preserva posição e direção, ativação única, indisponível após contato, vermelho rejeitado, azul continua dividindo, 30 trocas de mundo.
- Teste do azul ajustado para a nova posição de Level8 (fase 1-10).
- Teste de save reescrito: layouts v1 e v2, corrupção byte a byte dos 160 bytes, migração das três gerações de saves antigos, saves mais novos preservados.
- Teste de sprites passou a validar 109 quadros, larguras de página de 512/1024 e o orçamento de VRAM registrado no manifesto.
- Adicionados `ps2/tests/solver.cpp` e `solver2.cpp`, utilitários de host para localizar tiros vencedores; não entram no ELF.
- Atualizados `ps2/data/classic_actor_parameters.json`, métricas de sprites, 21 JSON de fase, descritores compilados, ícone do save, README, dependências e versão exibida.
- Incorporadas as modificações 0.8.0 ao histórico consolidado.
- Substituídos os três artefatos 0.8.0 no Drive por ELF, ISO e código-fonte 0.9.0; removidos da pasta os artefatos 0.7.0, o relatório 0.7.0 já consolidado e a cópia duplicada do histórico. APK, decompilação e estudo de compatibilidade preservados; a pasta Ferramentas não é alterada.

Continua um protótipo parcial do Classic. Faltam as páginas 2 e 3 de Poached Eggs e os demais episódios, pássaros preto/branco/grande, áudio, formas de colisão originais do cenário, pontuação fiel e validação em console físico.
