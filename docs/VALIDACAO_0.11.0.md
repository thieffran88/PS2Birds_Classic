# Validação 0.11.0

- Suite nativa completa: física, campanha, save, áudio, atlas e perfis passou com compilação sob -Wall/-Wextra/-Werror nos testes pertinentes.
- Conclusão sequencial das 27 fases, com bloqueios e desbloqueios, recordes de sessão, reinício, regressão de disparo que não alcança o cenário e 50 trocas de mundo.
- Pontuações do roteiro determinístico: 27000/41500/30500/7000/56000/15000/44500/22000/44000/26500/33000/45500/49500/65500/33500/51000/38000/29000/15000/38500/36500/56500/60500/95000/27500/26000/27500.
- Teste isolado do preto: solicitação manual enfileirada, evento único, dano com queda por distância, impulso além do raio de dano, imunidade estática, pavio automático, ação após contato, próximo pássaro e 20 substituições de mundo.
- Leitor sem execução de Lua: aliases e chaves calculadas, rejeição de chamadas, conferência de sete tabelas completas anteriormente truncadas e ordem oficial das páginas.
- Save: CRC, formatos v1/v2, slots alternados, cartão cheio/ausente, preservação de dados inválidos/futuros, migração antiga e extensão de 21 para 27 fases.
- Gráficos: 120 sprites em três páginas, 2359296 bytes; áudio: 18 efeitos, 360816 bytes, cabeçalhos/alinhamento/hashes e sinal ADPCM decodificado verificados.
- ISO de produção iniciou no PCSX2 com a identificação 0.11.0, progresso do MC1 carregado, menu de 27 itens, navegação por páginas e recusa de iniciar fase bloqueada.
- Para conferir visualmente as fases 2-5/2-6 sem completar manualmente 21 fases, foi usado um ELF de teste local com desbloqueio temporário e uma cópia separada da configuração/cartão. Essa alteração não entra no ELF/ISO nem nas fontes publicadas. O teste visual não equivale a completar a campanha manualmente no emulador.
- O ELF local de teste mostrou o preto no estilingue e em voo, explosão manual com anel/fumaça, manutenção dos próximos pássaros e os cenários novos. A simulação automatizada usa o mesmo núcleo de produção.
- O PCSX2 deste ambiente falha ao abrir o dispositivo físico de áudio (CUBEB_ERROR) e usa saída nula. A validade das amostras e eventos foi testada nativamente; não houve escuta de áudio nesta versão.
- ZIP extraído em diretório novo: recompilação completa de ELF e ISO e toda a suíte de testes passaram sem depender de arquivos do diretório original. Avisos restantes são de gsKit/Box2D fornecidos.
- Conferidos integridade do ZIP e conteúdo do ELF dentro da ISO. APK, ferramentas de SDK e BIOS não fazem parte do ZIP de fontes.
- Nenhum teste em PS2 físico foi realizado. Desempenho/fidelidade no console continuam pendentes. Físicas, contornos, temporização e propagação de explosão são aproximações documentadas.
