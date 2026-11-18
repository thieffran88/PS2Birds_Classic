# Modificações — 0.94.5

Versão de **correções do som e das peças do Rio**, a partir do vídeo enviado. O jogo Classic só ganha a correção do som.

## Adicionado
- **Simulação de estabilidade das fases do Rio** (feita fora do jogo, com o mesmo motor de física): as 45 fases rodam 30 segundos sem nenhum tiro e contam as peças que se mexem. Serviu para achar a causa das quedas.
- `docs/MODIFICACOES_0.94.5.md`, `docs/VALIDACAO_0.94.5.md` e `docs/rio/MODIFICACOES_RIO_0.44.0.md`.

## Editado
- **Chiado/zumbido contínuo (todos os jogos)** (`ps2/src/music.c`): o vídeo mostra, a partir de 8 s (depois do som de início de fase), um zumbido de timbre constante que repete a cada 0,32 s até o fim. Era o anel de áudio da música repetindo dados antigos depois que a música parava. Agora o anel é esvaziado quando uma música em andamento é parada de propósito (a fase do Rio não tem música: parava a música do menu) e, nas músicas que terminam sozinhas, assim que o anel termina de tocar. O teste da música (`make test`) cobre os dois casos.
- **Peças do Rio começam dormindo** (`ps2/Makefile`, só no ELF do Rio): as fases foram salvas em repouso; com as peças acordadas, até a primeira fase se mexia sozinha (as gaiolas do topo da torre escorregavam depois de 4 s) em todas as 45 fases. Com as peças dormindo, as 45 fases ficam paradas até o primeiro toque, e a primeira fase mostra as 5 gaiolas no topo da torre, como no desenho original.
- Versão 0.94.5 e README.

## Excluído
- Da pasta do Drive e do repositório: documentos, código-fonte, ISO, ELF e kits da 0.94.4 (substituídos) e a documentação 0.43.0 do Rio.

## Observações
- O som é confirmado só pela análise do vídeo (espectrograma e repetição); não ouvi o resultado depois da correção.
- Ficam pendentes: mira, gravidade, acentos e a interface do Rio.
