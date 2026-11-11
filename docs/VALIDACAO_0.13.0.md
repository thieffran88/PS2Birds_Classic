# Validação 0.13.0

- Suite completa passou: campanha de 34 fases, física, filas mistas, azul/amarelo/preto, temporização 30/60/120 Hz, save, áudio, sprites e importação/procedência.
- Roteiros de conclusão das duas fases novas: 2-12 21000 pontos; 2-13 28000 pontos. Contagens: Level35 45 objetos/2 porcos/6 pássaros; Level20 55 objetos/4 porcos/4 pássaros.
- Save v3 de 288 bytes, limite de 64 recordes; leitura v1/v2, corrupção por byte, limites, migração de 32 fases, desbloqueio condicionado à vitória anterior, último recorde v3 e cópia v2 recuperada após truncamento v3.
- AddressSanitizer/UndefinedBehaviorSanitizer passaram nos testes do codec e do acesso ao cartão simulado. LeakSanitizer desativado por incompatibilidade com ptrace neste ambiente.
- ISO de produção iniciou no PCSX2, mostrou versão 0.13.0 e 34 fases, leu save v2 do MC1 e recusou iniciar a fase final bloqueada. Menu final mostra um item (lista 34–34 de 34).
- Em cópia isolada do cartão de teste, L1 gravou v3, com confirmação na tela. Conferência do cartão bruto: CRC32 válido, registro novo de 288 bytes/34 fases, mesmos recordes/desbloqueios anteriores e slot v2 de 160 bytes preservado.
- Reiniciada a ISO com esse cartão: progresso v3 carregado, incluindo recorde anterior de 29000 na fase 1-1. O cartão original da configuração anterior foi preservado.
- Cenários 2-12/2-13 conferidos com ELF de teste local que libera fases em memória; alteração não incluída no ELF/ISO/fontes publicados. Não se afirma conclusão manual das 34 fases no emulador.
- PCSX2 sem dispositivo físico de áudio disponível: saída nula após erro CUBEB; validade das amostras/eventos verificada pelos testes, sem escuta de áudio.
- Atlas permanece com 125 sprites/2359296 bytes; banco sonoro com 18 efeitos/360816 bytes.
- ZIP extraído em diretório novo: recompilação de ELF/ISO e todos os testes passaram sem arquivos do diretório original. Restam avisos de gsKit/Box2D fornecidos.
- ZIP conferido por CRC e ELF dentro da ISO comparado byte a byte com ELF publicado. APK, BIOS e SDK não são incluídos nas fontes.
- Port parcial com aproximações de colisão, dano e habilidades. Sem testes em PS2 físico; desempenho/fidelidade no console ainda pendentes.
