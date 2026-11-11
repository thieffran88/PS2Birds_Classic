# Validação 0.3.0 — 11/11/2026

- Compilação PS2 com GCC/G++ 15.2.0, PS2SDK e gsKit fornecidos.
- Um aviso de parâmetro não usado vem do cabeçalho gsInline.h de terceiros.
- Atlas final: 23 sprites, 512×256, RGBA32 com alfa GS 0–128; 524.288 bytes.
- Testes independentes confirmam endian little, ordem RGBA, recorte da extrusão e conversão de transparência usando pixels conhecidos.
- Testes também verificam entrada truncada, limites do atlas, ausência de sobreposição e hashes por sprite e do atlas.
- Núcleo físico preservado: testes de posições importadas, assentamento, trajetória, destruição, vitória, derrota, reinício e liberação passam; 175 combinações vencedoras e 215 com destruição entre 567 testadas.
- Boot do ELF e da ISO verificado no PCSX2 v2.9.148, com renderizador Software.
- Sprites originais e transparência/rotação inspecionados visualmente na fase.
- Tiro pela ISO: ângulo 30, força 293, vitória com 28.000 pontos.

Os sprites são originais, mas a física de dano e as caixas de colisão continuam
aproximações do port. Não há áudio ou salvamento. O teste do emulador usa saída
nula por falta de dispositivo de áudio no ambiente. Não houve teste em console
físico nem validação de desempenho nele. Fundo e interface continuam procedurais.
