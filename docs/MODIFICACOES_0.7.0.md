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
