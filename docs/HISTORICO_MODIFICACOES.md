# Histórico consolidado — versões anteriores à 0.8.0

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
