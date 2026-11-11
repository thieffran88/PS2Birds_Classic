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
