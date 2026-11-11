# Validação 0.12.0

- Suite nativa: física Box2D, campanha de 32 fases, quatro pássaros, explosões, áudio, save, importação e atlas passou.
- Conclusão sequencial, bloqueios/desbloqueios, contagens de objetos e pássaros, recordes, reinício, regressão de disparo que não chega ao cenário e 50 trocas de mundo.
- Pontuações novas: 2-7 21000; 2-8 39000; 2-9 41500; 2-10 19500; 2-11 42000. São resultados do roteiro de teste, não metas de estrelas do jogo original.
- Testadas filas mistas de 2-7/2-8/2-9: mudança de tipo, limpeza de pavio/estado da habilidade, ativação única, três projéteis azuis, explosão preta e vermelho sem habilidade.
- Save de 27 fases com última fase vencida migra para desbloqueio 28, preservando pontuação. Sem vitória final, permanece em 27. Gravação/recarga preserva recorde da fase 32.
- Mantidos os testes de CRC, v1/v2, slots alternados, falhas de cartão, dados inválidos e preservação de cabeçalhos de versões/campanhas futuras.
- Corrigidas/conferidas as colisões circulares de WoodBlock7 e LightBlock7 contra trechos originais de blocks.lua. Fase 1-3 novamente concluída no teste com 40000 pontos.
- Atlas: 125 frames, três páginas, 2359296 bytes, limites e hashes conferidos. Banco sonoro permanece com 18 efeitos/360816 bytes; decodificação ADPCM validada nos testes.
- ISO de produção iniciou no PCSX2 com versão 0.12.0, carregou o progresso anterior do MC1, mostrou as 32 fases e a página final com apenas dois itens, e recusou iniciar a 2-11 bloqueada.
- Inspeção das fases novas feita também com ELF local de teste com desbloqueio temporário e cópia independente da configuração/cartão do emulador. A alteração de desbloqueio não está no ELF/ISO ou nas fontes publicadas. Não se afirma conclusão manual da campanha inteira no emulador.
- Conferidos visualmente os cenários de 2-10 e 2-11, incluindo morango, patos, vidros e pretos; a 2-11 tem 94 objetos e é uma das cenas mais pesadas do port.
- PCSX2 usa saída nula após erro do dispositivo CUBEB deste ambiente. Amostras/eventos foram testados, sem escuta de áudio nesta versão.
- ZIP extraído em diretório separado: recompilação completa de ELF/ISO e toda a suíte passaram sem depender do diretório original. Permanecem avisos de gsKit/Box2D fornecidos.
- Integridade do ZIP e igualdade byte a byte entre ELF publicado e ELF incorporado na ISO conferidas. Fontes incluem dependências e assets convertidos; não incluem APK/BIOS/SDK.
- Na fase 2-11 o PCSX2 apresentou cerca de 30 quadros desenhados por segundo, com 60 VPS/velocidade de emulação próxima de 100%; a fase 2-10 apresentou cerca de 60 FPS. Isso não mede desempenho em console físico.
- Validada a temporização por relógio do PS2SDK no emulador: na fase 2-11, segurar direita por aproximadamente um segundo elevou força de 260 para cerca de 354,5, compatível com cerca de 90 unidades por segundo. Teste nativo comparou posições a 30/60/120 Hz após o mesmo tempo, com tolerância de 0,01 pixel.
- O port permanece parcial, com colisões e habilidades aproximadas. Não houve validação em PS2 físico; desempenho e fidelidade no console continuam pendentes.
