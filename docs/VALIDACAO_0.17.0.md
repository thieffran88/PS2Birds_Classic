# Validação 0.17.0

- Campanha de 84 fases percorrida nos testes nativos; as 63 pontuações anteriores são idênticas às da 0.16.0.
- Mighty Hoax: 20 vitórias com 25000/54500/27000/62500/37500/40000/37000/18500/26000/49000/46000/33000/84500/45500/46500/10500/30000/38000/61500/70500 pontos (4-1 a 4-3 e 4-5 a 4-21).
- Sem roteiro vencedor: 4-4 (LevelP2_96, 79 objetos, 4 porcos); o melhor feixe causou só 4500 pontos de dano. Seguem sem roteiro 3-2, 3-19, 3-20 e 3-21.
- Regra das três derrotas: a próxima fase é liberada só na terceira derrota seguida, a contagem sobrevive à troca de fase e uma vitória a zera.
- Triângulos: os dois tipos de casco ficam em pé, com base no chão, e recebem colisão de pássaros.
- Bancos de textura: 203 sprites, 152 no banco 0 e 150 no banco 1, mais 23 apelidos; posições de página de 1 MiB, 1 MiB, 512 KiB e 256 KiB, total de 2.883.584 bytes; hashes, limites e ausência de sobreposição conferidos.
- Ícone de salvamento idêntico ao da 0.16.0.
- Save: v3 lido e migrado para v4 no outro slot; limites de 128 fases; migrações anteriores mantidas.
- TNT, preto, branco, azul, amarelo, áudio, Box2D e perfis aprovados sem alteração.
- ELF e ISO compilados a partir de `make clean`; ELF dentro da ISO idêntico ao distribuído; ZIP de fontes recompilado e testado em pasta nova.

Sem execução no PCSX2; o emulador não está acessível neste ambiente. Ficam sem conferência em execução a troca de banco de textura na VRAM, os triângulos e os novos sprites. Sem teste em PS2 físico.
