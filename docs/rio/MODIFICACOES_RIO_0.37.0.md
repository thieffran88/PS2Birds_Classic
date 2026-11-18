# PS2Birds Rio — modificações desta etapa (0.37.0)

Nenhum ELF, ISO ou kit do Rio foi publicado (o gerador continua recusando ISO do Rio); o Classic 0.94.1 não mudou.

## Adicionado
- **Pássaro em voo:** o pássaro que espera no estilingue ou na fila usa o sprite normal; o lançado usa o sprite de voo e, ao bater, o de colisão (`ps2/src/main.c`, ramo do Rio).
- `docs/rio/MODIFICACOES_RIO_0.37.0.md`.

## Editado
- **Círculos e retângulos brancos da selva:** vinham dos blocos estáticos e dummy (`BLOCK_STATIC_*`, `BLOCK_DUMMY_STATIC`), cujo sprite é um marcador branco; esses blocos só têm física (o cenário é do tema) e agora não são desenhados nem entram no banco (`tools/rio/extract_sprites_rio.py`, `pack_sprites_rio.py`). O banco da selva passou de 141 sprites sem os marcadores.
- Tamanho do pássaro pela razão do raio, como no Classic.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.36.0.md` (e a cópia do Drive), substituído por este.

## Resultado no emulador
- Na primeira fase da selva e no galpão, os pássaros aparecem com a arte do Rio (vermelho com rosto, azul pequeno, amarelo, branco) e os marcadores brancos sumiram. O disparo foi simulado por um ELF de teste fora do repositório.
- A fase da selva é muito larga (cerca de 190 unidades): como o runtime mostra a fase inteira sem rolar, tudo fica pequeno. O original usa uma câmera que acompanha o pássaro.

## O que falta
1. Câmera que acompanha o pássaro (rolagem) para as fases largas.
2. Pássaros por partes e habilidades: bico/rosto trocando de estado, o ovo do branco, o bumerangue, a explosão do preto etc., com a arte do Rio.
3. Os 49 níveis com pássaros sem tipo (`BIRD_RIO_BLUE_ALONE`, `RioBirds`, `RocketBird`, `SUPERPOWERUP_BIRD`); macacos, folhas e frutas.
4. Vitória por gaiolas confirmada na tela, gravidade real, interface/fonte/menu, música; gerador de ponta a ponta e primeira ISO.
