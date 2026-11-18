# PS2Birds Rio — modificações desta etapa (0.35.0)

Nenhum ELF, ISO ou kit do Rio foi publicado: o gerador continua recusando ISO do Rio. O Classic 0.94.1 não mudou.
Esta etapa fez o Rio **aparecer e jogar** no emulador (PCSX2) com os dados gerados do APK do Rio, pela primeira vez.

## Adicionado
- Aparecem agora as caixas de metal, as correntes e o abacaxi: o leitor de WebP (`tools/webp.py`) foi ligado ao recorte de sprites (`tools/rio/crop_sprites.py`). Os 76 sprites das 30 fases do episódio 1 saem do APK; antes faltavam 6.
- Sprites dos 4 pássaros que o runtime conhece (vermelho, azul pequeno, amarelo e preto), em três estados (normal, voo, colisão), e a tabela `bird_sprites` em `ps2/src/rio/definition_sprites.h`.
- `docs/rio/MODIFICACOES_RIO_0.35.0.md`.

## Editado
- **Cores dos sprites:** o banco do Rio guardava a transparência de 0 a 255, e o PS2 usa 0 a 128 (o Classic já convertia). Os sprites saíam com tom vermelho/rosado; agora saem com as cores certas.
- **Câmera das fases:** `tools/rio/rio_campaign.py` calcula a visão pela mesma regra do Classic (a fase inteira na área de jogo). Antes as fases ficavam fora da tela.
- **Tema:** cada fase do Rio aponta para o tema 0 (`theme2`, o galpão), então o fundo do galpão passa a ser desenhado.
- `ps2/src/main.c` (só no ramo do Rio): o pássaro é desenhado pelo sprite do seu tipo e do seu estado.
- `tools/rio/extract_sprites_rio.py` e `pack_sprites_rio.py`.

## Excluído
- `docs/rio/MODIFICACOES_RIO_0.34.0.md` (e a cópia do Drive), substituído por este.

## Resultado no emulador
- ELF do Rio (`make GAME=rio`) com os dados das 30 fases de Smugglers' Den: abre a seleção de fases, carrega a Level190, mostra o galpão, as caixas, as gaiolas e a fila de pássaros, lança os pássaros, a física roda e os pontos sobem.
- O disparo foi simulado por um ELF de teste (não faz parte do repositório); o menu foi visto, mas não percorrido com o controle.

## O que falta
1. Vitória por libertar gaiolas (`levelGoal`): ver se a fase termina ao soltar as 5 gaiolas; o pássaro solto ainda não é desenhado.
2. Gravidade (ainda +20 provisório) e parâmetros reais dos pássaros do Rio.
3. `UI.BIN`, `FONT.BIN` e `MENU.BIN` do Rio (o jogo usa fontes e menus provisórios) e música da fase.
4. Pássaros que o runtime não conhece (branco, Terence, etc.), macacos, Luiz, chefes e os outros 43 pacotes de fases.
5. Gerador do Rio de ponta a ponta (`gerar_iso.py --jogo rio`): hoje os dados vêm de `tools/rio/build_rio_data.py`, que depende da análise do APK feita antes (`--investigar`).
