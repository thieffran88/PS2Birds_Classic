# Validação 0.14.0

- Campanha de 35 fases concluída sequencialmente nos testes, com bloqueios, contagens, desbloqueios, recordes, reinício e 50 trocas de mundo. Pontuações das 34 fases anteriores mantidas nos roteiros.
- Nova 2-14: Level26, 100 objetos/4 porcos/4 brancos; três disparos com habilidade completam a fase com 37500 pontos no roteiro determinístico, não uma meta de estrelas original.
- Branco isolado: queda/recuo e posições/velocidades, uma habilidade por pássaro, fila preservada, colisão e explosão em chão e corpo dinâmico, dano por distância, estáticos imunes, evento único, branco sobrevivente, próximo pássaro e limpeza em reset/timeout/30 trocas de mundo.
- Mesmo teste passou com ASan/UBSan; detecção de vazamentos desativada por incompatibilidade do LeakSanitizer com ptrace neste ambiente.
- Atlas: 132 sprites em três páginas, 2621440 bytes; hashes, limites e ausência de sobreposição. PNG: recorte/alfa GS e rejeição de frame fora da imagem. Ovo original de PARTICLES_1.png.
- Áudio: 20 efeitos/385712 bytes/10 canais; hashes, cabeçalhos, sinal ADPCM decodificado, orçamento e canais separados para habilidade/explosão.
- Save: formato v3 preservado; expansão de 34 para 35 com desbloqueio apenas após vitória anterior, novo recorde gravado/recarregado; mantidas migração v1/v2 e proteção contra saves futuros/inválidos.
- ISO de produção iniciou no PCSX2, mostrou 0.14.0 e 35 fases, carregou progresso v3 do MC1 e recusou iniciar 2-14 bloqueada. Três páginas de textura alocadas sem erro de VRAM.
- Usado ELF local de teste com desbloqueio temporário em cópia separada da configuração/cartão para conferir 2-14. Essa alteração não está no ELF/ISO ou nas fontes publicados. Não se afirma conclusão manual das 35 fases no emulador.
- Conferidos no PCSX2 o branco no estilingue, fila, voo, recuo, ovo caindo separado do branco, explosão no impacto e eliminação de um porco, com branco ainda em voo. Pneus/melancias e cenário completos visíveis. Capturas locais mantidas fora do pacote.
- PCSX2 usa saída nula após erro CUBEB do dispositivo deste ambiente; não houve escuta de áudio. A cena oscilou durante carga e depois apresentou cerca de 60 FPS nas capturas, sem equivalência a desempenho em PS2 físico.
- ZIP extraído em pasta nova: recompilação completa de ELF/ISO e todos os testes passaram sem arquivos do diretório original. Permanecem avisos de gsKit/Box2D fornecidos.
- ZIP conferido por CRC e ELF incorporado na ISO comparado byte a byte ao ELF publicado. Fontes incluem dependências e assets convertidos, sem APK, BIOS ou SDK.
- Port parcial, com aproximações de física/colisões/habilidades. Sem testes em PS2 físico.
