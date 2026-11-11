# Validação 0.8.0

- ELF e ISO compilados para Emotion Engine com GCC/G++ 15.2, PS2SDK, gsKit e libmc.
- Fontes Level6/7/8 recuperadas do APK fornecido e decompiladas sem execução de Lua.
- Contagens importadas: 1-6 com 39 objetos/5 porcos/4 vermelhos; 1-7 com 41/2/4 vermelhos; 1-8 com 43/2/5 azuis.
- Importação de registradores da 1-7: RedBird_2 com startNumber 2, x -10.6631, densidade 6; filas duplicadas rejeitadas antes de alterar a saída.
- Campanha nativa: oito vitórias com 28000/23000/17500/44500/22000/56000/23500/31000, bloqueios, recordes, reinício e 50 transições aprovados.
- Validação adicional após 20 s de acomodação encontrou vitórias em 1-6 (10°/320), 1-7 (10°/370) e 1-8 (15°/390 com divisão a 0,7 s).
- Roteiros: acomodação de 2 s; 1-4 repetir 20°/410, 1-5 30°/380, 1-6 10°/330, 1-7 10°/280, 1-8 15°/390 com divisão a partir de 0,7 s; até esgotar a fila ou vencer.
- Box2D: varredura da 1-1 com 173 vitórias e 215 lançamentos com destruição; trajetória, derrota e reinício aprovados.
- Regressão de dano: lançamento 10°/160 que não alcança o cenário mantém todos os porcos, blocos e pontuação nas fases 1-6/7/8; não ocorre vitória automática por pressão de repouso.
- Azul: validação de fila, rejeição no vermelho/mira, três trajetórias, velocidade escalar conservada, separação espacial, ativação única e sem consumo extra da fila.
- Azul: posições finitas, encerramento do turno, remoção dos três corpos no lançamento seguinte/reinício e 30 transições de mundo aprovadas.
- Sprites: 85 quadros, hashes, canais, alfa GS 0..128, limites e ausência de sobreposição aprovados. Texturas somam 1572864 bytes: 512×512 + 512×256 RGBA32.
- Memory Card simulado: testes anteriores preservados; migração de saves com três e cinco fases e proteção de saves com nove fases aprovadas.
- PCSX2: ISO final inicia, aloca as duas páginas de textura e carrega o cartão da 0.7.0, preservando os recordes 29000/23500/36500 e o desbloqueio da 1-5.

- PCSX2: vitória em 1-5 com a física atualizada (22000 pontos) e gravação automática concluída.

- PCSX2: 1-6 concluída e avanço por X para 1-7 confirmado, com os sprites de porcos médios visíveis.
- Compilação limpa e suíte completa de testes a partir do ZIP extraído aprovadas, sem APK nem arquivos de build do diretório original.
- ELF da ISO comparado byte a byte com o ELF distribuído e CRC de todas as entradas do ZIP conferido.

- PCSX2: 1-7 concluída com 44000 pontos e gravação automática; avanço normal até a fila de cinco azuis em 1-8.
- PCSX2: divisão por segundo toque em X e eliminação do primeiro porco de 1-8 confirmadas; controle de pausa continua funcionando durante o grupo em voo.

- PCSX2: a tentativa manual de 1-8 terminou com 15000 pontos e um porco restante; a vitória desta fase foi validada no teste nativo, não nessa tentativa manual com força 390,5 e acionamento aproximado da divisão.

- PCSX2 após nova inicialização: progresso carregado, página 7-8 de 8, recorde 44000 em 1-7 e 1-8 disponível confirmados.

Sem teste em console físico. Áudio ainda não implementado. Os parâmetros de raio/densidade/fator de vidro foram consultados nos dados originais; abertura do leque, cálculo de dano, colisões de cenário e pontuação continuam aproximados. A elegibilidade de dano exige impacto ligado ao lançamento para evitar desgaste de estruturas que não foram atingidas.
