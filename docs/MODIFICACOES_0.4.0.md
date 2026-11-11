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
