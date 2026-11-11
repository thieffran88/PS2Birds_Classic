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
