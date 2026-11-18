# PS2Birds Rio — modificações desta etapa (0.39.0)

Nenhum ELF, ISO ou kit do Rio foi publicado (o gerador continua recusando ISO do Rio); o Classic 0.94.1 não mudou (a rolagem do fundo só atua no ELF do Rio, onde a câmera se move).

## Adicionado
- **Rolagem do fundo** (`background_set_scroll` em `ps2/src/background.c`/`.h`, chamada em `main.c`): o cenário do tema se desloca a 35% da rolagem da câmera, com volta circular nos 640 pixels, então o fundo deixa de ficar parado enquanto a fase passa. Fora do jogo (menus) e nas fases de câmera fixa o deslocamento é zero.
- `docs/rio/MODIFICACOES_RIO_0.39.0.md`.

## Editado
- `ps2/src/background.c`: a imagem do fundo é refeita também quando o deslocamento muda (cópia das 448 linhas, em duas partes por linha).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.38.0.md` (e a cópia do Drive), substituído por este.

## Resultado no emulador
- Na primeira fase da selva o cenário acompanha a câmera em velocidade menor que a da fase; sem emenda visível. O disparo foi simulado por um ELF de teste fora do repositório.
- A velocidade (35%) é uma escolha minha; o original separa as camadas do tema em velocidades diferentes (paralaxe por camada), o que não foi feito.

## O que falta
1. Paralaxe por camada (cada camada do tema com sua velocidade) e câmera vertical.
2. Habilidades dos pássaros com a arte do Rio (ovo do branco, bumerangue, explosão do preto).
3. Os 49 níveis com pássaros sem tipo; macacos, folhas e frutas.
4. Vitória por gaiolas confirmada na tela, gravidade real, interface/fonte/menu, música; gerador de ponta a ponta e primeira ISO.
