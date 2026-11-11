# Modificações completas — 0.9.0 sobre 0.8.0

- Recuperada de `assets/data/scripts/episodes.lua` a ordem oficial da primeira página de Poached Eggs; o script foi decompilado, lido e nunca executado.
- Corrigida a numeração das fases: as versões 0.2.0 a 0.8.0 assumiam que 1-N era LevelN, mas o APK declara Level1, Level57, Level53, Level3, Level6, Level2, Level4, Level5, Level7, Level8, Level9, Level13, Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44.
- Criado `profiles/classic_episode.json` com a lista ordenada, pasta de origem e nota de procedência; `tools/build_campaign.py` passou a gerar a campanha a partir dele e grava o nome do script em cada descritor.
- Recuperados e decompilados 13 novos scripts de fase (Level57, Level53, Level9, Level13, Level10, Level39, Level12, Level15, Level17, Level14, Level16, Level23 e Level44) com o unluac do ZIP de referência; a saída para Level8.lua foi comparada com a guardada e é idêntica.
- Campanha ampliada de 8 para 21 fases jogáveis, cobrindo toda a primeira página do episódio.
- Preservados posições, rotações, materiais e filas originais de pássaros das 21 cenas; as colisões continuam caixas/círculos aproximados.
- Adicionado o pássaro amarelo (YellowBird): raio 0,8, densidade 6, atrito 0,3 e restituição 0,23 consultados em blocks.lua.
- Implementada a aceleração do amarelo pelo segundo toque em X durante o voo, antes da primeira colisão: velocidade multiplicada por 1,8 (aproximação do port; a constante nativa não foi recuperada), direção e posição preservadas, uma ativação por lançamento.
- Aplicado fator 2,4 de dano direto do amarelo contra madeira, consultado em YellowBirdDamageFactors; mantido o fator 2,9 do azul contra vidro.
- Adicionados porcos grande (raio 2,325, resistência 20) e com capacete (raio 1,95, resistência 15, defesa 8) com sprites originais normal, piscada e dano.
- Adicionados StoneBlock1, StoneBlock2 e WoodBlock7 com quatro estados de dano cada.
- Adicionado ExtraBlockSmiley como objeto destrutível circular, com os dois estados originais (sorriso e dano).
- Adicionadas a plataforma StaticBlock_03 (máscara original de 642×33, cor provisória) e a Estrade01 com sprite original.
- Ampliado o adaptador de importação com o tipo 2 de pássaro e as nove definições novas; definições desconhecidas continuam rejeitadas.
- Ampliada a extração de 85 para 109 sprites originais, agora também das folhas INGAME_PIGS_NEW_SHEET_0_1 e dos quadros amarelos.
- Empacotador de atlas passou a aceitar páginas de 1024 de largura apenas quando um sprite excede 510 pixels; resultado: três páginas (512×512, 512×512 e 1024×64), 2,25 MiB de RGBA32 incorporados no ELF.
- Orçamento de texturas elevado de 1,5 MiB para 2,5 MiB; o frame buffer duplo passou de 32 para 16 bits com dithering do GS, liberando a VRAM necessária.
- Renderizador escolhe a página e os quadros corretos para amarelo, porcos grandes/capacete, sorridente, Estrade01 e StaticBlock_03; largura de página deixou de ser constante global.
- Aviso contextual de X passou a distinguir "dividir azul" de "acelerar amarelo" pelo pássaro em voo, não pelo próximo da fila.
- Corrigido o lançamento em fases cujo estilingue fica sobre uma rampa aproximada por caixa (StaticBlock_04 girado em Level15 e Level44): as plataformas estáticas que o pássaro já sobrepõe ao nascer são registradas e seus contatos ficam desativados até a separação, evitando colisão falsa, perda de velocidade e habilidade indisponível; antes disso a 1-21 não tinha vitória possível no buscador.
- Capacidade de campanha elevada de 16 para 32 fases nos descritores, no núcleo e nos recordes.
- Formato de save versão 2: 160 bytes, até 32 fases, mesma assinatura, identificador e CRC32; o jogo lê as versões 1 e 2 e grava sempre a versão 2.
- Migração de saves 0.6.0–0.8.0: recordes das antigas fases 1-1…1-8 movidos para as posições 1, 6, 4, 7, 8, 5, 9 e 10 da nova ordem; cada fase antiga vencida continua liberando a seguinte na nova ordem.
- Saves que declaram mais fases do que o executável suporta continuam preservados com aviso.
- Buffers de leitura do Memory Card ampliados para acomodar os dois tamanhos de arquivo; a verificação pós-gravação exige exatamente 160 bytes.
- Gerador de ISO ganhou alternativa em Python puro (pycdlib) quando genisoimage/mkisofs não existem; SYSTEM.CNF e nome do ELF inalterados.
- Adicionado `tools/py7z.py`, substituto do 7-Zip baseado em py7zr para os streams de sprites, deixando a reprodução da extração sem binário externo.
- Teste de campanha reescrito para as 21 fases em ordem de episódio: scripts de tiro encontrados por um buscador nativo, contagens de objetos/porcos/blocos/pássaros, bloqueios, recordes, bônus único, reinício, tiros que erram o cenário e 50 transições.
- Adicionado teste do amarelo: fila, aceleração preserva posição e direção, ativação única, indisponível após contato, vermelho rejeitado, azul continua dividindo, 30 trocas de mundo.
- Teste do azul ajustado para a nova posição de Level8 (fase 1-10).
- Teste de save reescrito: layouts v1 e v2, corrupção byte a byte dos 160 bytes, migração das três gerações de saves antigos, saves mais novos preservados.
- Teste de sprites passou a validar 109 quadros, larguras de página de 512/1024 e o orçamento de VRAM registrado no manifesto.
- Adicionados `ps2/tests/solver.cpp` e `solver2.cpp`, utilitários de host para localizar tiros vencedores; não entram no ELF.
- Atualizados `ps2/data/classic_actor_parameters.json`, métricas de sprites, 21 JSON de fase, descritores compilados, ícone do save, README, dependências e versão exibida.
- Incorporadas as modificações 0.8.0 ao histórico consolidado.
- Substituídos os três artefatos 0.8.0 no Drive por ELF, ISO e código-fonte 0.9.0; removidos da pasta os artefatos 0.7.0, o relatório 0.7.0 já consolidado e a cópia duplicada do histórico. APK, decompilação e estudo de compatibilidade preservados; a pasta Ferramentas não é alterada.

Continua um protótipo parcial do Classic. Faltam as páginas 2 e 3 de Poached Eggs e os demais episódios, pássaros preto/branco/grande, áudio, formas de colisão originais do cenário, pontuação fiel e validação em console físico.
