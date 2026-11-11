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
