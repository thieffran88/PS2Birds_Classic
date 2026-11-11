# Validação 0.7.0

- Compilação PS2 com PS2SDK/gsKit/libmc e GCC/G++ 15.2: ELF e ISO gerados.
- Fontes Level4/5 recuperadas do APK e decompiladas sem executar os scripts.
- JSON gerados conferidos pelos hashes SHA256 das fontes Lua.
- Testes nativos de campanha: cinco vitórias (28000/23000/18000/33000/41500), bloqueios, recordes, reinício e 50 transições de mundo aprovados.
- Fase 1-4: 36 objetos (3 porcos, 29 destrutíveis, 4 plataformas), 4 pássaros.
- Fase 1-5: 26 objetos (4 porcos, 10 destrutíveis incluindo baú, 12 plataformas), 4 pássaros; três pedras circulares.
- Roteiros novos nos testes: após 2 s de acomodação, repetir até 4 lançamentos em 20°/370 para 1-4 e 30°/370 para 1-5.
- Testes anteriores de Box2D, sprites, ferramentas/perfis e falhas de Memory Card aprovados.
- Testes novos de migração: save de 3 fases vencidas libera 1-4; save sem vitória em 1-3 continua limitado a 1-3; gravação seguinte declara 5 fases e preserva recordes.
- Atlas: 75 sprites em 512×512 + 512×128, 1310720 bytes; hashes, canais, alfa, limites e ausência de sobreposição aprovados.
- PCSX2 com a ISO: save real da 0.6.0 carregado, preservando 29000 pontos em 1-1 e desbloqueio de 1-2; nova gravação carregada após reinício.
- Jogadas no emulador: vitória em 1-2 (23500) e 1-3 (36500), vitória em 1-4 (39000), seguida de avanço e renderização de 1-5, incluindo baú e pedras.

- PCSX2: menu mostrou fases 4-5 de 5 com recorde 39000 em 1-4; cima/baixo e retorno da última para a primeira página aprovados.
- PCSX2: em 1-5, verificados rolamento das pedras, dano e eliminação de três porcos; a vitória desta fase foi validada no teste nativo, não nesta tentativa manual.
- Compilação limpa e todos os testes aprovados a partir do ZIP extraído, sem APK, SDK incorporado ou downloads adicionais.
- ELF extraído da ISO idêntico byte a byte ao ELF distribuído; integridade CRC do ZIP aprovada.

Ainda sem teste em console físico. Física e pontuação permanecem aproximadas; baú usa 500 pontos como os demais objetos destrutíveis, sem eventos especiais do original. Plataformas têm máscaras originais com preenchimento provisório. Áudio não implementado.
