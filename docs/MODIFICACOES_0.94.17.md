# Modificações — 0.94.17 (Classic) e 0.49.7 (Rio)

## Editado
- **Borda de grama no topo do terreno** (`ps2/src/background.c`): ao pintar uma peça de terreno no fundo da fase, as primeiras 5 a 8 linhas de cada coluna, a partir da borda de cima, ganham a cor da linha do chão do tema (verde nos temas com grama; terra, areia ou neve nos outros). O original desenha essa franja pelo código nativo; aqui ela é uma aproximação feita com a própria cor do chão do tema, com as duas primeiras linhas mais claras.
- **Textura nas peças de terreno que quebram** (`ps2/src/main.c`): as peças `StaticFragileBlock06` (424 nas fases) eram desenhadas com uma cor lisa; agora recebem a textura do tema e a borda como o terreno fixo. Como podem quebrar, o fundo é pintado de novo quando uma some (`fragile_alive` entra na assinatura da fase).
- Capturas na pasta `Capturas` do Drive (antes e depois).

## Adicionado
- `docs/MODIFICACOES_0.94.17.md` e `docs/rio/MODIFICACOES_RIO_0.49.7.md`.

## Excluído
- Notas da versão anterior.

## Aproximado / não validado
- A franja de grama real do original não foi recuperada (nenhum sprite próprio nos APK: é código nativo); a largura (5 a 8 pixels) e o clareado são escolhas minhas. Visto só no emulador.
