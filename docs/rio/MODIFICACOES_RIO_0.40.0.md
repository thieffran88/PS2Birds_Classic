# PS2Birds Rio — modificações desta etapa (0.40.0)

Nenhum ELF, ISO ou kit do Rio foi publicado (o gerador continua recusando ISO do Rio); o Classic 0.94.1 não mudou.

## Adicionado
- **Arte das habilidades dos pássaros do Rio** (`ps2/src/main.c`, ramo do Rio, e a tabela `bird_sprites`): as habilidades que o runtime já executa agora mostram os sprites do Rio.
  - Preto: o sprite da bomba troca em três tempos (`BIRD_BLACK_SPECIAL_1..3`).
  - Branco: sprite especial ao pôr o ovo (`BIRD_WHITE_SPECIAL`) e o ovo solto (`DROPPABLE_EGG`).
  - Bumerangue: sprite especial (`BIRD_BOOMERANG_SPECIAL`) e de colisão.
  - Rosa (Stella): a bolha (`STELLA_BUBBLE`).
  - Globo: o corpo inflado (`BIRD_PUFFER_2`), na escala do tamanho de inflagem.
- `docs/rio/MODIFICACOES_RIO_0.40.0.md`.

## Editado
- `tools/rio/extract_sprites_rio.py` e `pack_sprites_rio.py`: o banco leva os sprites de habilidade (148 sprites contra 141) e a tabela de pássaros ganhou os campos de ação, os três tempos da bomba e o sprite extra; `BIRDS` inclui o ovo (tipo 5).

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.39.0.md` (e a cópia do Drive), substituído por este.

## Resultado
- O ELF do Rio compila e a fase da selva roda no emulador com a tabela nova. Os sprites de habilidade saem do APK pelos mesmos caminhos dos demais (conferi a tabela gerada, tipo por tipo).
- Não vi cada habilidade em ação na tela: no emulador sem controle, os pássaros ficam muito pequenos para conferir a olho, e a fase de teste só tem pássaros vermelhos e brancos.

## O que falta
1. Conferir cada habilidade em ação (azul que se divide, amarelo acelerado, preto explodindo, branco, bumerangue, globo, rosa) e ajustar escalas: o azul e o amarelo mostram o sprite de voo, pois o Rio não tem sprite separado para a habilidade.
2. Os 49 níveis com pássaros sem tipo (`BIRD_RIO_BLUE_ALONE`, `RioBirds`, `RocketBird`, `SUPERPOWERUP_BIRD`); macacos, folhas e frutas.
3. Paralaxe por camada e câmera vertical.
4. Vitória por gaiolas confirmada na tela, gravidade real, interface/fonte/menu, música; gerador de ponta a ponta e primeira ISO.
