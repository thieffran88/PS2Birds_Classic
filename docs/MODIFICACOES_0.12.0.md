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
