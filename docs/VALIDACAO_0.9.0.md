# Validação 0.9.0

- ELF e ISO compilados para Emotion Engine com GCC/G++ 15.2 (ps2dev da release oficial, mesmo SHA-256 do pacote em Ferramentas), PS2SDK, gsKit, dmaKit, libpad e libmc; compilação limpa a partir de `make clean`.
- APK Classic conferido: SHA-256 `8cdea1ef…24979b8`, igual ao diagnóstico inicial; 2.057 scripts Lua recuperados sem executar nada (options.lua continua sendo o único payload 7z recusado, como antes).
- `episodes.lua` decompilado: a primeira página de Poached Eggs declara Level1, Level57, Level53, Level3, Level6, Level2, Level4, Level5, Level7, Level8, Level9, Level13, Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44, nessa ordem.
- Decompilação com unluac do ZIP de referência: Level8.lua gera texto idêntico ao arquivo já guardado; os 13 scripts novos importaram sem definições desconhecidas.
- Contagens importadas (objetos/porcos/pássaros): 1-1 19/1/3, 1-2 27/4/5, 1-3 15/2/4, 1-4 20/1/4, 1-5 39/5/4, 1-6 24/2/4, 1-7 36/3/4, 1-8 26/4/4, 1-9 41/2/4, 1-10 43/2/5, 1-11 43/3/3, 1-12 41/3/4, 1-13 39/3/4, 1-14 27/9/4, 1-15 44/3/4, 1-16 39/4/5, 1-17 44/2/3, 1-18 32/4/5, 1-19 42/1/3, 1-20 36/2/5, 1-21 40/2/3.
- Campanha nativa: 21 vitórias em ordem com 27000/41500/30500/7000/56000/15000/44500/22000/23500/16000/26000/45500/49500/65500/33500/51000/32500/29000/14000/38500/19000 pontos; bloqueios, desbloqueio sequencial, recordes, bônus único, reinício e 50 transições aprovados.
- Roteiros de tiro (ângulo/força, repetidos por pássaro, acomodação de 2 s): 10/350, 10/240, 10/290, 10/300, 10/330, 10/360, 20/410, 30/380, 10/280, 20/380, 25/270, 10/350, 10/410, 25/390, 10/420, 10/290, 10/410, 10/300 com aceleração a 0,4 s, 10/300, 10/280 e 10/350; encontrados pelo buscador `ps2/tests/solver.cpp`.
- Regressão de sobreposição no lançamento: em Level15 e Level44 o pássaro nasce sobreposto a um StaticBlock_04 girado; antes da correção a primeira colisão era marcada no quadro zero e a 1-21 não tinha vitória possível. Com os contatos de nascimento desativados, o amarelo mantém a habilidade e a 1-21 vence com 19000 pontos.
- Regressão de dano por repouso: tiro 10°/160 que não alcança o cenário mantém porcos, blocos e pontuação nas fases 1-1, 1-5, 1-9, 1-13, 1-17 e 1-21.
- Box2D: varredura da 1-1 com 173 vitórias e 215 lançamentos com destruição; trajetória, derrota e reinício aprovados.
- Azul: validação anterior preservada na nova posição (1-10, Level8).
- Amarelo: fila da 1-16, aceleração ×1,8 preserva posição e direção, ativação única, indisponível após contato, vermelho não acelera, azul continua dividindo, 30 trocas de mundo.
- Sprites: 109 quadros, hashes, canais, alfa GS 0..128, limites e ausência de sobreposição aprovados. Páginas 512×512 + 512×512 + 1024×64 RGBA32, 2.359.296 bytes dentro do orçamento de 2.621.440.
- Memory Card simulado: layouts v1 (96 bytes) e v2 (160 bytes), corrupção em cada um dos 160 bytes, truncamento, escolha de slot, cartão cheio, saves inválidos preservados, migração de saves com 3, 5 e 8 fases para a nova ordem (recordes nas posições 1/6/4/7/8/5/9/10 e desbloqueios 6/9/11/10 conforme o caso) e save de versão mais nova preservado.
- ISO gerada com pycdlib: entradas PS2BIRDS.ELF;1 e SYSTEM.CNF;1, volume PS2BIRDS_CLASSIC, ELF extraído idêntico byte a byte ao distribuído.
- Perfis: 6 chaves declaradas, round trips sintéticos AES/LZMA, rejeição de chave/perfil incorretos e de adaptador não suportado.

Não houve teste no PCSX2 nesta rodada: o emulador e a BIOS da pasta Ferramentas não estavam acessíveis no ambiente de compilação. Também sem teste em console físico e sem áudio. Fator de aceleração do amarelo, abertura do leque do azul, fórmula de dano, pontuação e formas de colisão do cenário continuam aproximações.
