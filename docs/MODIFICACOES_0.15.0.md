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
