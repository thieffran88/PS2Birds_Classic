# PS2Birds Rio — modificações desta etapa (0.38.0)

Nenhum ELF, ISO ou kit do Rio foi publicado (o gerador continua recusando ISO do Rio); o Classic 0.94.1 não mudou (a câmera só existe no ELF do Rio).

## Adicionado
- **Câmera que acompanha o pássaro** (`game_camera` em `ps2/src/game.cpp`, ligada em `main.c`): nas fases do Rio mais largas que a tela, o jogo rola para a direita seguindo o pássaro lançado e volta ao estilingue quando o próximo pássaro entra. A rolagem é suave e não passa da borda direita da fase. Só o ELF do Rio usa; as fases do Classic (inclusive Egg Defender) continuam com a câmera fixa.
- `docs/rio/MODIFICACOES_RIO_0.38.0.md`.

## Editado
- **Zoom das fases largas** (`tools/rio/rio_campaign.py`): quando a fase não cabe na tela no zoom da câmera original (câmera do iPhone, `castleCameraData`), a fase usa esse zoom em vez de encolher tudo; a altura continua cabendo. As fases que cabem seguem a regra do Classic.
- `ps2/src/game.h` (campos da câmera) e `ps2/src/main.c`.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.37.0.md` (e a cópia do Drive), substituído por este.

## Resultado no emulador
- Na primeira fase da selva a fase aparece maior, e a tela acompanha o pássaro e volta ao estilingue. O disparo foi simulado por um ELF de teste fora do repositório.
- O fundo (tema) ainda não rola com a câmera.

## O que falta
1. Rolagem do fundo (paralaxe) e câmera vertical; a câmera do original também mostra a estrutura antes do lançamento.
2. Habilidades dos pássaros com a arte do Rio (ovo do branco, bumerangue, explosão do preto).
3. Os 49 níveis com pássaros sem tipo; macacos, folhas e frutas.
4. Vitória por gaiolas confirmada na tela, gravidade real, interface/fonte/menu, música; gerador de ponta a ponta e primeira ISO.
